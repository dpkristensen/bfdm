/**
    BFDP Parse Command

    Copyright 2023, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define BFDP_MODULE "App::CmdParse"

// Base Includes
#include "App/Commands.hpp"

// External Includes
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

// Internal Includes
#include "App/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Stream/RawStream.hpp"
#include "Bfdp/Unicode/Common.hpp"
#include "BfsdlParser/Objects/Database.hpp"
#include "BfsdlParser/Objects/IObject.hpp"
#include "BfsdlParser/Objects/Property.hpp"
#include "BfsdlParser/Objects/Tree.hpp"
#include "BfsdlParser/StreamParser.hpp"

using Bfdp::Console::ArgParser;
using Bfdp::Console::Msg;
using Bfdp::Console::Param;

namespace App
{

    using BfsdlParser::Objects::BitBase;
    using BfsdlParser::Objects::BfsdlVersionType;
    using BfsdlParser::Objects::Database;
    using BfsdlParser::Objects::DatabasePtr;
    using BfsdlParser::Objects::Endianness;
    using BfsdlParser::Objects::Field;
    using BfsdlParser::Objects::FieldPtr;
    using BfsdlParser::Objects::IObjectPtr;
    using BfsdlParser::Objects::ObjectType;
    using BfsdlParser::Objects::Property;
    using BfsdlParser::Objects::PropertyPtr;
    using BfsdlParser::Objects::Tree;
    using BfsdlParser::Objects::TreePtr;

    namespace CmdValidateSpecInternal
    {

    }
    using namespace CmdValidateSpecInternal;

    class StreamDataObserver
        : public Bfdp::Stream::IStreamObserver
    {
        BFDP_OVERRIDE( Bfdp::Stream::Control::Type OnStreamData
            (
            Bfdp::BitManip::GenericBitStream& aInBitStream
            ) )
        {
            // TODO: Parse the fields instead of just dumping the bytes to stdout
            while( aInBitStream.GetBitsTillEnd() )
            {
                uint8_t value;
                if( !aInBitStream.ReadBits(&value, 8) )
                {
                    return Bfdp::Stream::Control::Error;
                }
                std::cout << "0x" << std::hex << std::setw(2) << std::setfill( '0' ) << (uint32_t)value << std::dec << std::endl;
            }
            return Bfdp::Stream::Control::Continue;
        }
    };

    int CmdParse
        (
        Context& aContext,
        int const aArgC,
        char const* const* const aArgV
        )
    {
        SavedParamMap args;

        ArgParser parser = ArgParser()
            .SetName( APP_NAME " " APP_CMD_PARSE_NAME )
            .SetPrologue( APP_CMD_PARSE_DESC )
            .AddHelp()
            .Add( Param::CreateLong( "spec", 's' )
                    .SetDescription( "Path to specification file" )
                    .SetValueName( "spec_file" )
                    .SetCallback( SaveToParamMap )
                    .SetUserdataPtr( &args )
                )
            .Add( Param::CreateLong( "data", 'd' )
                    .SetDescription( "Path to data file (- := stdin)" )
                    .SetDefault( "", "data_file" )
                    .SetCallback( SaveToParamMap )
                    .SetUserdataPtr( &args )
                )
            .Add( Param::CreateLong( "format", 'f' )
                    .SetDescription( "Format of input data" )
                    .SetDefault( "raw", "format" )
                    .SetCallback( SaveToParamMap )
                    .SetUserdataPtr( &args )
                );

        int ret = parser.Parse( aArgV, aArgC );
        if( 0 != ret )
        {
            // Error logged by the parser
            parser.PrintHelp( stdout );
            return ret;
        }

        std::string specFileName = args["spec"];
        std::string dataFileName = args["data"];
        std::fstream dataFileStream;
        if( dataFileName.empty() )
        {
            dataFileName = "<stdin>";
            dataFileStream = std::fstream( stdin );
        }
        else
        {
            dataFileStream.open( dataFileName, std::ios::in | std::ios::binary );
        }

        if( !dataFileStream )
        {
            aContext.Log( stderr, Msg( "Failed to open " ) << dataFileName, Context::LogLevel::Problem );
            return 1;
        }

        // Validate the input format and create a data stream
        std::string format_str = args["format"];
        Bfdp::Stream::StreamPtr streamPtr = nullptr;
        StreamDataObserver streamDataObserver;
        if( format_str == "raw" )
        {
            streamPtr = std::make_shared< Bfdp::Stream::RawStream >( dataFileName, dataFileStream, streamDataObserver );
        }
        if( !streamPtr )
        {
            aContext.Log( stderr, Msg( "Invalid stream format '" ) << format_str << "'", Context::LogLevel::Problem );
            return 1;
        }
        if( !streamPtr->IsValid() )
        {
            aContext.Log( stderr, Msg( "Stream format '" ) << format_str << "' setup failure", Context::LogLevel::Problem );
            return 1;
        }

        // Create a database to receive objects discovered from the stream
        DatabasePtr db = Database::Create();
        if( !db )
        {
            aContext.Log( stderr, Msg( "Failed to create Database" ), Context::LogLevel::Problem );
            return -1;
        }

        // Set Filename property for future reference
        PropertyPtr fileNameProp = Property::StaticCast
            (
            db->GetRoot()->Add( std::make_shared< Property >( "Filename" ) )
            );
        if( !fileNameProp ||
            !fileNameProp->SetString( specFileName ) )
        {
            aContext.Log( stderr, Msg( "Failed to set Filename property" ), Context::LogLevel::Problem );
            return -1;
        }

        std::fstream specStream( specFileName.c_str(), std::ios::in | std::ios::binary );
        if( !specStream.is_open() )
        {
            aContext.Log( stderr, Msg( "Failed to open " ) << specFileName, Context::LogLevel::Problem );
            return 1;
        }
        else
        {
            aContext.Log( stdout, Msg( "Processing BFSDL Stream..." ) << specFileName, Context::LogLevel::Debug );
            ret = BfsdlParser::ParseStream( db->GetRoot(), specStream, 4096 );
            specStream.close();
        }

        // If the BFSDL stream is loaded, parse the data stream
        if( ret == 0 )
        {
            aContext.Log( stdout, Msg( "Processing data stream " ) << dataFileName << " as '" << format_str << "'", Context::LogLevel::Debug );
            if( !streamPtr->ReadStream() || streamPtr->HasError() )
            {
                aContext.Log( stderr, Msg( "Binary data stream parse failure" ), Context::LogLevel::Problem );
                // TODO: Print parse context from Stream object
                ret = 1;
            }
            aContext.Log( stdout, Msg( "Total: " ) << streamPtr->GetTotalProcessedStr(), Context::LogLevel::Info );
        }

        return ret;
    }

} // namespace App

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
#include <algorithm>
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
#include "BfsdlParser/Objects/NumericField.hpp"
#include "BfsdlParser/Objects/NumericValueBuilder.hpp"
#include "BfsdlParser/Objects/Property.hpp"
#include "BfsdlParser/Objects/Tree.hpp"
#include "BfsdlParser/StreamParser.hpp"


namespace App
{

    using Bfdp::Console::ArgParser;
    using BfsdlParser::Objects::BitBase;
    using BfsdlParser::Objects::BfsdlVersionType;
    using Bfdp::Stream::Control;
    using BfsdlParser::Objects::Database;
    using BfsdlParser::Objects::DatabasePtr;
    using BfsdlParser::Objects::Endianness;
    using BfsdlParser::Objects::Field;
    using BfsdlParser::Objects::FieldPtr;
    using BfsdlParser::Objects::FieldType;
    using Bfdp::BitManip::GenericBitStream;
    using BfsdlParser::Objects::IObjectPtr;
    using Bfdp::Console::Msg;
    using BfsdlParser::Objects::NumericField;
    using BfsdlParser::Objects::NumericFieldProperties;
    using BfsdlParser::Objects::NumericFieldPtr;
    using BfsdlParser::Objects::NumericValueBuilder;
    using BfsdlParser::Objects::ObjectType;
    using Bfdp::Console::Param;
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
    public:
        StreamDataObserver
            (
            Context& aContext
            )
            : mContext( aContext )
            , mFieldIsComplete( false )
        {
        }

        //! Set the pointer to the field root.
        void SetRoot
            (
            TreePtr& aRoot
            )
        {
            mFrameStack.clear();
            mFrameStack.emplace_back( aRoot );

            Frame& rootFrame = mFrameStack.back();
            aRoot->IterateFields( &StreamDataObserver::AddFieldsToFrame, &rootFrame );
            // Reset the end iterator, in case of ambiguity in C++03 and incomplete
            // implementation of C++11 by compilers.
            rootFrame.mCurFieldIter = rootFrame.mFields.end();
        }

    private:
        typedef std::list< FieldPtr > FieldList;
        struct Frame
        {
            Frame
                (
                TreePtr& aTree
                )
                : mFields()
                , mCurFieldIter( mFields.end() )
                , mTree( aTree )
            {
            }

            FieldList mFields;
            FieldList::iterator mCurFieldIter;
            TreePtr mTree;
        };
        typedef std::list< Frame > FrameStack;

        static void AddFieldsToFrame
            (
            FieldPtr& aField,
            void* const aArg
            )
        {
            Frame* frame = reinterpret_cast< Frame* >( aArg );
            frame->mFields.push_back( aField );
        }

        BFDP_OVERRIDE( Control::Type OnStreamData
            (
            GenericBitStream& aInBitStream
            ) )
        {
            if( mFrameStack.empty() )
            {
                // Should always leave the top frame loaded to avoid churning
                // through the database re-loading it all the time; so this
                // likely means SetRoot() wasn't called.
                BFDP_INTERNAL_ERROR( "Failed to get current frame" );
                return Control::Error;
            }
            Frame& curFrame = mFrameStack.back();
            // Handle end of frame
            while( curFrame.mCurFieldIter == curFrame.mFields.end() )
            {
                if( mFrameStack.size() > 1 ) {
                    // End of a non-root frame, pop the stack to the parent.
                    mFrameStack.pop_back();
                    curFrame = mFrameStack.back();
                }
                else
                {
                    // Root frame; just cycle back to the first field.
                    curFrame.mCurFieldIter = curFrame.mFields.begin();
                    if( curFrame.mCurFieldIter == curFrame.mFields.end() )
                    {
                        // Most likely cause is that the spec did not define any fields.
                        // Stop with an error to avoid an infinite loop.
                        mContext.Log( stderr, Msg( "No fields to parse" ), Context::LogLevel::Problem );
                        return Control::Error;
                    }
                }
            }

            while( aInBitStream.GetBitsTillEnd() )
            {
                FieldPtr curField = *curFrame.mCurFieldIter;
                Control::Type fieldRet = Control::Continue;
                switch( curField->GetFieldType() )
                {
                    case FieldType::Numeric:
                        fieldRet = Parse( *NumericField::StaticCast( curField ), aInBitStream );
                        break;

                    case FieldType::String:
                    case FieldType::Unknown:
                    default:
                        mContext.Log( stderr, Msg( "Failed to parse " ) << curField->GetTypeStr() << " field " << curField->GetName(), Context::LogLevel::Problem );
                        fieldRet = Control::Error;
                        break;
                }
                if( fieldRet != Control::Continue )
                {
                    return fieldRet;
                }
                else if( !mFieldIsComplete )
                {
                    break;
                }
                curFrame.mCurFieldIter++;

                // Reset per-field parsing state
                mFieldIsComplete = false;
                mNumericValueBuilder.Reset();

                if( curFrame.mCurFieldIter == curFrame.mFields.end() ) {
                    // Reached the end of the frame; continue to the next.
                    return Control::Continue;
                }
            }
            return Control::Continue;
        }

        Control::Type Parse
            (
            NumericField const& aField,
            GenericBitStream& aInBitStream
            )
        {
            if( !mNumericValueBuilder.HasProperties() )
            {
                if( !mNumericValueBuilder.SetFieldProperties( aField.GetNumericFieldProperties() ) )
                {
                    mContext.Log( stderr, Msg( "Unsupported field " ) << aField.GetTypeStr() << " " << aField.GetName(), Context::LogLevel::Problem );
                    return Control::Error;
                }
            }
            size_t bitsToRead = std::min( aInBitStream.GetBitsTillEnd(), mNumericValueBuilder.GetBitsTillComplete() );
            if( bitsToRead == 0 )
            {
                return Control::Continue;
            }
            uint64_t uintValue = 0;
            if( !aInBitStream.ReadBits(reinterpret_cast< Bfdp::Byte* >( &uintValue ), bitsToRead ) )
            {
                mContext.Log( stderr, Msg( "Failed to read " ) << aField.GetName(), Context::LogLevel::Problem );
                return Control::Error;
            }
            if( !mNumericValueBuilder.ParseBits( uintValue, bitsToRead ) )
            {
                mContext.Log( stderr, Msg( "Failed to parse " ) << aField.GetName(), Context::LogLevel::Problem );
                return Control::Error;
            }
            if( mNumericValueBuilder.IsComplete() )
            {
                // If complete, dump value and mark complete
                // TODO: Should have a FixedPointNumber class that encapsulates the value, makes it pretty, etc...
                if( mNumericValueBuilder.IsSigned() )
                {
                    std::cout << aField.GetName() << "=" << mNumericValueBuilder.GetRawS64() << std::endl;
                }
                else
                {
                    std::cout << aField.GetName() << "=" << mNumericValueBuilder.GetRawU64() << std::endl;
                }
                mFieldIsComplete = true;
            }
            // Either way, continue
            return Control::Continue;
        }

        Context& mContext;
        bool mFieldIsComplete;
        FrameStack mFrameStack;
        NumericValueBuilder mNumericValueBuilder;
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
        StreamDataObserver streamDataObserver( aContext );
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
            if( ret != 0 )
            {
                // If the BFSDL stream is not loaded, stop early
                return ret;
            }
        }

        streamDataObserver.SetRoot( db->GetRoot() );
        Endianness::Type defaultBitOrder = db->GetRoot()->GetNumericPropertyWithDefault< Endianness::Type >( "DefaultBitOrder", Endianness::Default );
        Endianness::Type defaultByteOrder = db->GetRoot()->GetNumericPropertyWithDefault< Endianness::Type >( "DefaultByteOrder", Endianness::Default );
        if( Endianness::Little != defaultBitOrder )
        {
            aContext.Log( stderr, Msg( "Unsupported DefaultBitOrder" ), Context::LogLevel::Problem );
            // TODO: Add support in Stream (hint: probably need to use something other than GenericBitStream... )
            return 1;
        }
        if( Endianness::Little != defaultByteOrder )
        {
            aContext.Log( stderr, Msg( "Unsupported DefaultByteOrder" ), Context::LogLevel::Problem );
            // TODO: Add support in Stream (hint: probably need to use something other than GenericBitStream... )
            return 1;
        }

        aContext.Log( stdout, Msg( "Processing data stream " ) << dataFileName << " as '" << format_str << "'", Context::LogLevel::Debug );
        if( !streamPtr->ReadStream() || streamPtr->HasError() )
        {
            aContext.Log( stderr, Msg( "Binary data stream parse failure" ), Context::LogLevel::Problem );
            // TODO: Print parse context from Stream object
            ret = 1;
        }
        aContext.Log( stdout, Msg( "Total: " ) << streamPtr->GetTotalProcessedStr(), Context::LogLevel::Info );

        return ret;
    }

} // namespace App

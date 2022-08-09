/**
    BFDP Validate Spec Command

    Copyright 2022, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#define BFDP_MODULE "App::CmdValidateSpec"

// Base Includes
#include "App/Commands.hpp"

// External Includes
#include <fstream>
#include <memory>

// Internal Includes
#include "App/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
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
        static void DumpField
            (
            Context& aContext,
            FieldPtr const aField
            );

        static void DumpObject
            (
            IObjectPtr const aObject,
            void* const aArg
            );

        static void DumpProperty
            (
            Context& aContext,
            PropertyPtr const aProperty
            );

        static bool gIsTestMode = false;

        static void DumpField
            (
            Context& aContext,
            FieldPtr const aField
            )
        {
            std::stringstream ss;
            ss << "FIELD " << aField->GetName();
            aContext.Log( stdout, Msg( ss.str() ), Context::LogLevel::Info );
        }

        static void DumpObject
            (
            IObjectPtr const aObject,
            void* const aArg
            )
        {
            Context* context = reinterpret_cast< Context* >( aArg );

            switch( aObject->GetType() )
            {
            case ObjectType::Property:
                DumpProperty( *context, Property::StaticCast( aObject ) );
                break;

            case ObjectType::Field:
                DumpField( *context, Field::StaticCast( aObject ) );
                break;

            case ObjectType::Tree:
            case ObjectType::Count:
            default:
                break;
            }
        }

        static void DumpProperty
            (
            Context& aContext,
            PropertyPtr const aProperty
            )
        {
            std::stringstream ss;

            ss << "PROP " << aProperty->GetName() << "=";

            if( aProperty->GetName() == "Version" )
            {
                BfsdlVersionType version = 0;
                if( aProperty->GetNumericValue( version ) )
                {
                    ss << version;
                }
                else
                {
                    ss << "<invalid>";
                }
            }
            else if(
                ( aProperty->GetName() == "DefaultBitOrder" ) ||
                ( aProperty->GetName() == "DefaultByteOrder" ) )
            {
                Endianness::Type endianness = Endianness::Default;
                if( aProperty->GetNumericValue( endianness ) )
                {
                    if( endianness == Endianness::Little )
                    {
                        ss << "LE";
                    }
                    else if( endianness == Endianness::Big )
                    {
                        ss << "BE";
                    }
                    else
                    {
                        ss << "<invalid> (" << endianness << ")";
                    }
                }
                else
                {
                    ss << "<invalid>";
                }
            }
            else if( aProperty->GetName() == "BitBase" )
            {
                BitBase::Type bitBase = BitBase::Default;
                if( aProperty->GetNumericValue( bitBase ) )
                {
                    ss << bitBase;
                }
                else
                {
                    ss << "<invalid>";
                }
            }
            else if( aProperty->GetName() == "Filename" )
            {
                if( gIsTestMode )
                {
                    ss << "<valid>";
                }
                else
                {
                    ss << aProperty->GetString();
                }
            }

            aContext.Log( stdout, Msg( ss.str() ), Context::LogLevel::Info );
        }

    }
    using namespace CmdValidateSpecInternal;

    int CmdValidateSpec
        (
        Context& aContext,
        int const aArgC,
        char const* const* const aArgV
        )
    {
        SavedParamMap args;

        ArgParser parser = ArgParser()
            .SetName( APP_NAME " " APP_CMD_VALIDATE_SPEC_NAME )
            .SetPrologue( APP_CMD_VALIDATE_SPEC_DESC )
            .AddHelp()
            .Add
                (
                Param::CreateLong( "file", 'f' )
                    .SetDescription( "Path to specification file" )
                    .SetValueName( "filename" )
                    .SetCallback( SaveToParamMap )
                    .SetUserdataPtr( &args )
                )
            .Add
                (
                Param::CreateLong( "testing", 't' )
                    .SetDescription( "Format output for system tests" )
                    .SetOptional()
                    .SetCallback
                        ( // Lambda
                        [] (
                            ArgParser const& aParser,
                            Param const& aParam,
                            std::string const& aValue,
                            uintptr_t const aUserdata
                            )
                        {
                            BFDP_UNUSED_PARAMETER( aParser );
                            BFDP_UNUSED_PARAMETER( aParam );
                            BFDP_UNUSED_PARAMETER( aValue );
                            BFDP_UNUSED_PARAMETER( aUserdata );
                            gIsTestMode = true;
                            return 0;
                        } )
                );

        int ret = parser.Parse( aArgV, aArgC );
        if( 0 != ret )
        {
            // Error logged by the parser
            parser.PrintHelp( stdout );
            return ret;
        }

        std::string specFile = args["file"];
        if( !gIsTestMode )
        {
            aContext.Log( stdout, Msg( "File: " ) << specFile, Context::LogLevel::Info );
        }

        // Create a database to receive objects discovered from the stream
        DatabasePtr db = Database::Create();
        if( !db )
        {
            BFDP_RUNTIME_ERROR( "Failed to create Database" );
            return -1;
        }

        // Set Filename property for future reference
        PropertyPtr fileNameProp = Property::StaticCast
            (
            db->GetRoot()->Add( std::make_shared< Property >( "Filename" ) )
            );
        if( !fileNameProp ||
            !fileNameProp->SetString( specFile ) )
        {
            BFDP_RUNTIME_ERROR( "Failed to set Filename property" );
            return -1;
        }

        std::fstream fs( specFile.c_str(), std::ios::in | std::ios::binary );
        if( !fs.is_open() )
        {
            BFDP_RUNTIME_ERROR( "Cannot open file" );
            ret = 1;
        }
        else
        {
            ret = BfsdlParser::ParseStream( db->GetRoot(), fs, 4096 );
            fs.close();
        }

        db->Iterate( DumpObject, &aContext );

        return ret;
    }

}

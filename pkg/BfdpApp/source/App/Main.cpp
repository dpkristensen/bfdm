/**
    BFDP Console Application Entry Point

    Copyright 2020, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#define BFDP_MODULE "App::Main"

// Base Includes
#include "App/Main.h"

// External Includes
#include <cstdio>
#include <string>
#include <sstream>

// Internal Includes
#include "App/Commands.hpp"
#include "App/Context.hpp"
#include "Bfdp/Console/ArgParser.hpp"
#include "Bfdp/Console/Msg.hpp"
#include "Bfdp/Console/Printer.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"

using namespace App;
using Bfdp::Console::ArgParser;
using Bfdp::Console::Param;

static void OnInternalError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    );

static void OnMisuseError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    );

static void OnRunTimeError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    );

static Context gContext = Context();

int BFDP_app_main
    (
    int const argc,
    char const* const* const argv
    )
{
    int ret = 0;

    Bfdp::ErrorReporter::SetInternalErrorHandler( OnInternalError );
    Bfdp::ErrorReporter::SetMisuseErrorHandler( OnMisuseError );
    Bfdp::ErrorReporter::SetRunTimeErrorHandler( OnRunTimeError );

    ArgParser parser = ArgParser()
        .SetName( APP_NAME )
        .SetPrologue( "Binary Format Data Parser" )
        .AddHelp()
        .Add
            (
            Param::CreateLong( "verbose", 'v' )
                .SetDescription( "Increase verbosity level (stacks up to 2 times)" )
                .SetCounter()
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
                        gContext.IncreaseLogLevel();
                        return 0;
                    } )
            )
        .Add
            (
            Param::CreateCommand()
                .SetDescription( "Command to execute" )
            );

    std::stringstream cmdText;
    cmdText << "Commands:" << std::endl
        << "    help - Show this help text" << std::endl
        << "    " APP_CMD_PARSE_NAME " - " APP_CMD_PARSE_DESC
        << "    " APP_CMD_VALIDATE_SPEC_NAME " - " APP_CMD_VALIDATE_SPEC_DESC;
    parser.SetEpilogue( cmdText.str() );

    ret = parser.Parse( argv, argc );
    int cmdIdx = parser.GetParseIndex();

    if( 0 != ret )
    {
        // Error logged by the parser
        parser.PrintHelp( stdout );
    }
    else if( cmdIdx >= argc )
    {
        BFDP_INTERNAL_ERROR( "No command found" );
        ret = 1;
    }
    else
    {
        std::string cmd = argv[cmdIdx];

        if( cmd == "help" )
        {
            parser.PrintHelp( stdout );
        }
        else if( cmd == "tests" )
        {
            // Undocumented command to run internal smoke tests
            BFDP_INTERNAL_ERROR( "Test Internal Error");
            BFDP_MISUSE_ERROR( "Test Misuse Error");
            BFDP_RUNTIME_ERROR( "Test RunTime Error");
        }
        else if( cmd == APP_CMD_PARSE_NAME )
        {
            ret = CmdParse( gContext, argc - cmdIdx, &argv[cmdIdx] );
        }
        else if( cmd == APP_CMD_VALIDATE_SPEC_NAME )
        {
            ret = CmdValidateSpec( gContext, argc - cmdIdx, &argv[cmdIdx] );
        }
        else
        {
            BFDP_MISUSE_ERROR( "Invalid command" );
            ret = 1;
        }
    }

    return ret;
}

static void OnInternalError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    )
{
    std::stringstream ss;

    ss << "Misuse Error: " << aModuleName;
    if( gContext.IsVerbose( Context::LogLevel::Debug ) )
    {
        ss << "@" << aLine;
    }
    ss << " " << aErrorText;
    gContext.Log( stderr, ss.str(), Context::LogLevel::Info );
}

static void OnMisuseError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    )
{
    std::stringstream ss;

    ss << "Misuse Error: " << aModuleName;
    if( gContext.IsVerbose( Context::LogLevel::Debug ) )
    {
        ss << "@" << aLine;
    }
    ss << " " << aErrorText;
    gContext.Log( stderr, ss.str(), Context::LogLevel::Info );
}

static void OnRunTimeError
    (
    char const* const aModuleName,
    unsigned int const aLine,
    char const* const aErrorText
    )
{
    std::stringstream ss;

    ss << "RunTime Error: " << aModuleName;
    if( gContext.IsVerbose( Context::LogLevel::Debug ) )
    {
        ss << "@" << aLine;
    }
    ss << " " << aErrorText;
    gContext.Log( stderr, ss.str() );
}

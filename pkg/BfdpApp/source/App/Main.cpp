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

// Base Includes
#include "App/Main.h"

// External Includes
#include <string>

// Internal Includes
#include "App/Context.hpp"
#include "Bfdp/Console/Msg.hpp"
#include "Bfdp/Console/Printer.hpp"

using namespace App;

static void PrintHelp();

static int ProcGlobalOption
    (
    Context& aContext,
    std::string const& aOption
    );

static int ProcCommand
    (
    Context& aContext,
    std::string const& aCommand,
    char const* const* const aArgs,
    int const aNumArgs
    );

int BFDP_app_main
    (
    int const argc,
    char const* const* const argv
    )
{
    int ret = 0;
    Context ctx;

    char const* cmdString = "help";
    char const* const* cmdArgList = NULL;
    int cmdArgCount = 0;

    for( int i = 1; ( 0 == ret ) && ( i < argc ); ++i )
    {
        char const* argData = argv[i];
        if( argData[0] == '-' )
        {
            ret = ProcGlobalOption( ctx, std::string( &argData[1] ) );
            if( ret != 0 )
            {
                return ret;
            }
        }
        else
        {
            cmdString = argData;
            cmdArgList = &argv[i + 1];
            cmdArgCount = argc - i - 1;
            break;
        }
    }

    ret = ProcCommand( ctx, cmdString, cmdArgList, cmdArgCount );

    return ret;
}

static void PrintHelp()
{
    using Bfdp::Console::Indent;
    using Bfdp::Console::Msg;
    Bfdp::Console::Printer out;

    out << "Binary Format Data Parser"
        << ""
        << "bfdp <options> <command>"
        << "";

    out << "options:";
    {
        Indent indentOptions( out );
        {
            Indent indentOptionsVerbose( out, "-v,--verbose", "" );
            out << "Specify verbosity of output messages (stacks up to 2 times)"
                << "Default: Problems only";
        }
    }

    out << "command: help";
    {
        Indent indentHelp( out );
        out.Print( "Print this help text" );
    }
}

static int ProcGlobalOption
    (
    Context& aContext,
    std::string const& aOption
    )
{
    int ret = 0;

    // Long-form option
    if( aOption[0] == '-' )
    {
        if( aOption == "-verbose" )
        {
            aContext.IncreaseLogLevel();
        }
        else
        {
            aContext.Error( "Invalid option: -%s", aOption );
            ret = 1;
        }

        return ret;
    }

    // Short-form options
    for( size_t i = 0; ( 0 == ret ) && ( i < aOption.size() ); ++i )
    {
        switch( aOption[i] )
        {
            case 'v':
                aContext.IncreaseLogLevel();
                break;

            default:
                aContext.Error( "Invalid option: %c", aOption[i] );
                ret = 1;
                break;
        }
    }

    return ret;
}

static int ProcCommand
    (
    Context& aContext,
    std::string const& aCommand,
    char const* const* const /* aArgs */,
    int const /* aNumArgs */
    )
{
    int ret = 0;
    if( aCommand == "help" )
    {
        PrintHelp();
    }
    else if( aCommand == "tests" )
    {
        // Undocumented command to run internal smoke tests
        aContext.Debug( "Test Debug");
        aContext.Info( "Test Info");
        aContext.Warn( "Test Warn");
        aContext.Error( "Test Error");
    }
    else
    {
        aContext.Error( "Invalid command: %s", aCommand );
        ret = 1;
    }

    return ret;
}

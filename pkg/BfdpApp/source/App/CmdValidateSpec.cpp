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

// Internal Includes
#include "App/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "BfsdlParser/StreamParser.hpp"

using Bfdp::Console::ArgParser;
using Bfdp::Console::Msg;
using Bfdp::Console::Param;

namespace App
{

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
                );

        int ret = parser.Parse( aArgV, aArgC );
        if( 0 != ret )
        {
            // Error logged by the parser
            parser.PrintHelp( stdout );
            return ret;
        }

        std::string specFile = args["file"];
        aContext.Log( stdout, Msg( "File: " ) << specFile, Context::LogLevel::Info );

        std::fstream fs( specFile.c_str(), std::ios::in | std::ios::binary );
        if( !fs.is_open() )
        {
            BFDP_RUNTIME_ERROR( "Cannot open file" );
            ret = 1;
        }
        else
        {
            ret = BfsdlParser::ParseStream( specFile, fs, 4096 );
            fs.close();
        }

        return ret;
    }

}

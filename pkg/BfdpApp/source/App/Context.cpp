/**
    BFDP Console Application Context

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
#include "App/Context.hpp"

// External Includes
#include <cstdarg>

namespace App
{

    Context::Context()
        : mLogLevel( LogLevel::Problem )
    {
    }

    void Context::Debug( char const* const aFormat, ... )
    {
        if( mLogLevel >= LogLevel::Debug )
        {
            va_list args;
            va_start( args, aFormat );
            vsnprintf( mMsgBuf, sizeof( mMsgBuf ), aFormat, args );
            va_end( args );

            LogStr( stdout, "DEBUG", mMsgBuf );
        }
    }


    void Context::Error( char const* const aFormat, ... )
    {
        va_list args;
        va_start( args, aFormat );
        vsnprintf( mMsgBuf, sizeof( mMsgBuf ), aFormat, args );
        va_end( args );

        LogStr( stderr, "ERROR", mMsgBuf );
    }

    void Context::IncreaseLogLevel()
    {
        if( mLogLevel < LogLevel::Max )
        {
            ++mLogLevel;
        }
    }

    void Context::Info( char const* const aFormat, ... )
    {
        if( mLogLevel >= LogLevel::Info )
        {
            va_list args;
            va_start( args, aFormat );
            vsnprintf( mMsgBuf, sizeof( mMsgBuf ), aFormat, args );
            va_end( args );

            LogStr( stdout, "INFO", mMsgBuf );
        }
    }

    void Context::LogStr
        (
        FILE* const aFile,
        char const* const aTag,
        char const* const aMsg
        )
    {
        std::fputs( aTag, aFile );
        std::fputs( ": ", aFile );
        std::fputs( aMsg, aFile );
        std::fputc( '\n', aFile );
    }

    void Context::Warn( char const* const aFormat, ... )
    {
        va_list args;
        va_start( args, aFormat );
        vsnprintf( mMsgBuf, sizeof( mMsgBuf ), aFormat, args );
        va_end( args );

        LogStr( stderr, "WARNING", mMsgBuf );
    }

} // namespace App

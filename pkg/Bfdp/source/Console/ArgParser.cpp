/**
    BFDP Argument Parser Class

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

#define BFDP_MODULE "Console::ArgParser"

// Base Includes
#include "Bfdp/Console/ArgParser.hpp"

// External Includes
#include <sstream>

// Internal Includes
#include "Bfdp/Console/Msg.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace Console
    {

        ArgParser::ArgParser
            (
            Param::ArgumentCallback const aDefaultCallback,
            uintptr_t const aDefaultUserdata
            )
            : mCurCallback( nullptr )
            , mCurUserdata( 0U )
            , mDefaultCallback( aDefaultCallback )
            , mDefaultUserdata( aDefaultUserdata )
            , mKeepParsing( false )
            , mParseIndex( 0 )
            , mResult( 0 )
        {
        }

        ArgParser& ArgParser::Add
            (
            Param const& aParam
            )
        {
            mParams.push_back( aParam );
            return *this;
        }

        ArgParser& ArgParser::AddHelp()
        {
            mParams.push_back
                (
                Param::CreateLong( "help", 'h' )
                    .SetDescription( "Show this help text" )
                    .SetOptional()
                    .SetCallback
                        ( // lambda
                        [] (
                            ArgParser const& aParser,
                            Param const& aParam,
                            std::string const& aValue,
                            uintptr_t const aUserdata
                            )
                        {
                            BFDP_UNUSED_PARAMETER( aParam );
                            BFDP_UNUSED_PARAMETER( aValue );
                            BFDP_UNUSED_PARAMETER( aUserdata );
                            aParser.PrintHelp( stdout );
                            return 0;
                        } )
                );
            return *this;
        }

        std::string ArgParser::GetName() const
        {
            return mName;
        }

        int ArgParser::GetParseIndex() const
        {
            return mParseIndex;
        }

        int ArgParser::Parse
            (
            char const* const* const aArgV,
            int const aArgC,
            Param::ArgumentCallback const aCallback,
            uintptr_t const aUserdata
            )
        {
            if( ( nullptr == aArgV ) || ( nullptr == aArgV[0] ) )
            {
                BFDP_MISUSE_ERROR( "Null argument list" );
                mResult = 1;
                return mResult;
            }

            if( 0 == aArgC )
            {
                BFDP_MISUSE_ERROR( "Missing arg0" );
                mResult = 1;
                return mResult;
            }

            // Set parsing context variables
            mCurCallback = ( aCallback == nullptr ) ? mDefaultCallback : aCallback;
            mCurUserdata = ( aCallback == nullptr ) ? mDefaultUserdata : aUserdata;
            mKeepParsing = true;
            mResult = 0;
            mSkipNextArg = false;

            // Use the name found, if one is not specified otherwise
            bool useLearnedName = mName.empty();
            if( useLearnedName )
            {
                mName = aArgV[0];
            }

            // Build a list of parameter references to track which ones have been seen
            for( Param& iter : mParams )
            {
                mCurParams.push_back( &iter );
            }

            // Handle each parameter as it's observed; pass all remaining parameters for length detection
            for( mParseIndex = 1; mParseIndex < aArgC; ++mParseIndex )
            {
                if( nullptr == aArgV[mParseIndex] )
                {
                    BFDP_MISUSE_ERROR( "Null argument list" );
                    mResult = 1;
                    mKeepParsing = false;
                    break;
                }
                ProcArgs( aArgV[mParseIndex], &aArgV[mParseIndex + 1], aArgC - mParseIndex );
                if( mSkipNextArg )
                {
                    ++mParseIndex;
                    mSkipNextArg = false;
                }

                if( !mKeepParsing )
                {
                    // Break before the increment to preserve the index
                    break;
                }
            }

            // Loop over unvisited parameters
            int overrideErrorCode = 0;
            while( mKeepParsing && ( !mCurParams.empty() ) )
            {
                Param const* p = mCurParams.front();
                if( p->IsOptional() )
                {
                    // If the optional parameters have value, send the default along
                    if( p->HasValue() )
                    {
                        std::string defaultValue = p->GetDefaultValue();
                        char const* arg = defaultValue.c_str();
                        Dispatch( *p, &arg, 1 );
                    }
                    // Optional parameters without a value are
                }
                else
                {
                    // Missing mandatory parameters is an error
                    std::string errStr = ( Msg( "Missing <" ) << p->GetName() << ">" ).Get();
                    BFDP_RUNTIME_ERROR( errStr.c_str() );
                    overrideErrorCode = 1;
                    // Don't stop parsing, let's report all errors
                }
                // Remove the pointer - it's not owned by this list, so the Param itself is not freed.
                mCurParams.pop_front();
            }

            if( mResult == 0 )
            {
                mResult = overrideErrorCode;
            }

            if( useLearnedName )
            {
                // Clear the learned name
                mName.clear();
            }
            mCurParams.clear();
            mCurCallback = NULL;
            mCurUserdata = 0U;
            mKeepParsing = false;
            return mResult;
        }

        void ArgParser::PrintHelp
            (
            std::FILE* const aOut
            ) const
        {
            std::stringstream ss;

            // TODO: Legal notice
            // TODO: App description

            if( !mName.empty() )
            {
                ss << mName;
            }

            for( Param const& param : mParams )
            {
                ss << " " << param.GetUsage();
            }
            ss << std::endl;

            for( Param const& param : mParams )
            {
                // TODO: Need a line-based printer with indent levels and output wrapping
                ss << "  " << param.GetShortDescription() << std::endl;
                ss << "    " << param.GetDescription() << std::endl;
            }
            ss << std::endl;

            std::string s = ss.str();
            std::fputs( s.c_str(), aOut );
        }

        void ArgParser::SetName
            (
            std::string const& aName
            )
        {
            mName = aName;
        }

        void ArgParser::Dispatch
            (
            Param const& aParam,
            char const* const* const aArgV,
            int const aArgC
            )
        {
            std::string value;
            if( aParam.HasValue() )
            {
                if( aArgC == 0 )
                {
                    std::string msg = ( Msg( "Argument " ) << aParam.GetName() << " requires <" << aParam.GetValueName() << ">!" ).Get();
                    BFDP_RUNTIME_ERROR( msg.c_str() );
                    mResult = 1;
                    mKeepParsing = false;
                    return;
                }

                value = aArgV[0];
                mSkipNextArg = true;
            }
            else if( aParam.IsPositional() )
            {
                // Positional arguments' values are the data
                value = aArgV[0];
            }

            Param::ArgumentCallback paramCallback = aParam.GetCallback();
            if( paramCallback )
            {
                mResult = paramCallback( *this, aParam, value, aParam.GetUserdata() );
                mKeepParsing = ( mResult == 0 );
            }
            else if( mCurCallback )
            {
                mResult = mCurCallback( *this, aParam, value, mCurUserdata );
                mKeepParsing = ( mResult == 0 );
            }

            if( aParam.IsTerminator() )
            {
                // Always stop parsing on terminators
                mKeepParsing = false;
            }
        }

        Param const* ArgParser::GetNextMatchingArg
            (
            std::string const& aText,
            bool const aLongForm,
            bool const aShortForm
            )
        {
            for( ParamPtrList::iterator iter = mCurParams.begin(); iter != mCurParams.end(); ++iter )
            {
                Param const* param = *iter;

                // Match the short name first, as it's likely faster
                if( aShortForm && param->MatchShortName( aText ) )
                {
                    mCurParams.erase( iter );
                    return param;
                }
                else if( aLongForm && param->MatchLongName( aText ) )
                {
                    mCurParams.erase( iter );
                    return param;
                }
            }

            return nullptr;
        }

        Param const* ArgParser::GetNextPositional()
        {
            for( ParamPtrList::iterator iter = mCurParams.begin(); iter != mCurParams.end(); ++iter )
            {
                Param const* param = *iter;

                if( param->IsPositional() )
                {
                    mCurParams.erase( iter );
                    return param;
                }
            }

            return nullptr;
        }

        void ArgParser::ProcArgs
            (
            std::string const& aText,
            char const* const* const aArgV,
            int const aArgC
            )
        {
            if( aText[0] == '/' )
            {
                // PC-style argument, short or long form, but always singular
                Param const* arg = GetNextMatchingArg( aText.substr( 1 ), true, true );
                if( arg == NULL )
                {
                    std::string matchErr = ( Msg( "Unknown argument " ) << aText ).Get();
                    BFDP_RUNTIME_ERROR( matchErr.c_str() );
                    mKeepParsing = false;
                    mResult = 1;
                    return;
                }

                Dispatch( *arg, aArgV, aArgC );
            }
            else if( aText[0] == '-' )
            {
                // Short or long form
                if( aText[1] == '-' )
                {
                    if( aText[2] == '\0' )
                    {
                        // Stop parsing on -- in case the caller wants to do something with the
                        // rest of the command line; not an error!
                        mKeepParsing = false;
                        return;
                    }

                    // Long form - singular
                    Param const* arg = GetNextMatchingArg( aText.substr( 2 ), true, false );
                    if( arg == NULL )
                    {
                        // Special case, maybe it's --arg=value?
                        size_t eqPos = aText.find( '=' );
                        if( eqPos != std::string::npos )
                        {
                            arg = GetNextMatchingArg( aText.substr( 2, eqPos ), true, false );
                        }
                    }

                    if( arg == NULL )
                    {
                        std::string matchErr = ( Msg( "Unknown argument " ) << aText ).Get();
                        BFDP_RUNTIME_ERROR( matchErr.c_str() );
                        mKeepParsing = false;
                        mResult = 1;
                        return;
                    }
                    Dispatch( *arg, aArgV, aArgC );
                }
                else
                {
                    size_t const numShorts = aText.size();
                    if( numShorts == 0 )
                    {
                        BFDP_RUNTIME_ERROR( "Missing argument name after -" );
                        mKeepParsing = false;
                        mResult = 1;
                        return;
                    }

                    // Short form - plural
                    for( size_t i = 1; ( i < numShorts ) && mKeepParsing; ++i )
                    {
                        Param const* arg = GetNextMatchingArg( std::string( 1, aText[i] ), false, true );
                        if( arg == nullptr )
                        {
                            std::string matchErr = ( Msg( "Unknown argument -" ) << aText[i] ).Get();
                            BFDP_RUNTIME_ERROR( matchErr.c_str() );
                            mKeepParsing = false;
                            mResult = 1;
                            return;
                        }

                        if( i < numShorts )
                        {
                            // Only the last argument gets access to the subsequent parameters
                            // The others get nothing
                            Dispatch( *arg, nullptr, 0 );
                        }
                        else
                        {
                            Dispatch( *arg, aArgV, aArgC );
                        }
                    }
                }
            }
            else
            {
                // Not a switched argument, try to match a positional
                Param const* arg = GetNextPositional();
                if( arg == nullptr )
                {
                    std::string posArgErr = ( Msg( "Unknown argument " ) << aText ).Get();
                    BFDP_RUNTIME_ERROR( posArgErr.c_str() );
                    mKeepParsing = false;
                    mResult = 1;
                    return;
                }
                char const* const posValue = aText.c_str();
                Dispatch( *arg, &posValue, 1 );
            }
        }

    } // namespace Console

} // namespace Bfdp

/**
    BFSDL Parse Position Definitions

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

// Base Includes
#include "BfsdlParser/ParsePosition.hpp"

// External Includes
#include <algorithm>
#include <iomanip>
#include <sstream>

// Internal Includes
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "ParsePosition"

namespace BfsdlParser
{

    // Use internal namespace to avoid ODR violations
    namespace ParsePositionInternal
    {
        static void AddByteToStream
            (
            std::ostream& aOut,
            Bfdp::Byte const aByte
            )
        {
            if( ( aByte >= 0x20 ) && ( aByte <= 0x7e ) )
            {
                aOut << static_cast< char >( aByte );
            }
            else
            {
                aOut << "\\x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast< unsigned int >( aByte ) << std::dec;
            }
        }
    }
    using namespace ParsePositionInternal;

    ParsePosition::ParsePosition
        (
        std::string const& aName,
        size_t const aPretextLen,
        size_t const aPosttextLen
        )
        : mNewlineChar( '\0' )
        , mCurLineNumber( 1U )
        , mName( aName )
        , mRemainderSize( 0U )
    {
        // Try to allocate; worst case it fails and there's no context to print
        if( !mContextWindow.Init( aPretextLen ) )
        {
            BFDP_RUNTIME_ERROR( "Failed to allocate; context will be missing." );
        }
        else if( !mRemainderBuf.Allocate( aPosttextLen ) )
        {
            BFDP_RUNTIME_ERROR( "Failed to allocate; posttext could be incomplete." );
        }
    }

    size_t ParsePosition::GetContextBeginColumn() const
    {
        return mContextWindow.GetBeginCounter();
    }

    size_t ParsePosition::GetContextPositionOffset() const
    {
        return mContextWindow.GetSize();
    }

    size_t ParsePosition::GetCurColNumber() const
    {
        return mContextWindow.GetEndCounter() + 1;
    }

    size_t ParsePosition::GetCurLineNumber() const
    {
        return mCurLineNumber;
    }

    std::string const& ParsePosition::GetName() const
    {
        return mName;
    }

    std::string ParsePosition::GetPrintableContext() const
    {
        std::stringstream ss;
        size_t ctxSize = mContextWindow.GetSize();
        for( size_t i = 0; i < ctxSize; ++i )
        {
            AddByteToStream( ss, mContextWindow.GetByte( i ) );
        }
        for( size_t i = 0; i < mRemainderSize; ++i )
        {
            Bfdp::Byte b = mRemainderBuf[i];
            if( ( b == 0x0d ) || ( b == 0x0a ) )
            {
                // Stop at the first newline character in the remainder
                break;
            }

            AddByteToStream( ss, b );
        }
        return ss.str();
    }

    void ParsePosition::ProcessNewData
        (
        Bfdp::Byte const* const aInData,
        size_t const aInSize
        )
    {
        mRemainderSize = 0; // Clear out remainder on new data, as it is no longer valid

        for( size_t i = 0; i < aInSize; ++i )
        {
            switch( aInData[i] )
            {
                case 0x0a:
                case 0x0d:
                    if( mNewlineChar == 0x00 )
                    {
                        // Detect the first newline character as the one to count
                        mNewlineChar = aInData[i];
                    }
                    if( aInData[i] == mNewlineChar )
                    {
                        // Start counting the next line
                        mContextWindow.Reset();
                        ++mCurLineNumber;
                    }
                    // Newline characters don't get pushed to the window
                    break;

                default:
                    // Add everything else to the context window
                    mContextWindow.PushData( &aInData[i], 1U );
                    break;
            }
        }
    }

    void ParsePosition::ProcessRemainderData
        (
        Bfdp::Byte const* const aInData,
        size_t const aInSize
        )
    {
        // Overwrite the remainder with the first part of whatever we're given
        mRemainderSize = std::min( aInSize, mRemainderBuf.GetSize() );
        mRemainderBuf.CopyFrom( aInData, mRemainderSize );
    }

} // namespace BfsdlParser

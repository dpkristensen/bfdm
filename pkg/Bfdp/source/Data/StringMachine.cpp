/**
    BFDP Data StringMachine Definitions

    Copyright 2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/Data/StringMachine.hpp"

// External Includes
#include <ios>

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"

#define BFDP_MODULE "Data::StringMachine"

namespace Bfdp
{

    namespace Data
    {

        namespace StringMachineInternal
        {
            static std::streampos const POS_BEGIN = std::streampos( 0 );
        }

        using namespace StringMachineInternal;

        StringMachine::StringMachine()
            : mDefined( false )
        {
        }

        bool StringMachine::AppendString
            (
            Unicode::IConverter& aConverter,
            std::string const& aIn
            )
        {
            Unicode::Utf8Converter utf8Converter;
            ByteBuffer utf8Buffer;
            if( !utf8Buffer.Allocate( utf8Converter.GetMaxBytes() ) )
            {
                BFDP_RUNTIME_ERROR( "Out of memory while converting bytes" );
                return false;
            }

            Byte const* inPtr = reinterpret_cast< Byte const* >( aIn.c_str() );
            size_t inBytesLeft = aIn.length();

            std::ostringstream oss;
            Unicode::CodePoint cp;
            while( inBytesLeft )
            {
                // Convert input format -> Unicode
                size_t bytesConverted = aConverter.ConvertBytes( inPtr, inBytesLeft, cp );
                inBytesLeft -= bytesConverted;
                inPtr += bytesConverted;
                if( 0 == bytesConverted )
                {
                    return false;
                }

                // Convert Unicode -> UTF8
                bytesConverted = utf8Converter.ConvertSymbol( cp, utf8Buffer.GetPtr(), utf8Buffer.GetSize() );
                if( 0 == bytesConverted )
                {
                    return false;
                }
                oss << utf8Buffer.GetString( bytesConverted );
            }

            AppendUtf8( oss.str() );

            return true;
        }

        bool StringMachine::AppendUnicode
            (
            Unicode::CodePoint const aCodePoint
            )
        {
            Unicode::Utf8Converter utf8Converter;
            ByteBuffer utf8Buffer;
            if( !utf8Buffer.Allocate( utf8Converter.GetMaxBytes() ) )
            {
                BFDP_RUNTIME_ERROR( "Out of memory while converting bytes" );
                return false;
            }

            // Convert Unicode -> output format
            size_t bytesConverted = utf8Converter.ConvertSymbol( aCodePoint, utf8Buffer.GetPtr(), utf8Buffer.GetSize() );
            if( 0 == bytesConverted )
            {
                return false;
            }

            AppendUtf8( utf8Buffer.GetString( bytesConverted ) );

            return true;
        }

        void StringMachine::AppendUtf8
            (
            std::string const& aValue
            )
        {
            mBuffer << aValue;
            SetDefined();
        }

        bool StringMachine::GetString
            (
            Unicode::IConverter& aConverter,
            std::string& aOut
            ) const
        {
            Unicode::Utf8Converter utf8Converter;
            ByteBuffer utf8Buffer;
            ByteBuffer outBuffer;
            if( ( !utf8Buffer.Allocate( utf8Converter.GetMaxBytes() ) ) ||
                ( !outBuffer.Allocate( aConverter.GetMaxBytes() ) ) )
            {
                BFDP_RUNTIME_ERROR( "Out of memory while converting bytes" );
                return false;
            }

            Pack();

            Byte const* inPtr = reinterpret_cast< Byte const* >( mCache.c_str() );
            size_t inBytesLeft = mCache.length();

            std::ostringstream oss;
            Unicode::CodePoint cp;
            while( inBytesLeft )
            {
                // Convert UTF8 -> Unicode
                size_t bytesConverted = utf8Converter.ConvertBytes( inPtr, inBytesLeft, cp );
                inBytesLeft -= bytesConverted;
                inPtr += bytesConverted;
                if( 0 == bytesConverted )
                {
                    return false;
                }

                // Convert Unicode -> output format
                bytesConverted = aConverter.ConvertSymbol( cp, outBuffer.GetPtr(), outBuffer.GetSize() );
                if( 0 == bytesConverted )
                {
                    return false;
                }
                oss << outBuffer.GetString( bytesConverted );
            }

            aOut = oss.str();
            return true;
        }

        std::string StringMachine::GetUtf8HexString
            (
            std::string const& aSeparator,
            std::string const& aPrefix
            ) const
        {
            std::ostringstream oss;
            char digit[3];
            std::string sep;

            Pack();
            digit[2] = '\0';
            for( std::string::const_iterator iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                char c = *iter;
                digit[0] = GetHex( ( c >> 4 ) & 0xF );
                digit[1] = GetHex( c & 0xF );
                oss << sep << aPrefix << digit;
                sep = aSeparator;
            }

            return oss.str();
        }

        std::string const& StringMachine::GetUtf8String() const
        {
            Pack();
            return mCache;
        }

        bool StringMachine::IsDefined() const
        {
            return mDefined;
        }

        bool StringMachine::IsEmpty() const
        {
            return mCache.empty() && ( mBuffer.tellp() == POS_BEGIN );
        }

        void StringMachine::Reset()
        {
            mDefined = false;
            mCache.clear();
            mBuffer.str() = std::string();
        }

        void StringMachine::SetDefined()
        {
            mDefined = true;
        }

        char StringMachine::GetHex
            (
            char const aNibble
            ) const
        {
            if( IsWithinRange< char >( 0, aNibble, 9 ) )
            {
                return aNibble + '0';
            }
            else if( IsWithinRange< char >( 10, aNibble, 16 ) )
            {
                return aNibble - 10 + 'a';
            }
            return '?';
        }

        void StringMachine::Pack() const
        {
            if( mBuffer.tellp() != POS_BEGIN )
            {
                mCache = mCache + mBuffer.str();
                mBuffer.clear();
                mBuffer.str( "" );
            }
        }

    } // namespace Data

} // namespace Bfdp

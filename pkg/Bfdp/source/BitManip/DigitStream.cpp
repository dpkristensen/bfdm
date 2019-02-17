/**
    BFDP Bitmanip DigitStream Definitions

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
#include "Bfdp/BitManip/DigitStream.hpp"

// Internal Includes
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Macros.hpp"

#define BFDP_MODULE "BitManip::DigitStream"

namespace Bfdp
{

    namespace BitManip
    {

        DigitStream::DigitStream()
            : mRadix( InvalidRadix )
        {

        }

        RadixType DigitStream::GetRadix() const
        {
            return mRadix;
        }

        std::string DigitStream::GetStr()
        {
            SizeT bitsPerDigit = BitManip::GetRadixBits( mRadix );
            BFDP_RETURNIF_V( bitsPerDigit == 0, std::string() );
            BFDP_RETURNIF_VA
                (
                0 != ( mBuffer.GetDataBits() % bitsPerDigit ),
                std::string(),
                "Short buffer"
                );

            // Need enough characters for one digit per symbol
            std::string digits( mBuffer.GetDataBits() / bitsPerDigit, '\0' );
            BitManip::GenericBitStream bs( mBuffer );

            Byte value = 0;
            SizeT i = 0;
            while( bs.ReadBits( &value, bitsPerDigit ) )
            {
                char symbol;
                if( !BitManip::ConvertBase( mRadix, value, symbol ) )
                {
                    return std::string();
                }

                digits[i] = symbol;
                ++i;
            }

            if( i != digits.length() )
            {
                BFDP_INTERNAL_ERROR( "Failed to read bits" );
                return std::string();
            }

            return digits;
        }

        bool DigitStream::Set
            (
            std::string const aDigits,
            RadixType const aRadix
            )
        {
            SizeT bitsPerDigit = BitManip::GetRadixBits( aRadix );
            BFDP_RETURNIF_V( bitsPerDigit == 0, false );

            BitBuffer buffer;

            // Need enough bits for one symbol per digit
            buffer.ResizeNoPreserve( aDigits.length() * bitsPerDigit );
            BitManip::GenericBitStream bs( buffer );

            bool ok = true;
            std::string::const_iterator iter = aDigits.begin();
            for( ; iter != aDigits.end(); ++iter )
            {
                UInt8 value;
                ok = BitManip::ConvertBase( aRadix, *iter, value );
                if( !ok )
                {
                    break;
                }

                ok = bs.WriteBits( &value, bitsPerDigit );
                if( !ok )
                {
                    BFDP_INTERNAL_ERROR( "Failed to write bits" );
                    break;
                }
            }

            // Trim up the buffer to reflect the number of data bits used
            if( !buffer.ResizePreserve( ok ? bs.GetPosBits() : 0U ) )
            {
                return false;
            }

            if( ok )
            {
                mRadix = aRadix;
                mBuffer = buffer;
            }
            return ok;
        }

    } // namespace BitManip

} // namespace Bfdp
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
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Macros.hpp"

#define BFDP_MODULE "BitManip::DigitStream"

namespace Bfdp
{

    namespace BitManip
    {

        bool Digiterator::IsDone() const
        {
            return ( mBitsPerDigit == 0 ) || ( mStream.GetBitsTillEnd() < mBitsPerDigit );
        }

        unsigned int Digiterator::ReadDigit() const
        {
            unsigned int out = 0U;
            Byte* outBuf = reinterpret_cast< Byte* >( &out );

            if( ( mBitsPerDigit == 0 ) ||
                ( !mStream.ReadBits( outBuf, mBitsPerDigit ) ) )
            {
                return 0U;
            }

            return out;
        }

        Digiterator::Digiterator
            (
            BitBuffer& aBuffer,
            size_t const aBitsPerDigit
            )
            : mBitsPerDigit( aBitsPerDigit )
            , mStream( aBuffer )
        {
            if( BitsToBytes( aBitsPerDigit ) > sizeof( unsigned int ) )
            {
                // This should never happen...  But seek to the end of the
                // stream to avoid stack corruption in case of misuse or bugs.
                BFDP_INTERNAL_ERROR( "unsigned int too narrow" );
                mStream.SeekBits( mStream.GetBitsTillEnd() );
            }
        }

        DigitStream::DigitStream()
            : mRadix( Data::InvalidRadix )
        {
        }

        Digiterator DigitStream::GetIterator() const
        {
            return Digiterator( const_cast< BitBuffer& >( mBuffer ), Data::GetRadixBits( mRadix ) );
        }

        size_t DigitStream::GetNumDigits() const
        {
            BFDP_RETURNIF_V( mRadix == Data::InvalidRadix, 0U );
            size_t bitsPerDigit = Data::GetRadixBits( mRadix );
            return mBuffer.GetDataBits() / bitsPerDigit;
        }

        Data::RadixType DigitStream::GetRadix() const
        {
            return mRadix;
        }

        bool DigitStream::GetUint64
            (
            uint64_t& aOut
            ) const
        {
            aOut = 0;
            Digiterator iter = GetIterator();

            if( Data::IsRadixPowerOf2( mRadix ) )
            {
                // Optimization for powers of 2: use simple operations
                BFDP_RETURNIF_V( mBuffer.GetDataBits() > 64U, false );

                size_t const shift = Data::GetRadixBits( mRadix );
                while( !iter.IsDone() )
                {
                    aOut <<= shift;
                    aOut += iter.ReadDigit();
                }
            }
            else
            {
                // For others: use the REALLY SLOW method...
                while( !iter.IsDone() )
                {
                    uint64_t temp = aOut;
                    unsigned int digit = iter.ReadDigit();

                    temp *= mRadix;
                    if( ( ( temp / mRadix ) != aOut ) ||
                        ( ( UINT64_MAX - temp ) < digit ) )
                    {
                        // Overflow
                        return false;
                    }
                    aOut = temp + digit;
                }
            }

            return true;
        }

        bool DigitStream::IsDefined() const
        {
            return mRadix != Data::InvalidRadix;
        }

        std::string DigitStream::GetStr() const
        {
            size_t bitsPerDigit = Data::GetRadixBits( mRadix );
            BFDP_RETURNIF_V( bitsPerDigit == 0, std::string() );
            BFDP_RETURNIF_VA
                (
                0 != ( mBuffer.GetDataBits() % bitsPerDigit ),
                std::string(),
                "Short buffer"
                );

            // Need enough characters for one digit per symbol
            std::string digits( mBuffer.GetDataBits() / bitsPerDigit, '\0' );
            BitManip::GenericBitStream bs( const_cast< BitBuffer& >( mBuffer ) );

            Byte value = 0;
            size_t i = 0;
            while( bs.ReadBits( &value, bitsPerDigit ) )
            {
                char symbol;
                if( !Data::ConvertBase( mRadix, value, symbol ) )
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

            // Return "0" if no digits present
            if( digits.empty() )
            {
                digits = "0";
            }
            return digits;
        }

        void DigitStream::Reset()
        {
            mRadix = Data::InvalidRadix;
            BFDP_UNUSED_RETURN( mBuffer.ResizeNoPreserve( 0 ) );
        }

        bool DigitStream::Set
            (
            std::string const& aDigits,
            Data::RadixType const aRadix
            )
        {
            size_t bitsPerDigit = Data::GetRadixBits( aRadix );
            BFDP_RETURNIF_V( bitsPerDigit == 0, false );

            BitBuffer buffer;

            // Need enough bits for one symbol per digit
            buffer.ResizeNoPreserve( aDigits.length() * bitsPerDigit );
            BitManip::GenericBitStream bs( buffer );

            bool ok = true;
            std::string::const_iterator iter = aDigits.begin();
            for( ; iter != aDigits.end(); ++iter )
            {
                uint8_t value;
                ok = Data::ConvertBase( aRadix, *iter, value );
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

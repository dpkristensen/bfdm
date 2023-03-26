/**
    BFDP Unicode to UTF8 Converter

    Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

// Base includes
#include "Bfdp/Unicode/Utf8Converter.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/BitManip/Mask.hpp"
#include "Bfdp/Unicode/Functions.hpp"

#define BFDP_MODULE "Unicode::Utf8Converter"

namespace Bfdp
{

    namespace Unicode
    {

        namespace InternalUtf8Converter
        {

            // 1-byte 0xxxxxxx
            // 2-byte 110xxxxx 10xxxxxx
            // 3-byte 1110xxxx 10xxxxxx 10xxxxxx
            // 4-byte 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 5-byte 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 6-byte 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

            static size_t const ByteThreshold[] =
            {
                BitManip::CreateMask< size_t >( 7 ),
                BitManip::CreateMask< size_t >( 5 + 6 ),
                BitManip::CreateMask< size_t >( 4 + 6 + 6 ),
                BitManip::CreateMask< size_t >( 3 + 6 + 6 + 6 ),
                BitManip::CreateMask< size_t >( 2 + 6 + 6 + 6 + 6 ),
                BitManip::CreateMask< size_t >( 1 + 6 + 6 + 6 + 6 + 6 )
            };

            size_t CountHeaderBits
                (
                Byte const aByte
                )
            {
                size_t headerBitsSet = 0;
                for( size_t i = 8; i > 0; --i )
                {
                    uint8_t const mask = BitManip::CreateMask< uint8_t >( 1, ( i - 1 ) );
                    if( aByte & mask )
                    {
                        ++headerBitsSet;
                    }
                    else
                    {
                        break;
                    }
                }

                return headerBitsSet;
            }

        } // namespace InternalUtf8Converter

        using namespace InternalUtf8Converter;

        Utf8Converter::Utf8Converter()
        {
        }

        size_t Utf8Converter::ConvertBytes
            (
            Byte const* const aBytesIn,
            size_t const aByteCount,
            CodePoint& aSymbolOut
            )
        {
            if( ( NULL == aBytesIn ) ||
                ( 0 == aByteCount  ) )
            {
                BFDP_MISUSE_ERROR( "Invalid input for ConvertBytes()" );
                return 0;
            }

            // Read the number of 1's in the header starting from bit 7
            size_t bytesToRead = 0;
            size_t headerBitsSet = CountHeaderBits( aBytesIn[0] );
            if( headerBitsSet == 0 )
            {
                bytesToRead = 1;
            }
            else if( ( headerBitsSet <= 6 ) &&
                     ( headerBitsSet >= 2 ) )
            {
                bytesToRead = headerBitsSet;
            }
            else
            {
                // Invalid first byte
                return 0;
            }

            if( aByteCount < bytesToRead )
            {
                // Short sequence
                return 0;
            }

            // Read first byte
            uint8_t mask = BitManip::CreateMask< uint8_t >( 7 - headerBitsSet );
            CodePoint cp = static_cast< CodePoint >( aBytesIn[0] & mask );

            // Read subsequent bytes
            mask = BitManip::CreateMask< uint8_t >( 6 );
            for( size_t i = 0; i < ( bytesToRead - 1 ); ++i )
            {
                if( CountHeaderBits( aBytesIn[i + 1] ) != 1 )
                {
                    // Invalid subsequent byte
                    return 0;
                }

                // Add code bits
                cp <<= 6;
                cp |= ( aBytesIn[i + 1] & mask );
            }

            aSymbolOut = cp;
            return bytesToRead;
        }

        size_t Utf8Converter::ConvertSymbol
            (
            CodePoint const& aSymbolIn,
            Byte* const aBytesOut,
            size_t const aByteCount
            )
        {
            static uint8_t const byteNMask = BitManip::CreateMask< uint8_t >( 6 );

            if( ( NULL == aBytesOut          ) ||
                ( aByteCount < GetMaxBytes() ) )
            {
                BFDP_MISUSE_ERROR( "Invalid input for ConvertSymbol()" );
                return 0;
            }

            if( !IsValidCodePoint(aSymbolIn) )
            {
                // Ignore all invalid code points (Private Use and Non-Character values are accepted)
                return 0;
            }

            size_t numBytesToConvert = 0;
            while( numBytesToConvert < BFDP_COUNT_OF_ARRAY( ByteThreshold ) )
            {
                size_t const threshold = ByteThreshold[numBytesToConvert];
                ++numBytesToConvert;
                if( aSymbolIn <= threshold )
                {
                    break;
                }
            }

            if( aByteCount < numBytesToConvert )
            {
                BFDP_MISUSE_ERROR( "Not enough bytes for conversion" );
                return 0;
            }

            CodePoint cp = aSymbolIn;
            for( size_t i = numBytesToConvert; i != 0; --i )
            {
                Byte value;
                size_t bitsUsed;
                if( i == 1 )
                {
                    size_t maskBits = ( numBytesToConvert == 1 ) ? 7 : ( 7 - numBytesToConvert );
                    uint8_t byte1Mask = BitManip::CreateMask< uint8_t >( maskBits );
                    value = cp & byte1Mask;
                    bitsUsed = maskBits;
                    if( numBytesToConvert != 1 )
                    {
                        value |= BitManip::CreateMask< uint8_t >( numBytesToConvert, maskBits + 1 );
                    }
                }
                else
                {
                    value = ( cp & byteNMask ) | 0x80;
                    bitsUsed = 6;
                }
                cp >>= bitsUsed;
                aBytesOut[i - 1] = value;
            }

            if( cp != 0 )
            {
                BFDP_INTERNAL_ERROR( "Not all symbol bits accounted for" );
            }

            return numBytesToConvert;
        }

        size_t Utf8Converter::GetMaxBytes() const
        {
            return 6;
        }

        std::string Utf8Converter::GetTypeStr() const
        {
            return "utf8";
        }

    } // namespace Unicode

} // namespace Bfdp

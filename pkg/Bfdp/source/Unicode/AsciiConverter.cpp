/**
    BFDP Unicode to ASCII Converter

    Copyright 2018-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/Unicode/AsciiConverter.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/Private.hpp"

#define BFDP_MODULE "Unicode::AsciiConverter"

namespace Bfdp
{

    namespace Unicode
    {

        namespace InternalAscii
        {

            static ConversionTable const Conv[] =
            {
                // ASCII            Block   ASCII     ASCII
                // Code   Unicode   Len     Bytes     Range
                {   0,        0,   128,       1 }   // 0-127
            };

            static size_t const NumConv = BFDP_COUNT_OF_ARRAY( Conv );

        } // InternalAscii

        using namespace InternalAscii;

        AsciiConverter::AsciiConverter()
        {
        }

        /* virtual */ size_t AsciiConverter::ConvertBytes
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

            CodePoint ascii = static_cast< CodePoint >( aBytesIn[0] );
            return Transcode( false, Conv, NumConv, ascii, aSymbolOut );
        }

        /* virtual */ size_t AsciiConverter::ConvertSymbol
            (
            CodePoint const& aSymbolIn,
            Byte* const aBytesOut,
            size_t const aByteCount
            )
        {
            if( ( NULL == aBytesOut          ) ||
                ( aByteCount < GetMaxBytes() ) )
            {
                BFDP_MISUSE_ERROR( "Invalid input for ConvertSymbol()" );
                return 0;
            }

            CodePoint ascii;
            size_t bytesUsed = Transcode( true, Conv, NumConv, aSymbolIn, ascii );

            if( 0 != bytesUsed )
            {
                aBytesOut[0] = ( ascii & 0x7F );
            }

            return bytesUsed;
        }

        /* virtual */ size_t AsciiConverter::GetMaxBytes() const
        {
            return 1;
        }

    } // namespace Unicode

} // namespace Bfdp

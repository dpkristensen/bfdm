/**
    BFDP Unicode to Microsoft Code Page 1252 Converter

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
#include "Bfdp/Unicode/Ms1252Converter.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/Private.hpp"

#define BFDP_MODULE "Unicode::Ms1252Converter"

namespace Bfdp
{

    namespace Unicode
    {

        namespace InternalMs1252
        {

            static ConversionTable const Conv[] =
            {
                // 1252            Block    1252      1252
                // Code   Unicode   Len     Bytes     Range
                {   0,        0,   127,       1 },  // 0-126
                // not defined                      // 127
                { 128,     8364,     1,       1 },  // 128
                { 129,      129,     1,       1 },  // 129
                { 130,     8218,     1,       1 },  // 130
                { 131,      402,     1,       1 },  // 131
                { 132,     8222,     1,       1 },  // 132
                { 133,     8230,     1,       1 },  // 133
                { 134,     8224,     2,       1 },  // 134-135
                { 136,      710,     1,       1 },  // 136
                { 137,     8240,     1,       1 },  // 137
                { 138,      352,     1,       1 },  // 138
                { 139,     8249,     1,       1 },  // 139
                { 140,      338,     1,       1 },  // 140
                // not defined                      // 141
                { 142,      381,     1,       1 },  // 142
                // not defined                      // 143-144
                { 145,     8216,     2,       1 },  // 145-146
                { 147,     8220,     2,       1 },  // 147-148
                { 149,     8226,     1,       1 },  // 149
                { 150,     8211,     2,       1 },  // 150-151
                { 152,      732,     1,       1 },  // 152
                { 153,     8482,     1,       1 },  // 153
                { 154,      353,     1,       1 },  // 154
                { 155,     8250,     1,       1 },  // 155
                { 156,      339,     1,       1 },  // 157
                // not defined                      // 156
                { 158,      382,     1,       1 },  // 158
                { 159,      376,     1,       1 },  // 159
                { 160,      160,    96,       1 }   // 160-255
            };

            static size_t const NumConv = BFDP_COUNT_OF_ARRAY( Conv );

        } // InternalMs1252

        using namespace InternalMs1252;

        Ms1252Converter::Ms1252Converter()
        {
        }

        /* virtual */ size_t Ms1252Converter::ConvertBytes
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

            CodePoint ms1252 = reinterpret_cast< CodePoint const& >( aBytesIn[0] ) & 0xFF;
            return Transcode( false, Conv, NumConv, ms1252, aSymbolOut );
        }

        /* virtual */ size_t Ms1252Converter::ConvertSymbol
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

            CodePoint ms1252;
            size_t bytesUsed = Transcode( true, Conv, NumConv, aSymbolIn, ms1252 );

            if( 0 != bytesUsed )
            {
                aBytesOut[0] = ( ms1252 & 0xFF );
            }

            return bytesUsed;
        }

        /* virtual */ size_t Ms1252Converter::GetMaxBytes() const
        {
            return 1;
        }

    } // namespace Unicode

} // namespace Bfdp

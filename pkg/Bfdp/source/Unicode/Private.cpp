/**
    BFDP Unicode Private Definitions

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
#include "Bfdp/Unicode/Private.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        size_t Transcode
            (
            bool const aFromUnicode,
            ConversionTable const* const aTable,
            size_t const aNumEntries,
            CodePoint const aCodePointIn,
            CodePoint& aCodePointOut
            )
        {
            // Internal method; input checks omitted for performance

            size_t bytes = 0; // assume no match

            for( size_t i = 0; i < aNumEntries; ++i )
            {
                // 1. Determine which code to match the input against to find the conversion block
                ConversionTable const& entry = aTable[i];
                CodePoint const minTablePoint = aFromUnicode ? entry.unicodeValue : entry.otherValue;
                CodePoint const maxTablePoint = minTablePoint + entry.blockLen - 1;

                // 2. Find the block which matches this code
                if( ( minTablePoint <= aCodePointIn ) &&
                    ( aCodePointIn <= maxTablePoint ) )
                {
                    // 2.A. save data if found
                    bytes = aTable[i].otherBytes;
                    CodePoint const offset = aCodePointIn - minTablePoint;
                    CodePoint const transcodeBegin = aFromUnicode ? entry.otherValue : entry.unicodeValue;
                    aCodePointOut = transcodeBegin + offset;
                    break;
                }
            }

            // 3. Return the number of bytes the new code occupies
            return bytes;
        }

    } // namespace Unicode

} // namespace Bfdp

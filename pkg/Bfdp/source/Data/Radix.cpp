/**
    BFDP Data Radix Definitions

    Copyright 2016-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/Data/Radix.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace Data
    {

        bool ConvertBase
            (
            RadixType const aRadix,
            char const aChar,
            uint8_t& aValue
            )
        {
            BFDP_RETURNIF_V( !IsValidRadix( aRadix ), false );

            uint8_t digit = 0U;
            if( IsWithinRange( '0', aChar, '9' ) )
            {
                digit = static_cast< uint8_t >( aChar - '0' );
            }
            else if( IsWithinRange( 'a', aChar, 'z' ) )
            {
                digit = static_cast< uint8_t >( aChar - 'a' ) + 10U;
            }
            else if( IsWithinRange( 'A', aChar, 'Z' ) )
            {
                digit = static_cast< uint8_t >( aChar - 'A' ) + 10U;
            }
            else
            {
                BFDP_CTIME_ASSERT( MaxRadix == 36, "Unsupported conversion" );
                return false;
            }

            BFDP_RETURNIF_V( digit >= aRadix, false );

            aValue = digit;
            return true;
        }

        bool ConvertBase
            (
            RadixType const aRadix,
            uint8_t const aValue,
            char& aSymbol
            )
        {
            BFDP_RETURNIF_V( !IsValidRadix( aRadix ), false );
            BFDP_RETURNIF_V( aValue >= aRadix, false );

            char out = 0;
            if( IsWithinRange< uint8_t >( 0U, aValue, 9U ) )
            {
                out = '0' + static_cast< char >( aValue );
            }
            else if( IsWithinRange< uint8_t >( 10U, aValue, 35U ) )
            {
                out = 'a' + static_cast< char >( aValue - 10 );
            }
            else
            {
                BFDP_CTIME_ASSERT( MaxRadix == 36, "Unsupported conversion" );
                return false;
            }

            aSymbol = out;
            return true;
        }

        size_t GetRadixBits
            (
            RadixType const aRadix
            )
        {
            if( !IsValidRadix( aRadix ) )
            {
                return 0;
            }

            RadixType limit = 2;
            size_t bits = 1;
            while( aRadix > limit )
            {
                limit <<= 1U;
                ++bits;
            }

            return bits;
        }

        bool IsRadixPowerOf2
            (
            RadixType const aRadix
            )
        {
            BFDP_RETURNIF_V( !IsValidRadix( aRadix ), false );

            RadixType val = aRadix;
            while( ( val & 0x1 ) == 0 )
            {
                val >>= 1U;
            }

            return val == 1U;
        }

    } // namespace Data

} // namespace Bfdp

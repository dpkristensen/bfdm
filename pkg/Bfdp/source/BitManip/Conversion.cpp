/**
    BFDP BitManip Conversion Definitions

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
#include "Bfdp/BitManip/Conversion.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        bool ConvertBase
            (
            RadixType const aRadix,
            char const aChar,
            UInt8& aValue
            )
        {
            BFDP_RETURNIF_V( !IsValidRadix( aRadix ), false );

            UInt8 digit = 0;
            if( IsWithinRange( '0', aChar, '9' ) )
            {
                digit = static_cast< UInt8 >( aChar - '0' );
            }
            else if( IsWithinRange( 'a', aChar, 'z' ) )
            {
                digit = static_cast< UInt8 >( aChar - 'a' ) + 10;
            }
            else if( IsWithinRange( 'A', aChar, 'Z' ) )
            {
                digit = static_cast< UInt8 >( aChar - 'A' ) + 10;
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

        SizeT GetRadixBits
            (
            RadixType const aRadix
            )
        {
            if( !IsValidRadix( aRadix ) )
            {
                return 0;
            }

            RadixType limit = 2;
            SizeT bits = 1;
            while( aRadix > limit )
            {
                limit <<= 1U;
                ++bits;
            }

            return bits;
        }

    } // namespace Data

} // namespace Bfdp

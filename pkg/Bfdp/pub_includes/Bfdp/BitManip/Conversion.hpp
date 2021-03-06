/**
    BFDP BitManip Conversion Declarations

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

#ifndef Bfdp_BitManip_Conversion
#define Bfdp_BitManip_Conversion

// External Includes
#include <limits>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! Number of bits per byte
        //!
        //! @note Not using CHAR_BIT, since this is not intended to be architecture-specific
        static size_t const BitsPerByte = 8;

        //! Max number of bits (theoretical) for bitwise operations
        static size_t const MaxBits = std::numeric_limits< size_t >::max() - 7;

        //! Max number of bytes (theoretical) for bitwise operations
        static size_t const MaxBytes = MaxBits / BitsPerByte;

        //! @return The number of bytes needed to store aBits of data
        static inline size_t BitsToBytes
            (
            size_t const aBits
            )
        {
            if( aBits > MaxBits )
            {
                // Even though this will technically work, the reverse conversion would fail.  So
                // its best to avoid it in all cases.
                BFDP_MISUSE_ERROR_M( "Bit count too large", "BitManip::BitsToBytes" );
                return MaxBytes;
            }
            return ( aBits / 8 ) +
                ( ( ( aBits & 0x7 ) != 0 ) ? 1 : 0 );
        }

        //! @return The number of bytes needed to store aBits of data
        static inline size_t BytesToBits
            (
            size_t const aBytes
            )
        {
            if( aBytes > MaxBytes )
            {
                BFDP_MISUSE_ERROR_M( "Byte count too large", "BitManip::BytesToBits" );
                return MaxBits;
            }

            return aBytes * BitsPerByte;
        }

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_Conversion

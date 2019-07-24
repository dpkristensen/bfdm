/**
    BFDP Algorithm Calculation Definitions

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
#include "Bfdp/Algorithm/Calc.hpp"

namespace Bfdp
{

    namespace Algorithm
    {

        HashType FastHash
            (
            Byte const* const aData,
            size_t const aSize
            )
        {
            // Implements FNV-1a Hash (Fowler/Noll/Vo hash) for balance of speed, simplicity, and
            // low occurrence of collisions.  Algorithm adapted from
            // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
            // (yes, I know this is not a good citation :D).
            //
            // Credit: Glenn Fowler, Landon Curt Noll, and Kiem-Phong Vo
            //
            // TODO: This could probably be optimized on some CPUs using built-in hardware
            // implementations such as CRC-32-C.

            static HashType const FNV_prime = 0x1000193;
            static HashType const FNV_offset_bias = 0x811c9dc5;

            HashType hash = FNV_offset_bias;
            for( size_t i = 0; i < aSize; ++i )
            {
                hash *= FNV_prime;
                hash ^= aData[i];
            }
            return hash;
        }

    } // namespace Algorithm

} // namespace Bfdp

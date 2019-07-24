/**
    BFDP Algorithm Calculation Declarations

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

#ifndef Bfdp_Algorithm_Calc
#define Bfdp_Algorithm_Calc

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/String.hpp"

namespace Bfdp
{

    namespace Algorithm
    {

        typedef uint32_t HashType;

        typedef HashType (*HashFuncType)
            (
            Byte const* const aData,
            size_t const aSize
            );

        //! @note Callers should not rely on the implemented algorithm.
        //! @return A NON-CRYPTOGRAPHIC hash of the input buffer.
        HashType FastHash
            (
            Byte const* const aData,
            size_t const aSize
            );

        HashType inline FastHash
            (
            std::string const& aText
            )
        {
            return FastHash( Char( aText.c_str() ), aText.size() );
        }

    } // namespace Algorithm

} // namespace Bfdp

#endif // Bfdp_Algorithm_Calc

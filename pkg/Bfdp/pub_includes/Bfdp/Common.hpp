/**
    BFDP Common declarations

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

#ifndef Bfdp_Common
#define Bfdp_Common

// External includes
#include <stddef.h> // for size_t
#include <stdint.h> // for int types

#ifndef UINT8_MIN
    #define UINT8_MIN ( (uint8_t)0U )
#endif

#ifndef UINT16_MIN
    #define UINT16_MIN ( (uint16_t)0U )
#endif

#ifndef UINT32_MIN
    #define UINT32_MIN ( (uint32_t)0U )
#endif

#ifndef UINT64_MIN
    #define UINT64_MIN ( (uint64_t)0U )
#endif

namespace Bfdp
{

    template< class T >
    inline bool IsWithinRange
        (
        T const aMin,
        T const aValue1,
        T const aMax
        )
    {
        return ( aMin <= aValue1 ) && ( aValue1 <= aMax );
    }

} // namespace Bfdp

#endif // Bfdp_Common

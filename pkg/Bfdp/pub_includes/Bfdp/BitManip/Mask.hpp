/**
    BFDP Bitmanip Mask Declarations

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

#ifndef Bfdp_BitManip_Mask
#define Bfdp_BitManip_Mask

//! External includes
#include <limits>

//! Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! Create Mask
        //!
        //! @note This only works with unsigned types
        //! @return a bitmask containing aWidth consecutive bits set, beginning at bit aOffset
        template
            <
            class T
            >
        inline T CreateMask
            (
            SizeT const aWidth,
            SizeT const aOffset = 0
            )
        {
            BFDP_CTIME_ASSERT( !std::numeric_limits< T >::is_signed, Masks_Must_Be_Unsigned );
            if( aWidth == 0 )
            {
                return 0;
            }

            // This mask creation method is a little complex, but does not result in integer overflows.
            return static_cast< T >
                (
                ( static_cast< T >( ( ( static_cast< T >( 1U ) << static_cast< T >( aWidth - 1U ) ) - 1U ) << 1U ) + 1U ) << aOffset
                );
        }

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_Mask
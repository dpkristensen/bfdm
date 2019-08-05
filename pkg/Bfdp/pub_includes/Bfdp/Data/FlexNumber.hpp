/**
    BFDP Data FlexNumber Declarations

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

#ifndef Bfdp_Data_FlexNumber
#define Bfdp_Data_FlexNumber

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/DigitStream.hpp"
#include "Bfdp/Data/Sign.hpp"

namespace Bfdp
{

    namespace Data
    {

        //! Flexible Number
        //!
        //! This type stores numeric values with variable signed-ness, storage format, and value
        //! limits.  The three components are:
        //!
        //! * Significand
        //! * Base
        //! * Exponent
        //!
        //! This allows expressing a number as a combination of Scientific Notation and Exponential
        //! Notation:
        //!
        //! @code{.unparsed}
        //! <Significand> * (<Base> ^ <Exponent>)
        //!
        //! Scientific Notation: <significand> * (10 ^ <order>)
        //! Exponential Notation: <base> ^ <exponent>
        //! @endcode
        //!
        //! Each component is further composed of three parts:
        //!
        //! @code{.unparsed}
        //! <sign><integral part>.<fractional part>
        //! @endcode
        //!
        //! These components all use DigitStream, so each may have its own radix.  For sanity
        //! purposes, it is recommended to use the same radix for all three components.
        struct FlexNumber BFDP_FINAL
        {
            struct Component
            {
                bool IsDefined() const;

                std::string GetStr
                    (
                    bool const aVerbose = false
                    ) const;

                bool IsIntegral() const;

                void Reset();

                Sign sign;
                BitManip::DigitStream integral;
                BitManip::DigitStream fractional;
            };

            bool HasExponent() const;

            bool HasSignificand() const;

            bool IsDefined() const;

            bool IsIntegral() const;

            std::string GetStr
                (
                bool const aVerbose = false
                ) const;

            void Reset();

            Component significand;
            Component base;
            Component exponent;
        };

    } // namespace Data

} // namespace Bfdp

#endif // Bfdp_Data_FlexNumber

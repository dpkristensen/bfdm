/**
    BFDP Bitmanip DigitStream Declarations

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

#ifndef Bfdp_BitManip_DigitStream
#define Bfdp_BitManip_DigitStream

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/Data/Radix.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! Digit Stream
        //!
        //! This class encapsulates a bitstream of numeric digits
        class DigitStream BFDP_FINAL
        {
        public:
            DigitStream();

            Data::RadixType GetRadix() const;

            //! @return A string representation of the digits, or an empty
            //!     string on error.
            std::string GetStr();

            //! @return true if a value has been set
            bool IsDefined() const;

            //! Set the value of the string
            bool Set
                (
                std::string const aDigits,
                Data::RadixType const aRadix
                );

        private:
            BitBuffer mBuffer;
            Data::RadixType mRadix;
        };

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_DigitStream

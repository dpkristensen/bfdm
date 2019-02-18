/**
    BFSDL Parser Numeric Literal Declarations

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

#ifndef BfsdlParser_Objects_NumericLiteral
#define BfsdlParser_Objects_NumericLiteral

// External includes
#include <string>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Data/FlexNumber.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Numeric Literal
        //!
        //! Encapsulates storage of Numeric Literal data.
        //!
        //! @note Since this is used as a workspace for the parser to save data, the "Set" methods
        //!     are designed to be called as elements would be discovered.
        class NumericLiteral
        {
        public:
            NumericLiteral();

            std::string GetStr
                (
                bool const aVerbose
                );

            bool IsDefined() const;

            bool SetExponentDigits
                (
                std::string const aDigits
                );

            void SetExponentSign
                (
                Bfdp::Data::Sign const aSign
                );

            bool SetSignificandFractionalDigits
                (
                std::string const aDigits
                );

            bool SetSignificandIntegralDigits
                (
                std::string const aDigits
                );

            void SetSignificandSign
                (
                Bfdp::Data::Sign const aSign
                );

            void SetRadix
                (
                Bfdp::Data::RadixType const aRadix
                );

        private:
            //! FlexNumber stores all underlying data
            Bfdp::Data::FlexNumber mNumber;

            // Temporary value for radix
            Bfdp::Data::RadixType mRadix;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_NumericLiteral

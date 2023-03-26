/**
    BFSDL Parser Numeric Field Builder Declaration

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

#ifndef BfsdlParser_Objects_NumericFieldBuilder
#define BfsdlParser_Objects_NumericFieldBuilder

// Base Includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Objects/NumericField.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Numeric Field Builder
        //!
        //! Allows building properties for a NumericField incrementally
        class NumericFieldBuilder BFDP_FINAL
        {
        public:
            NumericFieldBuilder();

            NumericFieldPtr GetField
                (
                std::string const& aName
                ) const;

            //! @return true if the suffix was supplied, false otherwise.
            bool IsComplete() const;

            //! Parse the identifier
            //!
            //! This will be the text before a space or dot, consisting of numbers and/or letters.
            //!
            //! @return true if the identifier appears to be the beginning of a fixed-point
            //!     numeric type.
            bool ParseIdentifier
                (
                std::string const& aText
                );

            //! Parse the suffix
            //!
            //! This will be the text after a period, consisting of numbers and/or letters.
            //!
            //! @return true if the text is the fractional part of a fixed-point numeric type.
            bool ParseSuffix
                (
                std::string const& aText
                );

            void Reset();

            void SetBitBase
                (
                BitBase::Type const aBitBase
                );

        private:
            bool CalcBits
                (
                char const* aText,
                size_t& aOut
                );

            bool ParseSign
                (
                char const aChar
                );

            BitBase::Type mBitBase;

            bool mComplete;
            bool mIdentParsed;

            NumericFieldProperties mProps;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_NumericFieldBuilder

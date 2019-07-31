/**
    BFSDL Parser String Field Builder Declaration

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

#ifndef BfsdlParser_Objects_StringFieldBuilder
#define BfsdlParser_Objects_StringFieldBuilder

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Data/Tristate.hpp"
#include "Bfdp/Unicode/CodingMap.hpp"
#include "Bfdp/Unicode/Common.hpp"
#include "BfsdlParser/Objects/Common.hpp"
#include "BfsdlParser/Objects/NumericLiteral.hpp"
#include "BfsdlParser/Objects/StringField.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! String Field Builder
        //!
        //! Allows building properties for a NumericField incrementally
        class StringFieldBuilder BFDP_FINAL
        {
        public:
            StringFieldBuilder();

            //! Finalize the type
            //!
            //! @return true if the type is complete, false otherwise.
            bool Finalize();

            StringFieldPtr GetField
                (
                std::string const& aName
                ) const;

            //! Parse the identifier
            //!
            //! This will be 'string', or any derivative (ex: cstring, pstring).
            //!
            //! @return true if the identifier appears to be the beginning of a string type.
            bool ParseIdentifier
                (
                std::string const& aText
                );

            AttributeParseResult::Type ParseNumericAttribute
                (
                std::string const& aName,
                NumericLiteral const& aValue
                );

            AttributeParseResult::Type ParseStringAttribute
                (
                std::string const& aName,
                std::string const& aValue
                );

            void Reset();

            void SetDefaultCoding
                (
                Bfdp::Unicode::CodingId const aCode
                );

            void SetDefaultTermChar
                (
                Bfdp::Unicode::CodePoint const aTermChar
                );

        private:
            struct LengthType
            {
                // Type of length determination according to BFSDL Specification
                enum Id
                {
                    Bounded,    //!< Determined by the size of the parent stream or container
                    Fixed,      //!< len attribute (D.1.2)
                    Prefixed,   //!< plen attribute (D.1.5)

                    Unknown
                };
            };

            AttributeParseResult::Type SetCodeAttr
                (
                std::string const& aValue
                );

            AttributeParseResult::Type SetPlenAttr
                (
                size_t const aLength
                );

            AttributeParseResult::Type SetTermAttr
                (
                Bfdp::Unicode::CodePoint const aCodePoint
                );

            AttributeParseResult::Type SetUntermAttr
                (
                std::string const& aEmptyString = std::string()
                );

            std::string mAttrCode;

            bool mComplete;
            bool mError;
            bool mIdentParsed;

            Bfdp::Data::Tristate mAllowUnterminated;
            Bfdp::Unicode::CodingId mCode;
            LengthType::Id mLengthType;
            size_t mLengthValue;
            Bfdp::Unicode::CodePoint mTermChar;

            Bfdp::Unicode::CodingId mDefaultCode;
            Bfdp::Unicode::CodePoint mDefaultTermChar;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_StringFieldBuilder

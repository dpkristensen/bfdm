/**
    BFSDL Parser Numeric Literal Parser Definitions

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

#ifndef BfsdlParser_Token_NumericLiteralParser
#define BfsdlParser_Token_NumericLiteralParser

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// External includes
#include <string>

// Internal Includes
#include "BfsdlParser/Objects/NumericLiteral.hpp"
#include "BfsdlParser/Token/ITokenObserver.hpp"
#include "BfsdlParser/Token/ParseResult.hpp"
#include "BfsdlParser/Token/SymbolSequence.hpp"

namespace BfsdlParser
{

    namespace Token
    {

        //! Numeric Literal Parser
        //!
        //! This class encapsulates logic to parse a Numeric Literal
        class NumericLiteralParser
            : private Bfdp::NonAssignable
            , private Bfdp::NonCopyable
        {
        public:
            NumericLiteralParser
                (
                ITokenObserver& aObserver
                );

            //! @return The ParseResult indicating the progress of the operation
            ParseResult::Value GetParseResult() const;

            //! @return Immutable reference to parsed object
            Objects::NumericLiteral const& GetParsedObject() const;

            //! Parse symbols
            void ParseSymbols
                (
                SymbolSequence const& aSymbols
                );

            //! Reset the parser to prepare for a new parsing operation
            void Reset();

        private:
            void ParseControl
                (
                std::string const& aSymbol
                );

            void ParseDigits();

            void ParseHash();

            void ParsePeriod();

            void ParseRadix();

            void ParseSign
                (
                std::string const& aSymbol
                );

            void ParseTilde();

            void UseOrSetDefaultRadix();

            void UseOrSetDefaultExponentSign();

            void UseOrSetDefaultSignificandSign();

            bool mHasPeriod;

            //! Last result of parsing operations
            ParseResult::Value mLastParseResult;

            //! The Numeric Literal's current state (updated incrementally)
            Objects::NumericLiteral mNumericLiteral;

            //! Reference to observer
            ITokenObserver& mObserver;

            //! Buffer for unparsed text data
            std::string mUnparsedText;
        };

    } // namespace Token

} // namespace BfsdlParser

#endif // BfsdlParser_Token_NumericLiteralParser

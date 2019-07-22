/**
    BFSDL Parser String Literal Parser Definitions

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

#ifndef BfsdlParser_Token_StringLiteralParser
#define BfsdlParser_Token_StringLiteralParser

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// External includes
#include <string>

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Data/Radix.hpp"
#include "Bfdp/Data/StringMachine.hpp"
#include "Bfdp/StateMachine/Engine.hpp"
#include "Bfdp/Unicode/IConverter.hpp"
#include "BfsdlParser/Token/ITokenObserver.hpp"
#include "BfsdlParser/Token/ParseResult.hpp"
#include "BfsdlParser/Token/SymbolSequence.hpp"

namespace BfsdlParser
{

    namespace Token
    {

        //! String Literal Parser
        //!
        //! This class encapsulates logic to parse a String Literal
        class StringLiteralParser
            : private Bfdp::NonAssignable
            , private Bfdp::NonCopyable
        {
        public:
            StringLiteralParser
                (
                ITokenObserver& aObserver
                );

            //! @return The ParseResult indicating the progress of the operation
            ParseResult::Value GetParseResult() const;

            //! @return The parsed object
            Bfdp::Data::StringMachine const& GetParsedObject() const;

            //! @return whether the parser initialized successfully.
            bool IsInitOk() const;

            //! Parse symbols
            void ParseSymbols
                (
                SymbolSequence const& aSymbols
                );

            //! Reset the parser to prepare for a new parsing operation
            void Reset();

        private:
            struct StateVariables
            {
                StateVariables();

                void Reset();

                Bfdp::Unicode::GetUnicodeFunc escapeConverter;

                //! Number of digits to expect in the escape sequence
                size_t escapeDigits;

                //! Radix of data in the escape sequence
                Bfdp::Data::RadixType escapeRadix;

                //! Buffer for escape sequence characters
                Bfdp::Data::StringMachine escapeSequence;

                //! Number of symbols in escapeSequence
                size_t escapeSymbolCount;

                SymbolSequence symbols;
            };

            bool ParseEscapeChar();

            ParseResult::Value ParseEscapeCodePoint
                (
                Bfdp::Data::RadixType const aRadix
                );

            void ParseEscapeDigits();

            void StateBackslashEvaluate();

            void StateEscapeDigitsEvaluate();

            void StateTextEntry();

            void StateTextEvaluate();

            //! Whether initialization was performed successfully
            bool mInitOk;

            //! Last result of parsing operations
            ParseResult::Value mLastParseResult;

            //! Intermediate values used by the state machine
            StateVariables mState;

            //! State machine engine to use for algorithm control
            Bfdp::StateMachine::Engine mStateMachine;

            //! The String Literal's current state (updated incrementally)
            Bfdp::Data::StringMachine mStringLiteral;

            //! Reference to observer
            ITokenObserver& mObserver;
        };

    } // namespace Token

} // namespace BfsdlParser

#endif // BfsdlParser_Token_StringLiteralParser

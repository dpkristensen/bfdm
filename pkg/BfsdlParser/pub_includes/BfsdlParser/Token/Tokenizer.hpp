/**
    BFSDL Parser Tokenizer Declarations

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

#ifndef BfsdlParser_Token_Tokenizer
#define BfsdlParser_Token_Tokenizer

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// Internal Includes
#include "Bfdp/Lexer/ISymbolObserver.hpp"
#include "Bfdp/Lexer/StaticSymbolBuffer.hpp"
#include "Bfdp/Lexer/Symbolizer.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/StateMachine/Engine.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"
#include "BfsdlParser/Objects/NumericLiteral.hpp"
#include "BfsdlParser/Token/ITokenObserver.hpp"
#include "BfsdlParser/Token/NumericLiteralParser.hpp"

namespace BfsdlParser
{

    namespace Token
    {

        //! Tokenizer
        //!
        //! This class uses the Lexer to assemble context-insensitive tokens, which are the most
        //! basic items to be parsed from the BFSDL Stream.  The tokens represent text content
        //! which is identifiable according to the BFSDL Specification in a context-free manner.
        //!
        //! Tokens are the most identifiable piece of information possible in any context, but do
        //! not necessarily represent proper grammar of a statement or usage of the token in the
        //! appropriate context.
        class Tokenizer
            : private Bfdp::Lexer::ISymbolObserver
            , private Bfdp::NonAssignable
            , private Bfdp::NonCopyable
        {
        public:
            Tokenizer
                (
                BfsdlParser::Token::ITokenObserver& aObserver
                );

            //! End Parsing
            //!
            //! Notify the Tokenizer that no more Parse() calls are expected
            void EndParsing();

            //! @return whether the Tokenizer initialized successfully.
            bool IsInitOk() const;

            //! Parse a chunk of data
            //!
            //! @return true if parsing should continue, false otherwise.
            bool Parse
                (
                Bfdp::Byte const * aBytes, //!< [in] Pointer to bytes to read, must not be NULL
                Bfdp::SizeT aNumBytes, //!< [in] Number of bytes pointed to by aBytes, must be > 0
                Bfdp::SizeT& aBytesRead //!< [out] Number of bytes read from aBytes
                );

        private:
            struct StateVariables
            {
                StateVariables();

                Bfdp::SInt inputCategory;
                std::string inputSymbol;
                bool keepParsing;
            };

            //! Define the length of the longest token, which is a class name
            static Bfdp::SizeT const MAX_TOKEN_LENGTH = 256;

            //! @copydoc Lexer::ISymbolizer::OnMappedSymbol
            BFDP_OVERRIDE( bool OnMappedSymbol
                (
                Bfdp::SInt aCategory,
                std::string const& aSymbol
                ) );

            //! @copydoc Lexer::ISymbolizer::OnUnmappedSymbol
            BFDP_OVERRIDE( bool OnUnmappedSymbol
                (
                std::string const& aSymbol
                ) );

            // States
            void StateMainSequenceEvaluate();
            void StateNumericLiteralEntry();
            void StateNumericLiteralEvaluate();

            //! Converter for ASCII (initial format)
            Bfdp::Unicode::AsciiConverter mAsciiConverter;

            //! Whether initialization was performed successfully
            bool mInitOk;

            //! Parser for handling Numeric Literals
            NumericLiteralParser mNumericLiteralParser;

            //! Reference to observer
            ITokenObserver& mObserver;

            //! Whether a parsing error occurred
            bool mParseError;

            // Values used by the state machine
            StateVariables mState;

            //! Symbol buffer; this is fixed to the maximum length of any token
            Bfdp::Lexer::StaticSymbolBuffer< MAX_TOKEN_LENGTH > mSymbolBuffer;

            //! Symbolizer to use; this must be declared after mSymbolBuffer.
            Bfdp::Lexer::Symbolizer mSymbolizer;

            //! State machine engine to use for algorithm control
            Bfdp::StateMachine::Engine mStateMachine;
        };

    } // namespace Token

} // namespace BfsdlParser

#endif // BfsdlParser_Token_Tokenizer

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

// Base includes
#include "BfsdlParser/Token/Tokenizer.hpp"

// External includes
#include <string>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Lexer/ArraySymbolCategory.hpp"
#include "Bfdp/Lexer/RangeSymbolCategory.hpp"
#include "Bfdp/Lexer/StringSymbolCategory.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/StateMachine/Actions.hpp"
#include "BfsdlParser/Token/Category.hpp"

#define BFDP_MODULE "Token::Tokenizer"

namespace BfsdlParser
{

    namespace Token
    {

        using namespace Bfdp;

        namespace InternalTokenizer
        {

            static int const UnknownCategory = -1;

            static Lexer::StringSymbolCategory CatControl( Category::Control, "[];:", false );
            static Lexer::RangeSymbolCategory CatDecimalDigits( Category::DecimalDigits, 48, 57, true ); // 0-9
            static Lexer::RangeSymbolCategory CatHash( Category::Hash, 35, false ); // Hash
            static Lexer::RangeSymbolCategory CatLetters1( Category::Letters, 65, 90, true ); // A-Z
            static Lexer::RangeSymbolCategory CatLetters2( Category::Letters, 97, 122, true ); // a-z
            static Lexer::StringSymbolCategory CatOperators( Category::Operators, "+-", false );
            static Lexer::RangeSymbolCategory CatPeriod( Category::Period, 46, false ); // Period
            static Lexer::StringSymbolCategory CatTilde( Category::Tilde, "~", false ); // Tilde
            static Lexer::StringSymbolCategory CatWhitespace( Category::Whitespace, " \t\n\r", true );

            struct ParseState
            {
                enum Type
                {
                    MainSequence,
                    NumericLiteral,

                    Count
                };
            };

        };

        using namespace InternalTokenizer;

        Tokenizer::Tokenizer
            (
            ITokenObserver& aObserver
            )
            : mInitOk( false )
            , mNumericLiteralParser( aObserver )
            , mObserver( aObserver )
            , mParseError( false )
            , mSymbolizer( *this, mSymbolBuffer, mAsciiConverter )
        {
            bool ok = true;
            ok = ok && mSymbolizer.AddCategory( &CatControl );
            ok = ok && mSymbolizer.AddCategory( &CatDecimalDigits );
            ok = ok && mSymbolizer.AddCategory( &CatHash );
            ok = ok && mSymbolizer.AddCategory( &CatLetters1 );
            ok = ok && mSymbolizer.AddCategory( &CatLetters2 );
            ok = ok && mSymbolizer.AddCategory( &CatOperators );
            ok = ok && mSymbolizer.AddCategory( &CatPeriod );
            ok = ok && mSymbolizer.AddCategory( &CatWhitespace );

            if( !ok )
            {
                BFDP_RUNTIME_ERROR( "Failed to add categories" );
                return;
            }

            typedef StateMachine::CallMethod< Tokenizer > CallMethod;

            BFDP_STATE_MAP_BEGIN( ok, mStateMachine, ParseState::Count );

            BFDP_STATE_ACTION( ParseState::MainSequence, Evaluate, CallMethod( *this, &Tokenizer::StateMainSequenceEvaluate ) );
            BFDP_STATE_ACTION( ParseState::NumericLiteral, Entry, CallMethod( *this, &Tokenizer::StateNumericLiteralEntry ) );
            BFDP_STATE_ACTION( ParseState::NumericLiteral, Evaluate, CallMethod( *this, &Tokenizer::StateNumericLiteralEvaluate ) );

            BFDP_STATE_MAP_END();

            if( !ok )
            {
                BFDP_RUNTIME_ERROR( "Failed to init state machine" );
                return;
            }

            mStateMachine.Transition( ParseState::MainSequence );
            mStateMachine.DoTransition();
            mInitOk = true;
        }

        void Tokenizer::EndParsing()
        {
            if( !mInitOk )
            {
                BFDP_RUNTIME_ERROR( "Cannot parse; Tokenizer failed to initialize" );
                return;
            }

            // Do nothing else if an error was encountered
            BFDP_RETURNIF( mParseError );

            if( mStateMachine.GetCurState() != ParseState::MainSequence )
            {
                BFDP_RUNTIME_ERROR( "Unparsed content in stream" );
                return;
            }

            mSymbolizer.EndParsing();
        }

        bool Tokenizer::IsInitOk() const
        {
            return mInitOk;
        }

        bool Tokenizer::Parse
            (
            Byte const * aBytes,
            size_t aNumBytes,
            size_t& aBytesRead
            )
        {
            if( !mInitOk )
            {
                BFDP_RUNTIME_ERROR( "Cannot parse; Tokenizer failed to initialize" );
                mParseError = true;
                return false;
            }

            if( !mSymbolizer.Parse( aBytes, aNumBytes, aBytesRead ) )
            {
                mParseError = true;
                return false;
            }

            return true;
        }

        Tokenizer::StateVariables::StateVariables()
            : inputCategory( UnknownCategory )
            , keepParsing( true )
        {
        }

        /* virtual */ bool Tokenizer::OnMappedSymbol
            (
            int const aCategory,
            std::string const& aSymbol
            )
        {
            mState.inputCategory = aCategory;
            mState.inputSymbol = aSymbol;

            mStateMachine.EvaluateState();

            return mState.keepParsing;
        }

        /* virtual */ bool Tokenizer::OnUnmappedSymbol
            (
            std::string const& /* aSymbol */
            )
        {
            BFDP_RUNTIME_ERROR( "Unexpected symbol" );
            mParseError = true;
            return false;
        }

        void Tokenizer::StateMainSequenceEvaluate()
        {
            switch( mState.inputCategory )
            {
            case Category::Control:
                mObserver.OnControlCharacter( mState.inputSymbol );
                break;

            case Category::Hash:
                mStateMachine.Transition( ParseState::NumericLiteral );
                break;

            case Category::Whitespace:
                break;

            default:
                BFDP_RUNTIME_ERROR( "Unexpected symbol(s) at beginning of statement" );
                mParseError = true;
                mState.keepParsing = false;
                break;
            }
        }

        void Tokenizer::StateNumericLiteralEntry()
        {
            mNumericLiteralParser.Reset();
        }

        void Tokenizer::StateNumericLiteralEvaluate()
        {
            mNumericLiteralParser.ParseMappedSymbol( mState.inputCategory, mState.inputSymbol );
            switch( mNumericLiteralParser.GetParseResult() )
            {
                case ParseResult::Error:
                    mParseError = true;
                    mState.keepParsing = false;
                    break;

                case ParseResult::NotComplete:
                    // Nothing to do yet
                    break;

                case ParseResult::Complete:
                    mObserver.OnNumericLiteral( mNumericLiteralParser.GetParsedObject() );
                    mStateMachine.Transition( ParseState::MainSequence );
                    break;

                default:
                    BFDP_INTERNAL_ERROR( "Unexpected parse state" );
                    mParseError = true;
                    mState.keepParsing = false;
                    break;
            }
        }

    } // namespace Token

} // namespace BfsdlParser

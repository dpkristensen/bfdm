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

            static Lexer::RangeSymbolCategory CatAsterisk( Category::Asterisk, 42, false );
            static Lexer::RangeSymbolCategory CatBackslash( Category::Backslash, 92, false );
            static Lexer::StringSymbolCategory CatControl( Category::Control, "[];:", false );
            static Lexer::RangeSymbolCategory CatDecimalDigits( Category::DecimalDigits, 48, 57, true ); // 0-9
            static Lexer::RangeSymbolCategory CatDoubleQuotes( Category::DoubleQuotes, 34, false ); // Double Quotes
            static Lexer::StringSymbolCategory CatEndOfLine( Category::EndOfLine, "\r\n", true );
            static Lexer::RangeSymbolCategory CatForwardSlash( Category::ForwardSlash, 47, false );
            static Lexer::RangeSymbolCategory CatHash( Category::Hash, 35, false ); // Hash
            static Lexer::RangeSymbolCategory CatLetters1( Category::Letters, 65, 90, true ); // A-Z
            static Lexer::RangeSymbolCategory CatLetters2( Category::Letters, 97, 122, true ); // a-z
            static Lexer::StringSymbolCategory CatOperators( Category::Operators, "+-", false );
            static Lexer::RangeSymbolCategory CatPeriod( Category::Period, 46, false ); // Period
            static Lexer::StringSymbolCategory CatTilde( Category::Tilde, "~", false ); // Tilde
            static Lexer::StringSymbolCategory CatUnderscore( Category::Underscore, "_", true ); // Underscore
            static Lexer::StringSymbolCategory CatWhitespace( Category::Whitespace, " \t", true );

            struct ParseState
            {
                enum Type
                {
                    CommentML,
                    CommentSL,
                    NGraph,
                    MainSequence,
                    NumericLiteral,
                    StringLiteral,
                    Word,

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
            , mStringLiteralParser( aObserver )
            , mSymbolizer( *this, mSymbolBuffer, mAsciiConverter )
        {
            bool ok = true;
            ok = ok && mSymbolizer.AddCategory( &CatAsterisk );
            ok = ok && mSymbolizer.AddCategory( &CatBackslash );
            ok = ok && mSymbolizer.AddCategory( &CatControl );
            ok = ok && mSymbolizer.AddCategory( &CatDecimalDigits );
            ok = ok && mSymbolizer.AddCategory( &CatDoubleQuotes );
            ok = ok && mSymbolizer.AddCategory( &CatEndOfLine );
            ok = ok && mSymbolizer.AddCategory( &CatForwardSlash
             );
            ok = ok && mSymbolizer.AddCategory( &CatHash );
            ok = ok && mSymbolizer.AddCategory( &CatLetters1 );
            ok = ok && mSymbolizer.AddCategory( &CatLetters2 );
            ok = ok && mSymbolizer.AddCategory( &CatOperators );
            ok = ok && mSymbolizer.AddCategory( &CatPeriod );
            ok = ok && mSymbolizer.AddCategory( &CatUnderscore );
            ok = ok && mSymbolizer.AddCategory( &CatWhitespace );

            if( !ok )
            {
                BFDP_RUNTIME_ERROR( "Failed to add categories" );
                return;
            }

            typedef StateMachine::CallMethod< Tokenizer > CallMethod;

            BFDP_STATE_MAP_BEGIN( ok, mStateMachine, ParseState::Count );

            BFDP_STATE_ACTION( ParseState::CommentML, Evaluate, CallMethod( *this, &Tokenizer::StateCommentMLEvaluate ) );
            BFDP_STATE_ACTION( ParseState::CommentSL, Evaluate, CallMethod( *this, &Tokenizer::StateCommentSLEvaluate ) );
            BFDP_STATE_ACTION( ParseState::MainSequence, Evaluate, CallMethod( *this, &Tokenizer::StateMainSequenceEvaluate ) );
            BFDP_STATE_ACTION( ParseState::NGraph, Entry, CallMethod( *this, &Tokenizer::StateNGraphEntry ) );
            BFDP_STATE_ACTION( ParseState::NGraph, Evaluate, CallMethod( *this, &Tokenizer::StateNGraphEvaluate ) );
            BFDP_STATE_ACTION( ParseState::NumericLiteral, Entry, CallMethod( *this, &Tokenizer::StateNumericLiteralEntry ) );
            BFDP_STATE_ACTION( ParseState::NumericLiteral, Evaluate, CallMethod( *this, &Tokenizer::StateNumericLiteralEvaluate ) );
            BFDP_STATE_ACTION( ParseState::StringLiteral, Entry, CallMethod( *this, &Tokenizer::StateStringLiteralEntry ) );
            BFDP_STATE_ACTION( ParseState::StringLiteral, Evaluate, CallMethod( *this, &Tokenizer::StateStringLiteralEvaluate ) );
            BFDP_STATE_ACTION( ParseState::Word, Entry, CallMethod( *this, &Tokenizer::StateWordEntry ) );
            BFDP_STATE_ACTION( ParseState::Word, Evaluate, CallMethod( *this, &Tokenizer::StateWordEvaluate ) );

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

            // Flush out any remaining characters to drive the state machine forward
            mSymbolizer.EndParsing();
            BFDP_RETURNIF( mParseError );

            switch( mStateMachine.GetCurState() )
            {
                case ParseState::CommentSL:
                case ParseState::MainSequence:
                    // No error here
                    break;

                case ParseState::CommentML:
                    BFDP_RUNTIME_ERROR( "Unterminated multi-line comment" );
                    break;

                case ParseState::NGraph:
                    // Handle incomplete N-Graphs
                    BFDP_UNUSED_RETURN( ParseNGraph( true ) );
                    HandleNGraphEntryFromMainSequence();
                    break;

                case ParseState::NumericLiteral:
                    BFDP_RUNTIME_ERROR( "Unterminated Numeric Literal" );
                    mParseError = true;
                    break;

                case ParseState::StringLiteral:
                    BFDP_RUNTIME_ERROR( "Unterminated String Literal" );
                    mParseError = true;
                    break;

                case ParseState::Word:
                    // Emit any pending word
                    EmitWord();
                    break;

                default:
                    BFDP_INTERNAL_ERROR( "Bad final parse state" );
                    mParseError = true;
                    break;
            };
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
            }
            else if( !mSymbolizer.Parse( aBytes, aNumBytes, aBytesRead ) )
            {
                mParseError = true;
            }

            return mState.keepParsing && !mParseError;
        }

        Tokenizer::StateVariables::StateVariables()
            : symbols( Category::Unknown, 0, std::string() )
            , keepParsing( true )
            , reEvaluate( false )
        {
        }

        void Tokenizer::EmitWord()
        {
            std::string word = mState.word.str();
            if( !word.empty() )
            {
                mState.keepParsing = mObserver.OnWord( word );
            }
        }

        void Tokenizer::HandleNGraphEntryFromMainSequence()
        {
            if( mState.ngraph == "//" )
            {
                mStateMachine.Transition( ParseState::CommentSL );
            }
            else if( mState.ngraph == "/*" )
            {
                mStateMachine.Transition( ParseState::CommentML );
            }
            else
            {
                BFDP_RUNTIME_ERROR( "Unexpected operator" );
                mParseError = true;
                mState.keepParsing = false;
            }

            // Clear out the NGraph
            mState.ngraph.clear();
        }

        /* virtual */ bool Tokenizer::OnMappedSymbols
            (
            int const aCategory,
            std::string const& aSymbols,
            size_t const aNumSymbols
            )
        {
            mState.symbols.category = aCategory;
            mState.symbols.count = aNumSymbols;
            mState.symbols.str = aSymbols;

            do
            {
                mState.reEvaluate = false;
                mStateMachine.EvaluateState();
            } while( mState.reEvaluate && !mParseError );

            return mState.keepParsing;
        }

        /* virtual */ bool Tokenizer::OnUnmappedSymbols
            (
            std::string const& aSymbols,
            size_t const aNumSymbols
            )
        {
            return OnMappedSymbols( Category::Unknown, aSymbols, aNumSymbols );
        }

        ParseResult::Value Tokenizer::ParseNGraph
            (
            bool const aFinal
            )
        {
            // Use result of c_str() for bounds checking, as it is guaranteed to be
            // null-terminated (operator [] makes no such guarantee).
            std::string ngraph = mState.ngraphBuilder.str();
            char const* const c = ngraph.c_str();

            ParseResult::Value result = ParseResult::Error;
            if( c[0] == '/' )
            {
                if( ( c[1] == '/' ) ||
                    ( c[1] == '*' ) )
                {
                    result = ParseResult::Complete;
                }
                else if( c[1] == '\0' )
                {
                    // If math symbols were needed, divide could be found
                    // with aFinal && ( c[1] == '\0' ).
                    result = ParseResult::NotComplete;
                }
            }
            else if( c[0] == '*' )
            {
                if( c[1] == '/' )
                {
                    result = ParseResult::Complete;
                }
                else if( c[1] == '\0' )
                {
                    // If math symbols were needed, multiply could be found
                    // with aFinal && ( c[1] == '\0' ).
                    result = ParseResult::NotComplete;
                }
            }

            if( aFinal && ( result == ParseResult::NotComplete ) )
            {
                // Force completion if this is the end
                result = ParseResult::Complete;
            }

            // Save the result when it is deterministic (either good or bad)
            // Errors may be ignored depending on the context, so don't log one here.
            if( result != ParseResult::NotComplete )
            {
                mState.ngraph = ngraph;
                mState.ngraphBuilder.clear();
                mState.ngraphBuilder.str( std::string() );
            }

            return result;
        }

        void Tokenizer::StateCommentMLEvaluate()
        {
            ParseResult::Value ngraphResult = ParseResult::Error;

            switch( mState.symbols.category )
            {
            case Category::Asterisk:
            case Category::ForwardSlash:
                mState.ngraphBuilder << mState.symbols.str;
                ngraphResult = ParseNGraph( false );
                break;

            default:
                // Reset the NGraph parser
                mState.ngraphBuilder.clear();
                mState.ngraphBuilder.str( std::string() );
                break;
            }

            if( ( ParseResult::Complete == ngraphResult ) &&
                ( mState.ngraph == "*/" ) )
            {
                // Ending the comment is the only way to get out of this state
                mStateMachine.Transition( ParseState::MainSequence );
            }
        }

        void Tokenizer::StateCommentSLEvaluate()
        {
            switch( mState.symbols.category )
            {
            case Category::EndOfLine:
                mStateMachine.Transition( ParseState::MainSequence );
                break;

            default:
                // Ignore everything else
                break;
            }
        }

        void Tokenizer::StateMainSequenceEvaluate()
        {
            switch( mState.symbols.category )
            {
            case Category::Control:
                mObserver.OnControlCharacter( mState.symbols.str );
                break;

            case Category::DoubleQuotes:
                mStateMachine.Transition( ParseState::StringLiteral );
                break;

            case Category::Asterisk:
            case Category::ForwardSlash:
                // These could be part of a Di- or Tri-graph sequence.
                mStateMachine.Transition( ParseState::NGraph );
                break;

            case Category::Hash:
                mStateMachine.Transition( ParseState::NumericLiteral );
                break;

            case Category::EndOfLine:
            case Category::Whitespace:
                break;

            case Category::Letters:
            case Category::DecimalDigits:
            case Category::Underscore:
                mStateMachine.Transition( ParseState::Word );
                break;

            default:
                BFDP_RUNTIME_ERROR( "Unexpected symbol(s) at beginning of statement" );
                mParseError = true;
                mState.keepParsing = false;
                break;
            }
        }

        void Tokenizer::StateNGraphEntry()
        {
            mState.ngraphBuilder.clear();
            mState.ngraphBuilder.str( std::string() );
            mState.ngraphBuilder << mState.symbols.str;
            mState.ngraph.clear();
            if( ParseResult::NotComplete != ParseNGraph( false ) )
            {
                HandleNGraphEntryFromMainSequence();
            }
        }

        void Tokenizer::StateNGraphEvaluate()
        {
            switch( mState.symbols.category )
            {
                case Category::Asterisk:
                case Category::ForwardSlash:
                    mState.ngraphBuilder << mState.symbols.str;
                    if( ParseResult::NotComplete != ParseNGraph( false ) )
                    {
                        HandleNGraphEntryFromMainSequence();
                    }
                    break;

                default:
                    // Not an N-Graph character, so unconditionally handle what we have so far
                    BFDP_UNUSED_RETURN( ParseNGraph( true ) );
                    HandleNGraphEntryFromMainSequence();
                    // Re-parse the current symbols on re-entry to the new state
                    mState.reEvaluate = mState.keepParsing;
                    break;
            }
        }

        void Tokenizer::StateNumericLiteralEntry()
        {
            mNumericLiteralParser.Reset();
        }

        void Tokenizer::StateNumericLiteralEvaluate()
        {
            mNumericLiteralParser.ParseSymbols( mState.symbols );
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

        void Tokenizer::StateStringLiteralEntry()
        {
            mStringLiteralParser.Reset();
        }

        void Tokenizer::StateStringLiteralEvaluate()
        {
            mStringLiteralParser.ParseSymbols( mState.symbols );
            switch( mStringLiteralParser.GetParseResult() )
            {
                case ParseResult::Error:
                    mParseError = true;
                    mState.keepParsing = false;
                    break;

                case ParseResult::NotComplete:
                    // Nothing to do yet
                    break;

                case ParseResult::Complete:
                    mObserver.OnStringLiteral( mStringLiteralParser.GetParsedObject() );
                    mStateMachine.Transition( ParseState::MainSequence );
                    break;

                default:
                    BFDP_INTERNAL_ERROR( "Unexpected parse state" );
                    mParseError = true;
                    mState.keepParsing = false;
                    break;
            }
        }

        void Tokenizer::StateWordEntry()
        {
            mState.word.clear();
            mState.word.str( std::string() );
            mState.word << mState.symbols.str;
        }

        void Tokenizer::StateWordEvaluate()
        {
            switch( mState.symbols.category )
            {
            case Category::Letters:
            case Category::DecimalDigits:
            case Category::Underscore:
                mState.word << mState.symbols.str;
                break;

            default:
                // End of the word
                EmitWord();

                // Re-parse the current symbols on re-entry to the main state
                mState.reEvaluate = true;
                mStateMachine.Transition( ParseState::MainSequence );
                break;
            }
        }

    } // namespace Token

} // namespace BfsdlParser

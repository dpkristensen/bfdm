/**
    BFSDL Parser String Literal Parser Declarations

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

// Base includes
#include "BfsdlParser/Token/StringLiteralParser.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/StateMachine/Actions.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/Iterator.hpp"
#include "Bfdp/Unicode/Ms1252Converter.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"
#include "BfsdlParser/Token/Category.hpp"

#define BFDP_MODULE "Token::StringLiteralParser"

namespace BfsdlParser
{

    namespace Token
    {

        using namespace Bfdp;

        namespace InternalStringLiteralParser
        {

            struct ParseState
            {
                enum Type
                {
                    Text,
                    Backslash,
                    EscapeDigits,

                    Count
                };
            };

        };

        using namespace InternalStringLiteralParser;

        StringLiteralParser::StringLiteralParser
            (
            ITokenObserver& aObserver
            )
            : mInitOk( false )
            , mLastParseResult( ParseResult::NotComplete )
            , mObserver( aObserver )
        {
            typedef StateMachine::CallMethod< StringLiteralParser > CallMethod;

            bool ok = true;
            BFDP_STATE_MAP_BEGIN( ok, mStateMachine, ParseState::Count );

            BFDP_STATE_ACTION( ParseState::Text, Entry, CallMethod( *this, &StringLiteralParser::StateTextEntry ) );
            BFDP_STATE_ACTION( ParseState::Text, Evaluate, CallMethod( *this, &StringLiteralParser::StateTextEvaluate ) );
            BFDP_STATE_ACTION( ParseState::Backslash, Evaluate, CallMethod( *this, &StringLiteralParser::StateBackslashEvaluate ) );
            BFDP_STATE_ACTION( ParseState::EscapeDigits, Evaluate, CallMethod( *this, &StringLiteralParser::StateEscapeDigitsEvaluate ) );

            BFDP_STATE_MAP_END();

            if( !ok )
            {
                BFDP_RUNTIME_ERROR( "Failed to init state machine" );
                return;
            }

            mStateMachine.Transition( ParseState::Text );
            mStateMachine.DoTransition();
            mInitOk = true;
        }

        ParseResult::Value StringLiteralParser::GetParseResult() const
        {
            return mLastParseResult;
        }

        Data::StringMachine const& StringLiteralParser::GetParsedObject() const
        {
            return mStringLiteral;
        }

        bool StringLiteralParser::IsInitOk() const
        {
            return mInitOk;
        }

        void StringLiteralParser::ParseSymbols
            (
            SymbolSequence const& aSymbols
            )
        {
            mState.symbols = aSymbols;

            mStateMachine.EvaluateState();
        }

        void StringLiteralParser::Reset()
        {
            mState.Reset();
            mStringLiteral.Reset();
            mLastParseResult = ParseResult::NotComplete;

            mStateMachine.Transition( ParseState::Text );
            mStateMachine.DoTransition();
        }

        StringLiteralParser::StateVariables::StateVariables()
            : escapeConverter( NULL )
            , escapeDigits( 0U )
            , escapeRadix( Bfdp::Data::InvalidRadix )
            , escapeSymbolCount( 0U )
            , symbols( Category::Unknown, 0, std::string() )
        {
        }

        void StringLiteralParser::StateVariables::Reset()
        {
            escapeConverter = NULL;
            escapeDigits = 0U;
            escapeRadix = Bfdp::Data::InvalidRadix;
            escapeSequence.Reset();
            escapeSymbolCount = 0U;
        }

        bool StringLiteralParser::ParseEscapeChar()
        {
            BFDP_RETURNIF_V( mState.symbols.count != 1, false );

            switch( mState.symbols.str[0] )
            {
            case '"':
                mStringLiteral.AppendUtf8( "\x22" );
                mStateMachine.Transition( ParseState::Text );
                break;

            case '\\':
                mStringLiteral.AppendUtf8( "\x5c" );
                mStateMachine.Transition( ParseState::Text );
                break;

            case 'a':
                BFDP_RETURNIF_V( mState.escapeDigits != 0, false );
                mState.escapeDigits = 2; // Always 2 digits
                mState.escapeRadix = 16; // Hex
                mState.escapeConverter = &Bfdp::Unicode::AsciiConverter::GetUnicode;
                mStateMachine.Transition( ParseState::EscapeDigits );
                break;

            case 'b':
                BFDP_RETURNIF_V( mState.escapeDigits > 32, false );
                if( mState.escapeDigits == 0 )
                {
                    mState.escapeDigits = 8;
                }
                mState.escapeRadix = 2; // Binary
                mStateMachine.Transition( ParseState::EscapeDigits );
                break;

            case 'n':
                mStringLiteral.AppendUtf8( "\x0a" );
                mStateMachine.Transition( ParseState::Text );
                break;

            case 'r':
                mStringLiteral.AppendUtf8( "\x0d" );
                mStateMachine.Transition( ParseState::Text );
                break;

            case 't':
                mStringLiteral.AppendUtf8( "\x08" );
                mStateMachine.Transition( ParseState::Text );
                break;

            case 'x':
                BFDP_RETURNIF_V( mState.escapeDigits > 8, false );
                if( mState.escapeDigits == 0 )
                {
                    mState.escapeDigits = 2;
                }
                mState.escapeRadix = 16; // Hex
                mStateMachine.Transition( ParseState::EscapeDigits );
                break;

            case 'u':
                BFDP_RETURNIF_V( mState.escapeDigits > 8, false );
                if( mState.escapeDigits == 0 )
                {
                    mState.escapeDigits = 4;
                }
                mState.escapeRadix = 16; // Hex
                mStateMachine.Transition( ParseState::EscapeDigits );
                break;

            case 'w':
                BFDP_RETURNIF_V( mState.escapeDigits != 0, false );
                mState.escapeDigits = 2; // Always 2 digits
                mState.escapeRadix = 16; // Hex
                mState.escapeConverter = &Bfdp::Unicode::Ms1252Converter::GetUnicode;
                mStateMachine.Transition( ParseState::EscapeDigits );
                break;

            default:
                return false;
                // Break omitted - unconditional return
            }

            return true;
        }

        ParseResult::Value StringLiteralParser::ParseEscapeCodePoint
            (
            Data::RadixType const aRadix
            )
        {
            if( mState.escapeSymbolCount < mState.escapeDigits )
            {
                // Not enough digits yet, add symbols to escape sequence
                mState.escapeSequence.AppendUtf8( mState.symbols.str );
                mState.escapeSymbolCount += mState.symbols.count;
            }

            if( mState.escapeSymbolCount < mState.escapeDigits )
            {
                return ParseResult::NotComplete;
            }

            Data::StringMachine leftovers;
            Unicode::CodePoint cp = 0;
            Unicode::AsciiConverter ascii;
            std::string input;
            if( !mState.escapeSequence.GetString( ascii, input ) )
            {
                return ParseResult::Error;
            }

            for( Unicode::StrIterator< Unicode::AsciiConverter > iter( input ); iter; ++iter )
            {
                if( iter.GetIndex() >= mState.escapeDigits )
                {
                    // These don't belong to the escape sequence, save them for later
                    leftovers.AppendUnicode( *iter );
                    continue;
                }

                std::string cpStr = iter.GetCodePointString();
                uint8_t value;
                if( !Data::ConvertBase( aRadix, cpStr[0] , value ) )
                {
                    return ParseResult::Error;
                }

                cp *= aRadix;
                cp += value;
            }
            if( mState.escapeConverter && !mState.escapeConverter( cp, cp ) )
            {
                return ParseResult::Error;
            }
            if( !mStringLiteral.AppendUnicode( cp ) )
            {
                return ParseResult::Error;
            }
            mStringLiteral.AppendUtf8( leftovers.GetUtf8String() );

            return ParseResult::Complete;
        }

        void StringLiteralParser::ParseEscapeDigits()
        {
            static Data::RadixType const RADIX = 10U;
            static size_t const MAX_IN_DIGITS = 2U;
            static uint8_t const MAX_OUT_DIGITS = 32U;
            static uint8_t const MIN_OUT_DIGITS = 1U;

            if( mState.escapeDigits != 0 )
            {
                BFDP_RUNTIME_ERROR( "Cannot redefine escape-digits in string-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }
            else if( mState.symbols.count > MAX_IN_DIGITS )
            {
                BFDP_RUNTIME_ERROR( "Too many escape-digits in string-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            uint8_t total = 0;
            for
                (
                std::string::const_iterator iter = mState.symbols.str.begin();
                iter != mState.symbols.str.end();
                ++iter
                )
            {
                uint8_t value;
                if( !Data::ConvertBase( RADIX, *iter, value ) )
                {
                    BFDP_RUNTIME_ERROR( "Invalid escape-digits in string-literal" );
                    mLastParseResult = ParseResult::Error;
                    return;
                }

                total *= RADIX;
                total += value;
            }

            if( !IsWithinRange( MIN_OUT_DIGITS, total, MAX_OUT_DIGITS ) )
            {
                BFDP_RUNTIME_ERROR( "Number of escape-digits out of range in string-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            mState.escapeDigits = total;
        }

        void StringLiteralParser::StateBackslashEvaluate()
        {
            if( mState.symbols.category == Category::DecimalDigits )
            {
                ParseEscapeDigits();
            }
            else if( !ParseEscapeChar() )
            {
                BFDP_RUNTIME_ERROR( "Illegal escape sequence in string-literal" );
                mLastParseResult = ParseResult::Error;
            }
        }

        void StringLiteralParser::StateEscapeDigitsEvaluate()
        {
            if( mState.symbols.category == Category::DoubleQuotes )
            {
                BFDP_RUNTIME_ERROR( "Missing digits in string-literal escape-sequence" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            switch( ParseEscapeCodePoint( mState.escapeRadix ) )
            {
            case ParseResult::Error:
                BFDP_RUNTIME_ERROR( "Invalid digits in string-literal escape-sequence" );
                mLastParseResult = ParseResult::Error;
                break;

            case ParseResult::Complete:
                mStateMachine.Transition( ParseState::Text );
                break;

            case ParseResult::NotComplete:
            default:
                break;
            }
        }

        void StringLiteralParser::StateTextEntry()
        {
            mState.Reset();
        }

        void StringLiteralParser::StateTextEvaluate()
        {
            switch( mState.symbols.category )
            {
            case Category::DoubleQuotes:
                mLastParseResult = ParseResult::Complete;
                break;

            case Category::Backslash:
                mStateMachine.Transition( ParseState::Backslash );
                break;

            default:
                mStringLiteral.AppendUtf8( mState.symbols.str );
            }
        }

    } // namespace Token

} // namespace BfsdlParser

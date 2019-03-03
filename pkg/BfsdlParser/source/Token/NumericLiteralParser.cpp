/**
    BFSDL Parser Numeric Literal Parser Declarations

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
#include "BfsdlParser/Token/NumericLiteralParser.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Token/Category.hpp"

#define BFDP_MODULE "Token::NumericLiteralParser"

namespace BfsdlParser
{

    namespace Token
    {

        using namespace Bfdp;

        NumericLiteralParser::NumericLiteralParser
            (
            ITokenObserver& aObserver
            )
            : mHasPeriod( false )
            , mLastParseResult( ParseResult::NotComplete )
            , mObserver( aObserver )
        {
        }

        ParseResult::Value NumericLiteralParser::GetParseResult() const
        {
            return mLastParseResult;
        }

        Objects::NumericLiteral const& NumericLiteralParser::GetParsedObject() const
        {
            return mNumericLiteral;
        }

        void NumericLiteralParser::ParseSymbols
            (
            SymbolSequence const& aSymbols
            )
        {
            switch( aSymbols.category )
            {
                case Category::Control:
                    ParseControl( aSymbols.str );
                    break;

                case Category::DecimalDigits:
                case Category::Letters:
                    // Letters and numbers are not processed immediately
                    mUnparsedText += aSymbols.str;
                    break;

                case Category::Hash:
                    ParseDigits();
                    ParseHash();
                    break;

                case Category::Operators:
                    UseOrSetDefaultRadix();
                    ParseSign( aSymbols.str );
                    break;

                case Category::Period:
                    ParseDigits();
                    ParsePeriod();
                    break;

                case Category::Tilde:
                    ParseDigits();
                    ParseTilde();
                    break;

                case Category::Whitespace:
                    BFDP_RUNTIME_ERROR( "Illegal whitespace in numeric-literal" );
                    mLastParseResult = ParseResult::Error;
                    break;

                default:
                    ParseDigits();
                    break;
            }
        }

        void NumericLiteralParser::Reset()
        {
            mHasPeriod = false;
            mLastParseResult = ParseResult::NotComplete;
            mNumericLiteral.Reset();
            mUnparsedText.clear();
        }

        void NumericLiteralParser::ParseControl
            (
            std::string const& aSymbol
            )
        {
            if( ":" == aSymbol )
            {
                ParseRadix();
            }
            else
            {
                BFDP_RUNTIME_ERROR( "Illegal control character in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
        }

        void NumericLiteralParser::ParseDigits()
        {
            if( mUnparsedText.empty() )
            {
                // Nothing to do
                return;
            }

            if( !mNumericLiteral.GetBase().IsDefined() )
            {
                // If the base is not defined, this is integral/fractional
                if( !mNumericLiteral.GetSignificand().integral.IsDefined() )
                {
                    // integral-component
                    UseOrSetDefaultRadix();
                    UseOrSetDefaultSignificandSign();
                    if( !mNumericLiteral.SetSignificandIntegralDigits( mUnparsedText ) )
                    {
                        mLastParseResult = ParseResult::Error;
                    }
                }
                else if( mHasPeriod && !mNumericLiteral.GetSignificand().fractional.IsDefined() )
                {
                    // fractional-component
                    UseOrSetDefaultRadix();
                    UseOrSetDefaultSignificandSign();
                    if( !mNumericLiteral.SetSignificandFractionalDigits( mUnparsedText ) )
                    {
                        mLastParseResult = ParseResult::Error;
                    }
                }
                else
                {
                    BFDP_INTERNAL_ERROR( "Unknown identifier" );
                    mLastParseResult = ParseResult::Error;
                    return;
                }
            }
            else if( !mNumericLiteral.GetExponent().IsDefined() )
            {
                BFDP_INTERNAL_ERROR( "Unknown identifier" );
                mLastParseResult = ParseResult::Error;
                return;
            }
            else
            {
                // exponential-component
                UseOrSetDefaultExponentSign();
                if( !mNumericLiteral.SetExponentDigits( mUnparsedText ) )
                {
                    mLastParseResult = ParseResult::Error;
                }
            }

            mUnparsedText.clear();
            return;
        }

        void NumericLiteralParser::ParseHash()
        {
            if( !mUnparsedText.empty() )
            {
                BFDP_RUNTIME_ERROR( "Unparsed text at the end of numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else if( !mNumericLiteral.GetSignificand().IsDefined() )
            {
                BFDP_RUNTIME_ERROR( "Missing significand in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else if( mNumericLiteral.GetBase().IsDefined() != mNumericLiteral.GetExponent().IsDefined() )
            {
                BFDP_RUNTIME_ERROR( "Malformed exponent in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else
            {
                mLastParseResult = ParseResult::Complete;
            }
        }

        void NumericLiteralParser::ParsePeriod()
        {
            // This must follow an integral-component
            if( !mNumericLiteral.GetSignificand().integral.IsDefined() )
            {
                BFDP_RUNTIME_ERROR( "Missing integral-component in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else if( mHasPeriod )
            {
                BFDP_RUNTIME_ERROR( "Illegal period in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else if( mNumericLiteral.GetBase().IsDefined() )
            {
                BFDP_RUNTIME_ERROR( "Illegal fractional exponential-component in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else
            {
                mHasPeriod = true;
            }
        }

        void NumericLiteralParser::ParseRadix()
        {
            // The precediing text must be a radix symbol
            if( mNumericLiteral.HasRadix() )
            {
                BFDP_RUNTIME_ERROR( "Redefinition of radix-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            if( mUnparsedText.empty() )
            {
                BFDP_RUNTIME_ERROR( "Missing radix-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            if( "b" == mUnparsedText )
            {
                mNumericLiteral.SetRadix( 2 );
            }
            else if( "o" == mUnparsedText )
            {
                mNumericLiteral.SetRadix( 8 );
            }
            else if( "d" == mUnparsedText )
            {
                mNumericLiteral.SetRadix( 10 );
            }
            else if( "x" == mUnparsedText )
            {
                mNumericLiteral.SetRadix( 16 );
            }
            else
            {
                BFDP_RUNTIME_ERROR( "Invalid radix-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            mUnparsedText.clear();
        }

        void NumericLiteralParser::ParseSign
            (
            std::string const& aText
            )
        {
            // Validate input first, so error messages make sense
            Data::Sign sign;
            if( "+" == aText )
            {
                sign = Data::Sign::Positive;
            }
            else if( "-" == aText )
            {
                sign = Data::Sign::Negative;
            }
            else
            {
                BFDP_RUNTIME_ERROR( "Illegal sign-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }

            if( !mUnparsedText.empty() )
            {
                BFDP_RUNTIME_ERROR( "Unexpected text before sign-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }
            else if( !mNumericLiteral.GetSignificand().IsDefined() )
            {
                // Sign for the significand
                if( mNumericLiteral.GetSignificand().sign.IsSpecified() )
                {
                    BFDP_RUNTIME_ERROR( "Redefinition of significand sign-marker in numeric-literal" );
                    mLastParseResult = ParseResult::Error;
                    return;
                }

                mNumericLiteral.SetSignificandSign(  sign );
            }
            else if( mNumericLiteral.GetBase().IsDefined() && !mNumericLiteral.GetExponent().IsDefined() )
            {
                // Sign for the exponent
                if( mNumericLiteral.GetExponent().sign.IsSpecified() )
                {
                    BFDP_RUNTIME_ERROR( "Redefinition of exponent sign-marker in numeric-literal" );
                    mLastParseResult = ParseResult::Error;
                    return;
                }

                mNumericLiteral.SetExponentSign( sign );
            }
            else
            {
                BFDP_RUNTIME_ERROR( "Illegal sign-marker in numeric-literal" );
                mLastParseResult = ParseResult::Error;
                return;
            }
        }

        void NumericLiteralParser::ParseTilde()
        {
            if( !mNumericLiteral.GetSignificand().IsDefined() )
            {
                BFDP_RUNTIME_ERROR( "Missing significand before exponent in numeric-literal" );
                mLastParseResult = ParseResult::Error;
            }
            else if( !mNumericLiteral.SetDefaultBase() )
            {
                BFDP_RUNTIME_ERROR( "Failed to set default base" );
                mLastParseResult = ParseResult::Error;
            }
        }

        void NumericLiteralParser::UseOrSetDefaultRadix()
        {
            if( !mNumericLiteral.HasRadix() )
            {
                mNumericLiteral.SetRadix( 10 );
            }
        }

        void NumericLiteralParser::UseOrSetDefaultExponentSign()
        {
            if( !mNumericLiteral.GetExponent().sign.IsSpecified() )
            {
                mNumericLiteral.SetExponentSign( Data::Sign::Positive );
            }
        }

        void NumericLiteralParser::UseOrSetDefaultSignificandSign()
        {
            if( !mNumericLiteral.GetSignificand().sign.IsSpecified() )
            {
                mNumericLiteral.SetSignificandSign( Data::Sign::Positive );
            }
        }

    } // namespace Token

} // namespace BfsdlParser

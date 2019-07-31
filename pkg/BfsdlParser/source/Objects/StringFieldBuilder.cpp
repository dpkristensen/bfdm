/**
    BFSDL Parser String Field Builder Definition

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

// Base Includes
#include "BfsdlParser/Objects/StringFieldBuilder.hpp"

// External Includes
#include <climits>
#include <cstdlib>

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "BfsdlParser/Objects/FStringField.hpp"
#include "BfsdlParser/Objects/PStringField.hpp"
#include "BfsdlParser/Objects/StringField.hpp"

#define BFDP_MODULE "Objects::StringFieldBuilder"

#define bfsdl_STRING_ID "string"
#define bfsdl_STRING_ID_LEN ( sizeof( bfsdl_STRING_ID ) - 1 )

namespace BfsdlParser
{

    namespace Objects
    {

        using Bfdp::Unicode::InvalidCodePoint;
        using Bfdp::Unicode::InvalidCodingId;

        StringFieldBuilder::StringFieldBuilder()
            : mComplete( false )
            , mError( false )
            , mIdentParsed( false )
            , mCode( InvalidCodingId )
            , mLengthType( LengthType::Unknown )
            , mLengthValue( 0 )
            , mTermChar( 0 )
            , mDefaultCode( Bfdp::Unicode::GetCodingId( "ASCII" ) )
            , mDefaultTermChar( 0U )
        {
        }

        bool StringFieldBuilder::Finalize()
        {
            if( !mIdentParsed )
            {
                BFDP_INTERNAL_ERROR( "Identifier not yet parsed" );
                return false;
            }

            if( mError )
            {
                BFDP_INTERNAL_ERROR( "Cannot finalize with error" );
                return false;
            }

            // Apply defaults
            if( mLengthType == LengthType::Unknown )
            {
                mLengthType = LengthType::Bounded;
            }
            if( mTermChar == InvalidCodePoint )
            {
                mTermChar = mDefaultTermChar;
            }
            if( mAllowUnterminated == Bfdp::Data::Tristate::Unset )
            {
                mAllowUnterminated = false;
            }
            if( mCode == InvalidCodingId )
            {
                mCode = mDefaultCode;
            }

            mComplete = true;
            return mComplete;
        }

        StringFieldPtr StringFieldBuilder::GetField
            (
            std::string const& aName
            ) const
        {
            BFDP_RETURNIF_V( !mComplete, NULL );
            if( mLengthType == LengthType::Bounded )
            {
                return std::make_shared< StringField >( aName, mTermChar, mAllowUnterminated.IsTrue(), mCode );
            }
            else if( mLengthType == LengthType::Fixed )
            {
                return std::make_shared< FStringField >( aName, mTermChar, mAllowUnterminated.IsTrue(), mCode, mLengthValue );
            }
            else if( mLengthType == LengthType::Prefixed )
            {
                return std::make_shared< PStringField >( aName, mTermChar, mAllowUnterminated.IsTrue(), mCode, mLengthValue );
            }

            return NULL;
        }

        bool StringFieldBuilder::ParseIdentifier
            (
            std::string const& aText
            )
        {
            if( mIdentParsed )
            {
                BFDP_INTERNAL_ERROR( "Identifier already parsed" );
                return false;
            }

            // Efficient type check
            size_t textLen = aText.length();
            if( !Bfdp::IsWithinRange( bfsdl_STRING_ID_LEN, textLen, bfsdl_STRING_ID_LEN + 1 ) )
            {
                // Wrong length
                return false;
            }

            char const* str = aText.c_str();
            if( 0 != std::strncmp
                (
                bfsdl_STRING_ID,
                str + ( textLen - bfsdl_STRING_ID_LEN ),
                bfsdl_STRING_ID_LEN
                ) )
            {
                // Does not end with "string"
                return false;
            }

            bool ok = false;
            if( textLen == bfsdl_STRING_ID_LEN )
            {
                // Matches exactly
                ok = true;
            }
            else if( textLen != ( bfsdl_STRING_ID_LEN + 1 ) )
            {
                // Too long
                ok = false;
            }
            else if( str[0] == 'c' )
            {
                // Null-terminated C-style string
                ok = ( AttributeParseResult::Success == SetTermAttr( 0 ) );
            }
            else if( str[0] == 'p' )
            {
                // Unterminated Pascal-style string
                ok = ( AttributeParseResult::Success == SetPlenAttr( 8 ) );
                ok = ok && ( AttributeParseResult::Success == SetUntermAttr() );
            }

            mIdentParsed = ok;
            return ok;
        }

        AttributeParseResult::Type StringFieldBuilder::ParseNumericAttribute
            (
            std::string const& /* aName */,
            NumericLiteral const& /* aValue */
            )
        {
            return AttributeParseResult::Unknown;
        }

        AttributeParseResult::Type StringFieldBuilder::ParseStringAttribute
            (
            std::string const& aName,
            std::string const& aValue
            )
        {
            AttributeParseResult::Type result = AttributeParseResult::Unsupported;

            if( aName == "code" )
            {
                result = SetCodeAttr( aValue );
            }
            else if( aName == "unterm" )
            {
                result = SetUntermAttr( aValue );
            }

            if( result != AttributeParseResult::Success )
            {
                mError = true;
            }

            return result;
        }

        void StringFieldBuilder::Reset()
        {
            mComplete = false;
            mError = false;
            mIdentParsed = false;

            mAllowUnterminated.Reset();
            mCode = InvalidCodingId;
            mLengthType = LengthType::Unknown;
            mLengthValue = 0;
            mTermChar = InvalidCodePoint;
        }

        void StringFieldBuilder::SetDefaultCoding
            (
            Bfdp::Unicode::CodingId const aCode
            )
        {
            mDefaultCode = aCode;
        }

        void StringFieldBuilder::SetDefaultTermChar
            (
            Bfdp::Unicode::CodePoint const aTermChar
            )
        {
            mDefaultTermChar = aTermChar;
        }

        AttributeParseResult::Type StringFieldBuilder::SetCodeAttr
            (
            std::string const& aValue
            )
        {
            BFDP_RETURNIF_V( mCode != InvalidCodingId, AttributeParseResult::Redefinition );

            mCode = Bfdp::Unicode::GetCodingId( aValue );
            BFDP_RETURNIF_V( mCode == InvalidCodingId, AttributeParseResult::InvalidArgument );

            return AttributeParseResult::Success;
        }

        AttributeParseResult::Type StringFieldBuilder::SetPlenAttr
            (
            size_t const aLength
            )
        {
            BFDP_RETURNIF_V( mLengthType != LengthType::Unknown, AttributeParseResult::Redefinition );

            mLengthType = LengthType::Prefixed;
            mLengthValue = aLength;

            return AttributeParseResult::Success;
        }

        AttributeParseResult::Type StringFieldBuilder::SetTermAttr
            (
            Bfdp::Unicode::CodePoint const aCodePoint
            )
        {
            BFDP_RETURNIF_V( mLengthType != LengthType::Unknown, AttributeParseResult::Redefinition );
            BFDP_RETURNIF_V( mTermChar != InvalidCodePoint, AttributeParseResult::Redefinition );

            mTermChar = aCodePoint;
            mLengthType = LengthType::Bounded;

            return AttributeParseResult::Success;
        }

        AttributeParseResult::Type StringFieldBuilder::SetUntermAttr
            (
            std::string const& aEmptyString
            )
        {
            BFDP_RETURNIF_V( mAllowUnterminated.IsSet(), AttributeParseResult::Redefinition );
            BFDP_RETURNIF_V( !aEmptyString.empty(), AttributeParseResult::InvalidArgument );

            mAllowUnterminated = true;
            return AttributeParseResult::Success;
        }

    } // namespace Objects

} // namespace BfsdlParser

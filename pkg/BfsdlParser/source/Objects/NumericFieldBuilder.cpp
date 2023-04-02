/**
    BFSDL Parser Numeric Field Builder Definition

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
#include "BfsdlParser/Objects/NumericFieldBuilder.hpp"

// External Includes
#include <climits>
#include <cstdlib>

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "Objects::NumericFieldBuilder"

namespace BfsdlParser
{

    namespace Objects
    {

        NumericFieldBuilder::NumericFieldBuilder()
            : mBitBase( BitBase::Default )
            , mComplete( false )
            , mIdentParsed( false )
            , mProps( false, 0, 0 )
        {
        }

        NumericFieldPtr NumericFieldBuilder::GetField
            (
            std::string const& aName
            ) const
        {
            BFDP_RETURNIF_V( !mComplete, NULL );
            return std::make_shared< NumericField >( aName, mProps );
        }

        bool NumericFieldBuilder::IsComplete() const
        {
            return mComplete;
        }

        bool NumericFieldBuilder::ParseIdentifier
            (
            std::string const& aText
            )
        {
            if( mIdentParsed )
            {
                BFDP_INTERNAL_ERROR( "Identifier already parsed" );
                return false;
            }

            // Must have at least 2 characters
            size_t strLen = aText.length();
            BFDP_RETURNIF_V( strLen < 2, false );

            // Sign is the first character
            if( !ParseSign( aText[0] ) )
            {
                return false;
            }

            // Check for integral (only numeric digits)
            char const* cdata = aText.c_str();
            for( size_t i = 1; i < strLen; ++i )
            {
                if( !Bfdp::IsWithinRange( '0', cdata[i], '9' ) )
                {
                    return false;
                }
            }

            if( !CalcBits( &cdata[1], mProps.mIntegralBits ) )
            {
                return false;
            }

            if( mProps.mSigned && ( mProps.mIntegralBits == 0 ) )
            {
                // Signed types require at least one integral bit.
                return false;
            }

            mIdentParsed = true;
            return true;
        }

        bool NumericFieldBuilder::ParseSuffix
            (
            std::string const& aText
            )
        {
            if( !mIdentParsed )
            {
                BFDP_INTERNAL_ERROR( "Identifier not yet parsed" );
                return false;
            }

            if( aText.empty() )
            {
                // No suffix, finalize
                mProps.mFractionalBits = 0;
            }
            else
            {
                // Check for fractional (only numeric digits)
                size_t strLen = aText.length();
                char const* cdata = aText.c_str();
                for( size_t i = 0; i < strLen; ++i )
                {
                    if( !Bfdp::IsWithinRange( '0', cdata[i], '9' ) )
                    {
                        return false;
                    }
                }

                if( !CalcBits( cdata, mProps.mFractionalBits ) )
                {
                    return false;
                }
            }

            mComplete = Bfdp::IsWithinRange< size_t >
                (
                1U, // Must have at least 1 bit
                mProps.mIntegralBits + mProps.mFractionalBits,
                MAX_NUMERIC_FIELD_BITS
                );

            return mComplete;
        }

        void NumericFieldBuilder::Reset()
        {
            mIdentParsed = false;
            mComplete = false;
        }

        void NumericFieldBuilder::SetBitBase
            (
            BitBase::Type const aBitBase
            )
        {
            mBitBase = aBitBase;
        }

        bool NumericFieldBuilder::CalcBits
            (
            char const* aText,
            size_t& aOut
            )
        {
            unsigned long int v = std::strtoul( aText, NULL, 10 );
            v *= mBitBase; // Multiplication overflow still results in a huge number

            if( v > MAX_NUMERIC_FIELD_BITS )
            {
                // Conversion is invalid or the number is too high
                // strtoul() returns std::ULONG_MAX on failure, but input is validated anyway.
                return false;
            }

            aOut = static_cast< size_t >( v );
            return true;
        }

        bool NumericFieldBuilder::ParseSign
            (
            char const aChar
            )
        {
            if( aChar == 'u' )
            {
                mProps.mSigned = false;
            }
            else if( aChar == 's' )
            {
                mProps.mSigned = true;
            }
            else
            {
                // Invalid sign
                return false;
            }

            return true;
        }

    } // namespace Objects

} // namespace BfsdlParser

/**
    BFSDL Parser Numeric Literal Tests

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

#include "gtest/gtest.h"

#include <cstring>

#include "BfsdlParser/Objects/NumericLiteral.hpp"
#include "BfsdlTests/TestUtil.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using namespace BfsdlParser;

    class NumericLiteralTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        NumericLiteralTest()
        {
        }
    };

    TEST_F( NumericLiteralTest, DefaultState )
    {
        BfsdlParser::Objects::NumericLiteral literal;

        ASSERT_FALSE( literal.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );
    }

    TEST_F( NumericLiteralTest, SetValueBase10 )
    {
        BfsdlParser::Objects::NumericLiteral literal;

        // This test verifies setting each part of the literal rough order of how they might
        // appear from the parser.

        // 1) Significand sign -- no observable effect yet
        literal.SetSignificandSign( Data::Sign::Positive );
        ASSERT_FALSE( literal.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );

        // Can't set digits without radix--no effect on output
        ASSERT_FALSE( literal.SetSignificandIntegralDigits( "13" ) );
        ASSERT_FALSE( literal.SetSignificandFractionalDigits( "24" ) );
        ASSERT_FALSE( literal.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );

        // 2) Radix (used by all parts)
        literal.SetRadix( 10 );
        ASSERT_FALSE( literal.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );

        // Can't set digits outside of range--no effect on output
        ASSERT_FALSE( literal.SetSignificandIntegralDigits( "1abc" ) );
        ASSERT_FALSE( literal.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );

        // 3) Significand integral digits
        ASSERT_TRUE( literal.SetSignificandIntegralDigits( "123" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123", literal.GetStr( true ).c_str() );

        // Can't set digits outside of range--no effect on output
        ASSERT_FALSE( literal.SetSignificandFractionalDigits( "1abc" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123", literal.GetStr( true ).c_str() );

        // 4) Significand fractional digits
        ASSERT_TRUE( literal.SetSignificandFractionalDigits( "456" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // 5) Exponent sign
        literal.SetExponentSign( Data::Sign::Negative );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // Can't set exponent digits out of range
        ASSERT_FALSE( literal.SetExponentDigits( "1abc" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // 6) Exponent digits
        ASSERT_TRUE( literal.SetExponentDigits( "789" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123.456 x +2 ^ -789", literal.GetStr( true ).c_str() );
    }

    TEST_F( NumericLiteralTest, SetValueBase2 )
    {
        BfsdlParser::Objects::NumericLiteral literal;

        // Like SetValueBase10, but shorter and with base 2.

        literal.SetSignificandSign( Data::Sign::Negative );
        literal.SetRadix( 2 );
        ASSERT_TRUE( literal.SetSignificandIntegralDigits( "101" ) );
        ASSERT_TRUE( literal.SetSignificandFractionalDigits( "110" ) );
        literal.SetExponentSign( Data::Sign::Positive );
        ASSERT_TRUE( literal.SetExponentDigits( "011" ) );

        ASSERT_TRUE( literal.IsDefined() );
        // Note that for base 2, the base part of the exponent is shown differently
        ASSERT_STREQ( "-101.110 x +10 ^ +011", literal.GetStr( true ).c_str() );
    }

} // namespace BfsdlTests

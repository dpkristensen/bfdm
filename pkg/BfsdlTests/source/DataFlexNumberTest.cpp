/**
    BFDP Data FlexNumber Test

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

#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/Data/FlexNumber.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    class DataFlexNumberTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    using namespace Bfdp;
    using Data::FlexNumber;
    using Data::Sign;

    TEST_F( DataFlexNumberTest, ComponentStates )
    {
        static struct TestDataType
        {
            Sign::Value sign;
            char const* integral;
            char const* fractional;
            bool defined;
            char const* str;
        } const testData[] =
        {
            // Variations of sign
            { Sign::Unspecified, NULL, NULL, false, "?0" },
            { Sign::Positive, NULL, NULL, false, "+0" },
            { Sign::Negative, NULL, NULL, false, "-0" },

            // Variations of integral values
            { Sign::Unspecified, "123", NULL, false, "?123" },
            { Sign::Positive, "123", NULL, true, "+123" },

            // Variations of fractional values
            { Sign::Unspecified, NULL, "456", false, "?0.456" },
            { Sign::Negative, NULL, "456", true, "-0.456" },

            // Variations of combined values
            { Sign::Unspecified, "123", "456", false, "?123.456" },
            { Sign::Positive, "123", "456", true, "+123.456" },
            { Sign::Negative, "123", "456", true, "-123.456" }
        };
        static SizeT const testCount = BFDP_COUNT_OF_ARRAY( testData );

        for( SizeT i = 0; i < testCount; ++i )
        {
            SCOPED_TRACE
                (
                ::testing::Message( "[" ) << i << "]"
                    << " s=" << Sign( testData[i].sign ).GetExplicitStr()
                    << " i=" << StrOrNull( testData[i].integral )
                    << " f=" << StrOrNull( testData[i].fractional )
                );

            FlexNumber::Component c;
            c.sign = testData[i].sign;
            if( testData[i].integral )
            {
                ASSERT_TRUE( c.integral.Set( testData[i].integral, 10 ) );
            }
            if( testData[i].fractional )
            {
                ASSERT_TRUE( c.fractional.Set( testData[i].fractional, 10 ) );
            }
            ASSERT_EQ( testData[i].defined, c.IsDefined() );
            ASSERT_STREQ( testData[i].str, c.GetStr( true ).c_str() );
        }

    }

    TEST_F( DataFlexNumberTest, DefaultState )
    {
        FlexNumber defaultNumber;
        ASSERT_FALSE( defaultNumber.IsDefined() );
        ASSERT_STREQ( "", defaultNumber.GetStr().c_str() );
        ASSERT_STREQ( "", defaultNumber.GetStr( true ).c_str() );
    }

    TEST_F( DataFlexNumberTest, Set )
    {
        FlexNumber number;

        // Significand

        number.significand.sign = Data::Sign::Positive;
        ASSERT_FALSE( number.IsDefined() );
        ASSERT_FALSE( number.significand.IsDefined() );
        ASSERT_FALSE( number.significand.integral.IsDefined() );
        ASSERT_FALSE( number.significand.fractional.IsDefined() );
        ASSERT_STREQ( "", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.significand.integral.Set( "135", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.significand.IsDefined() );
        ASSERT_TRUE( number.significand.integral.IsDefined() );
        ASSERT_FALSE( number.significand.fractional.IsDefined() );
        ASSERT_STREQ( "+135", number.GetStr( true ).c_str() );

        number.significand.integral.Reset();
        ASSERT_FALSE( number.IsDefined() );
        ASSERT_FALSE( number.significand.IsDefined() );
        ASSERT_FALSE( number.significand.integral.IsDefined() );
        ASSERT_FALSE( number.significand.fractional.IsDefined() );
        ASSERT_STREQ( "", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.significand.fractional.Set( "def", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.significand.IsDefined() );
        ASSERT_FALSE( number.significand.integral.IsDefined() );
        ASSERT_TRUE( number.significand.fractional.IsDefined() );
        ASSERT_STREQ( "+0.def", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.significand.integral.Set( "abc", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.significand.IsDefined() );
        ASSERT_TRUE( number.significand.integral.IsDefined() );
        ASSERT_TRUE( number.significand.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def", number.GetStr( true ).c_str() );

        // Base

        ASSERT_TRUE( number.base.fractional.Set( "456", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_FALSE( number.base.IsDefined() );
        ASSERT_FALSE( number.base.integral.IsDefined() );
        ASSERT_TRUE( number.base.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.base.integral.Set( "123", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_FALSE( number.base.IsDefined() );
        ASSERT_TRUE( number.base.integral.IsDefined() );
        ASSERT_TRUE( number.base.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def", number.GetStr( true ).c_str() );

        number.base.sign = Data::Sign::Positive;
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.base.IsDefined() );
        ASSERT_TRUE( number.base.integral.IsDefined() );
        ASSERT_TRUE( number.base.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def", number.GetStr( true ).c_str() );

        // Exponent

        number.exponent.sign = Data::Sign::Negative;
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_FALSE( number.exponent.IsDefined() );
        ASSERT_FALSE( number.exponent.integral.IsDefined() );
        ASSERT_FALSE( number.exponent.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.exponent.integral.Set( "78", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.exponent.IsDefined() );
        ASSERT_TRUE( number.exponent.integral.IsDefined() );
        ASSERT_FALSE( number.exponent.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def x +123.456 ^ -78", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.exponent.fractional.Set( "90", 16 ) );
        ASSERT_TRUE( number.IsDefined() );
        ASSERT_TRUE( number.exponent.IsDefined() );
        ASSERT_TRUE( number.exponent.integral.IsDefined() );
        ASSERT_TRUE( number.exponent.fractional.IsDefined() );
        ASSERT_STREQ( "+abc.def x +123.456 ^ -78.90", number.GetStr( true ).c_str() );

        ASSERT_TRUE( number.significand.IsDefined() );
        ASSERT_TRUE( number.significand.integral.IsDefined() );
        ASSERT_TRUE( number.significand.fractional.IsDefined() );
        ASSERT_TRUE( number.base.IsDefined() );
        ASSERT_TRUE( number.base.integral.IsDefined() );
        ASSERT_TRUE( number.base.fractional.IsDefined() );
        ASSERT_TRUE( number.exponent.IsDefined() );
        ASSERT_TRUE( number.exponent.integral.IsDefined() );
        ASSERT_TRUE( number.exponent.fractional.IsDefined() );
    }

    TEST_F( DataFlexNumberTest, Reset )
    {
        FlexNumber number;

        number.significand.sign = Data::Sign::Positive;
        ASSERT_TRUE( number.significand.integral.Set( "abc", 16 ) );
        ASSERT_TRUE( number.significand.fractional.Set( "def", 16 ) );

        number.base.sign = Data::Sign::Positive;
        ASSERT_TRUE( number.base.integral.Set( "123", 16 ) );
        ASSERT_TRUE( number.base.fractional.Set( "456", 16 ) );

        number.exponent.sign = Data::Sign::Negative;
        ASSERT_TRUE( number.exponent.integral.Set( "78", 16 ) );
        ASSERT_TRUE( number.exponent.fractional.Set( "90", 16 ) );

        ASSERT_STREQ( "+abc.def x +123.456 ^ -78.90", number.GetStr( true ).c_str() );

        number.Reset();
        ASSERT_STREQ( "", number.GetStr( true ).c_str() );

        ASSERT_FALSE( number.significand.IsDefined() );
        ASSERT_FALSE( number.significand.integral.IsDefined() );
        ASSERT_FALSE( number.significand.fractional.IsDefined() );
        ASSERT_FALSE( number.base.IsDefined() );
        ASSERT_FALSE( number.base.integral.IsDefined() );
        ASSERT_FALSE( number.base.fractional.IsDefined() );
        ASSERT_FALSE( number.exponent.IsDefined() );
        ASSERT_FALSE( number.exponent.integral.IsDefined() );
        ASSERT_FALSE( number.exponent.fractional.IsDefined() );
    }

} // namespace BfsdlTests

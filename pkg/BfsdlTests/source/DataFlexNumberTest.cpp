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
            bool isDefined;
            bool isIntegral;
            char const* str;
        } const testData[] =
        {
            // Variations of sign
            { Sign::Unspecified, NULL, NULL, false, false, "?0" },
            { Sign::Positive, NULL, NULL, false, false, "+0" },
            { Sign::Negative, NULL, NULL, false, false, "-0" },

            // Variations of integral values
            { Sign::Unspecified, "123", NULL, false, false, "?123" },
            { Sign::Positive, "123", NULL, true, true, "+123" },

            // Variations of fractional values
            { Sign::Unspecified, NULL, "456", false, false, "?0.456" },
            { Sign::Negative, NULL, "456", true, false, "-0.456" },

            // Variations of combined values
            { Sign::Unspecified, "123", "456", false, false, "?123.456" },
            { Sign::Positive, "123", "456", true, false, "+123.456" },
            { Sign::Negative, "123", "456", true, false, "-123.456" }
        };
        static size_t const testCount = BFDP_COUNT_OF_ARRAY( testData );

        for( size_t i = 0; i < testCount; ++i )
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
            ASSERT_EQ( testData[i].isDefined, c.IsDefined() );
            ASSERT_EQ( testData[i].isIntegral, c.IsIntegral() );
            ASSERT_STREQ( testData[i].str, c.GetStr( true ).c_str() );

        }
    }

    TEST_F( DataFlexNumberTest, DefaultState )
    {
        FlexNumber defaultNumber;
        ASSERT_FALSE( defaultNumber.IsDefined() );
        ASSERT_FALSE( defaultNumber.HasSignificand() );
        ASSERT_FALSE( defaultNumber.HasExponent() );
        ASSERT_FALSE( defaultNumber.IsIntegral() );
        ASSERT_FALSE( defaultNumber.significand.IsDefined() );
        ASSERT_FALSE( defaultNumber.significand.sign.IsSpecified() );
        ASSERT_FALSE( defaultNumber.significand.integral.IsDefined() );
        ASSERT_FALSE( defaultNumber.significand.fractional.IsDefined() );
        ASSERT_FALSE( defaultNumber.exponent.IsDefined() );
        ASSERT_FALSE( defaultNumber.exponent.sign.IsSpecified() );
        ASSERT_FALSE( defaultNumber.exponent.integral.IsDefined() );
        ASSERT_FALSE( defaultNumber.exponent.fractional.IsDefined() );
        ASSERT_STREQ( "", defaultNumber.GetStr().c_str() );
        ASSERT_STREQ( "", defaultNumber.GetStr( true ).c_str() );
    }

    TEST_F( DataFlexNumberTest, Set )
    {
        static struct TestDataType
        {
            struct TestDataComponentType
            {
                Sign::Value sign;
                char const* integral;
                char const* fractional;
                bool isDefined;
                bool isIntegral;

                void Verify
                    (
                    FlexNumber::Component& aComponent
                    ) const
                {
                    aComponent.sign = sign;
                    if( integral )
                    {
                        ASSERT_TRUE( aComponent.integral.Set( integral, 16 ) );
                    }
                    if( fractional )
                    {
                        ASSERT_TRUE( aComponent.fractional.Set( fractional, 16 ) );
                    }
                    ASSERT_EQ( isDefined, aComponent.IsDefined() );
                    ASSERT_EQ( isIntegral, aComponent.IsIntegral() );
                }
            };
            TestDataComponentType sig;
            TestDataComponentType base;
            TestDataComponentType exp;
            bool hasExponent;
            bool hasSignificand;
            bool isDefined;
            bool isIntegral;
            char const* resultStr;
        } const testData[] =
        {
            { // Case 0: sign
                // Sign              int    frac  isDef? isInt?
                { Sign::Positive,    NULL,  NULL, false, false }, // integral
                { Sign::Unspecified, NULL,  NULL, false, false }, // base
                { Sign::Unspecified, NULL,  NULL, false, false }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                false,   false, false, false,  ""
            },
            { // Case 1: sign + integral
                // Sign              int   frac   isDef? isInt?
                { Sign::Positive,    "12", NULL,  true,  true }, // integral
                { Sign::Unspecified, NULL, NULL, false, false }, // base
                { Sign::Unspecified, NULL, NULL, false, false }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                false,   true,  true,  true,  "+12"
            },
            { // Case 2: sign + fractional
                // Sign              int    frac  isDef? isInt?
                { Sign::Positive,    NULL,  "ab",  true, false }, // integral
                { Sign::Unspecified, NULL,  NULL, false, false }, // base
                { Sign::Unspecified, NULL,  NULL, false, false }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                false,   true,  true, false,  "+0.ab"
            },
            { // Case 3: sign + integral + fractional
                // Sign              int    frac  isDef? isInt?
                { Sign::Negative,    "12",  "34",  true, false }, // integral
                { Sign::Unspecified, NULL,  NULL, false, false }, // base
                { Sign::Unspecified, NULL,  NULL, false, false }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                false,   true,  true, false,  "-12.34"
            },
            { // Case 4: all component elements
                // Sign              int    frac  isDef? isInt?
                { Sign::Positive,    "12",  "34",  true, false }, // integral
                { Sign::Positive,    "56",  "78",  true, false }, // base
                { Sign::Negative,    "90",  "ab",  true, false }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                true,    true,  true, false,  "+12.34 x +56.78 ^ -90.ab"
            },
            { // Case 5: int part only
                // Sign              int    frac  isDef? isInt?
                { Sign::Positive,    "12",  NULL,  true,  true }, // integral
                { Sign::Positive,    "56",  NULL,  true,  true }, // base
                { Sign::Negative,    "90",  NULL,  true,  true }, // fractional
                // hasE? hasS?  isDef? isInt?  string
                true,    true,  true, false,  "+12 x +56 ^ -90"
            },
        };
        static size_t const testCount = BFDP_COUNT_OF_ARRAY( testData );

        FlexNumber number;

        for( size_t i = 0; i < testCount; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i=" ) << i );

            number.Reset();  // Reset between iterations

            ASSERT_NO_FATAL_FAILURE( testData[i].sig.Verify( number.significand ) );
            ASSERT_NO_FATAL_FAILURE( testData[i].base.Verify( number.base ) );
            ASSERT_NO_FATAL_FAILURE( testData[i].exp.Verify( number.exponent ) );
            ASSERT_EQ( testData[i].hasExponent, number.HasExponent() );
            ASSERT_EQ( testData[i].hasSignificand, number.HasSignificand() );
            ASSERT_EQ( testData[i].isDefined, number.IsDefined() );
            ASSERT_EQ( testData[i].isIntegral, number.IsIntegral() );
            ASSERT_STREQ( testData[i].resultStr, number.GetStr( true ).c_str() );
        }
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

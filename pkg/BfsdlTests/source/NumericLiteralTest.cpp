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

    TEST_F( NumericLiteralTest, GetUint )
    {
        static Data::Sign::Value const UNS = Data::Sign::Unspecified;
        static Data::Sign::Value const NEG = Data::Sign::Negative;
        static Data::Sign::Value const POS = Data::Sign::Positive;
        static uint64_t const X = 0ULL;
        static struct TestDataType
        {
            Data::RadixType inRadix;
            Data::Sign inSign;
            char const* inDigits;
            size_t inMaxBits;
            bool outResult;
            uint64_t outValue;
        } TestData[] = {
            // Limits w/value = 0
            { 10, POS, "0", 1, true, 0 }, // Narrow 0
            { 10, POS, "0", 64, true, 0 }, // Wide 0
            { 10, POS, "0", 65, false, X }, // 65 bits too large
            {  2, POS, "0", 1, true, 0 }, // 0 in different base

            // Bad input
            { 10, POS, "0", 0, false, X }, // 0 bits fail
            { 10, NEG, "0", 1, false, X }, // Negative fail

            // Value=1
            {  2, POS, "1", 1, true, 1 },
            {  8, POS, "1", 1, true, 1 },
            { 10, POS, "1", 1, true, 1 },
            { 16, POS, "1", 1, true, 1 },

            // Value=10
            {  2, POS, "10", 5, true, 2 },
            {  8, POS, "10", 5, true, 8 },
            { 10, POS, "10", 5, true, 10 },
            { 16, POS, "10", 5, true, 16 },

            // 8 bit max => uint8
            {  2, POS, "11111111", 8, true, UINT8_MAX },
            {  8, POS, "377", 8, true, UINT8_MAX },
            { 10, POS, "255", 8, true, UINT8_MAX },
            { 16, POS, "ff", 8, true, UINT8_MAX },

            // 8 bit max with leading 0 => uint8
            {  2, POS, "011111111", 8, true, UINT8_MAX },
            {  8, POS, "0377", 8, true, UINT8_MAX },
            { 10, POS, "0255", 8, true, UINT8_MAX },
            { 16, POS, "0ff", 8, true, UINT8_MAX },

            // 8 bit max with leading 0 => uint16
            {  2, POS, "011111111", 9, true, UINT8_MAX },
            {  8, POS, "0377", 9, true, UINT8_MAX },
            { 10, POS, "0255", 9, true, UINT8_MAX },
            { 16, POS, "0ff", 9, true, UINT8_MAX },

            // (8 bit max + 1) => uint8
            {  2, POS, "100000000", 8, false, X },
            {  8, POS, "400", 8, false, X },
            { 10, POS, "256", 8, false, X },
            { 16, POS, "100", 8, false, X },

            // (8 bit max + 1) => uint16
            {  2, POS, "100000000", 9, true, UINT8_MAX + 1 },
            {  8, POS, "400", 9, true, UINT8_MAX + 1 },
            { 10, POS, "256", 9, true, UINT8_MAX + 1 },
            { 16, POS, "100", 9, true, UINT8_MAX + 1 },

            // 16 bit max => uint16
            {  2, POS, "1111111111111111", 16, true, UINT16_MAX },
            {  8, POS, "177777", 16, true, UINT16_MAX },
            { 10, POS, "65535", 16, true, UINT16_MAX },
            { 16, POS, "ffff", 16, true, UINT16_MAX },

            // 32 bit max => uint32
            {  2, POS, "11111111111111111111111111111111", 32, true, UINT32_MAX },
            {  8, POS, "37777777777", 32, true, UINT32_MAX },
            { 10, POS, "4294967295", 32, true, UINT32_MAX },
            { 16, POS, "ffffffff", 32, true, UINT32_MAX },

            // 64 bit max => uint64
            {  2, POS, "1111111111111111111111111111111111111111111111111111111111111111", 64, true, UINT64_MAX },
            {  8, POS, "777777777777777777777", 63, true, 0x7fffffffffffffffULL }, // NOTE: 63 bits since octal...
            { 10, POS, "18446744073709551615", 64, true, UINT64_MAX },
            { 16, POS, "ffffffffffffffff", 64, true, UINT64_MAX },

            // (64 bit max + 1) => uint64
            {  2, POS, "10000000000000000000000000000000000000000000000000000000000000000", 64, false, X },
            {  8, POS, "1777777777777777777777", 64, false, X }, // 66 bits
            { 10, POS, "18446744073709551616", 64, false, X },
            { 16, POS, "10000000000000000", 64, false, X },
        };
        static size_t const NumTests = BFDP_COUNT_OF_ARRAY( TestData );

        for( size_t i = 0; i < NumTests; ++i )
        {
            SCOPED_TRACE
                (
                ::testing::Message( "radix=" ) << TestData[i].inRadix
                << " s=" << TestData[i].inSign
                << " d=" << TestData[i].inDigits
                << " b=" << TestData[i].inMaxBits
                );

            BfsdlParser::Objects::NumericLiteral literal;
            literal.SetRadix( TestData[i].inRadix );
            literal.SetSignificandSign( TestData[i].inSign );
            ASSERT_TRUE( literal.SetSignificandIntegralDigits( TestData[i].inDigits ) );
            ASSERT_TRUE( literal.SetDefaultBase() );
            ASSERT_TRUE( literal.IsDefined() );
            if( TestData[i].inMaxBits <= 8 )
            {
                uint8_t u8;
                ASSERT_EQ( TestData[i].outResult, literal.GetUint( u8, TestData[i].inMaxBits ) );
                if( TestData[i].outResult )
                {
                    ASSERT_EQ( TestData[i].outValue, u8 );
                }
            }
            else if( TestData[i].inMaxBits <= 16 )
            {
                uint16_t u16;
                ASSERT_EQ( TestData[i].outResult, literal.GetUint( u16, TestData[i].inMaxBits ) );
                if( TestData[i].outResult )
                {
                    ASSERT_EQ( TestData[i].outValue, u16 );
                }
            }
            else if( TestData[i].inMaxBits <= 32 )
            {
                uint32_t u32;
                ASSERT_EQ( TestData[i].outResult, literal.GetUint( u32, TestData[i].inMaxBits ) );
                if( TestData[i].outResult )
                {
                    ASSERT_EQ( TestData[i].outValue, u32 );
                }
            }
            else
            {
                uint64_t u64;
                ASSERT_EQ( TestData[i].outResult, literal.GetUint( u64, TestData[i].inMaxBits ) );
                if( TestData[i].outResult )
                {
                    ASSERT_EQ( TestData[i].outValue, u64 );
                }
            }
        }

        BfsdlParser::Objects::NumericLiteral badLiteral;
        uint64_t unusedOutput = 42U;

        // Literal with exponent cannot be converted
        badLiteral.SetRadix( 16 );
        badLiteral.SetSignificandSign( POS );
        ASSERT_TRUE( badLiteral.SetSignificandIntegralDigits( "1" ) );
        ASSERT_TRUE( badLiteral.SetDefaultBase() );
        badLiteral.SetExponentSign( POS );
        ASSERT_TRUE( badLiteral.SetExponentDigits( "1" ) );
        ASSERT_TRUE( badLiteral.IsDefined() );
        ASSERT_FALSE( badLiteral.GetUint( unusedOutput, 64 ) );

        // Literal with fractional significand cannot be converted
        badLiteral.Reset();
        badLiteral.SetRadix( 16 );
        badLiteral.SetSignificandSign( POS );
        ASSERT_TRUE( badLiteral.SetSignificandIntegralDigits( "1" ) );
        ASSERT_TRUE( badLiteral.SetSignificandFractionalDigits( "1" ) );
        ASSERT_TRUE( badLiteral.SetDefaultBase() );
        ASSERT_TRUE( badLiteral.IsDefined() );
        ASSERT_FALSE( badLiteral.GetUint( unusedOutput, 64 ) );
    }

    TEST_F( NumericLiteralTest, Reset )
    {
        BfsdlParser::Objects::NumericLiteral literal;

        literal.SetSignificandSign( Data::Sign::Positive );
        literal.SetRadix( 8 );
        ASSERT_TRUE( literal.HasRadix() );
        ASSERT_TRUE( literal.SetSignificandIntegralDigits( "123" ) );
        ASSERT_TRUE( literal.SetSignificandFractionalDigits( "456" ) );
        ASSERT_TRUE( literal.SetDefaultBase() );
        literal.SetExponentSign( Data::Sign::Negative );
        ASSERT_TRUE( literal.SetExponentDigits( "70" ) );

        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_STREQ( "+123.456 x +2 ^ -70", literal.GetStr( true ).c_str() );

        literal.Reset();
        ASSERT_FALSE( literal.HasRadix() );
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

        // Can't set digits without radix
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
        ASSERT_FALSE( literal.GetSignificand().IsDefined() );
        ASSERT_FALSE( literal.GetSignificand().integral.IsDefined() );
        ASSERT_STREQ( "", literal.GetStr( true ).c_str() );

        // 3) Significand integral digits
        ASSERT_TRUE( literal.SetSignificandIntegralDigits( "123" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_TRUE( literal.GetSignificand().IsDefined() );
        ASSERT_TRUE( literal.GetSignificand().integral.IsDefined() );
        ASSERT_FALSE( literal.GetSignificand().fractional.IsDefined() );
        ASSERT_STREQ( "+123", literal.GetStr( true ).c_str() );

        // Can't set digits outside of range--no effect on output
        ASSERT_FALSE( literal.SetSignificandFractionalDigits( "1abc" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_TRUE( literal.GetSignificand().IsDefined() );
        ASSERT_FALSE( literal.GetSignificand().fractional.IsDefined() );
        ASSERT_STREQ( "+123", literal.GetStr( true ).c_str() );

        // 4) Significand fractional digits
        ASSERT_TRUE( literal.SetSignificandFractionalDigits( "456" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_TRUE( literal.GetSignificand().IsDefined() );
        ASSERT_TRUE( literal.GetSignificand().fractional.IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // 5) Exponent sign (does not make the base or exponent valid, though)
        literal.SetExponentSign( Data::Sign::Negative );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_FALSE( literal.GetBase().IsDefined() );
        ASSERT_FALSE( literal.GetExponent().IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // Can't set exponent digits prior to base
        ASSERT_FALSE( literal.SetExponentDigits( "789" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_FALSE( literal.GetBase().IsDefined() );
        ASSERT_FALSE( literal.GetExponent().IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // 6) Base
        ASSERT_TRUE( literal.SetDefaultBase() );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_TRUE( literal.GetBase().IsDefined() );
        ASSERT_FALSE( literal.GetExponent().IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // Can't set exponent digits out of range
        ASSERT_FALSE( literal.SetExponentDigits( "1abc" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_FALSE( literal.GetExponent().IsDefined() );
        ASSERT_STREQ( "+123.456", literal.GetStr( true ).c_str() );

        // 7) Exponent digits
        ASSERT_TRUE( literal.SetExponentDigits( "789" ) );
        ASSERT_TRUE( literal.IsDefined() );
        ASSERT_TRUE( literal.GetExponent().IsDefined() );
        ASSERT_STREQ( "+123.456 x +2 ^ -789", literal.GetStr( true ).c_str() );
    }

    TEST_F( NumericLiteralTest, SetValueBase2 )
    {
        BfsdlParser::Objects::NumericLiteral literal;

        // Like SetValueBase10, but only happy path with base 2.

        literal.SetSignificandSign( Data::Sign::Negative );
        literal.SetRadix( 2 );
        ASSERT_TRUE( literal.SetSignificandIntegralDigits( "101" ) );
        ASSERT_TRUE( literal.SetDefaultBase() );
        ASSERT_TRUE( literal.SetSignificandFractionalDigits( "110" ) );
        literal.SetExponentSign( Data::Sign::Positive );
        ASSERT_TRUE( literal.SetExponentDigits( "011" ) );

        ASSERT_TRUE( literal.IsDefined() );
        // Note that for base 2, the base part of the exponent is shown differently
        ASSERT_STREQ( "-101.110 x +10 ^ +011", literal.GetStr( true ).c_str() );
    }

} // namespace BfsdlTests

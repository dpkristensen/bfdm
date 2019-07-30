/**
    BFDP Object NumericFieldBuilder Tests

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

#include "gtest/gtest.h"

#include "BfsdlParser/Objects/Common.hpp"
#include "BfsdlParser/Objects/NumericFieldBuilder.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::BitBase;
    using BfsdlParser::Objects::NumericField;
    using BfsdlParser::Objects::NumericFieldBuilder;
    using BfsdlParser::Objects::NumericFieldPtr;

    class ObjectNumericFieldBuilderTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        struct TestDataType
        {
            char const* inStr1;
            char const* inStr2;
            char const* outStr;
        };

        void RunTest
            (
            TestDataType const* const aTestData,
            size_t const aNumTests,
            BitBase::Type aBase
            )
        {
            NumericFieldBuilder builder;
            for( size_t i = 0; i < aNumTests; ++i )
            {
                SCOPED_TRACE( ::testing::Message( "str2=" ) << aTestData[i].inStr2 );
                SCOPED_TRACE( ::testing::Message( "str1=" ) << aTestData[i].inStr1 );
                SCOPED_TRACE( ::testing::Message( "base=" ) << aBase );

                // Reset between iterations
                builder.Reset();

                builder.SetBitBase( aBase );
                bool expectedResult = ( aTestData[i].outStr != NULL );

                ASSERT_TRUE( NULL == builder.GetField( "test" ) );

                bool result = builder.ParseIdentifier( aTestData[i].inStr1 );
                if( expectedResult )
                {
                    ASSERT_TRUE( result );
                }

                if( result )
                {
                    result = builder.ParseSuffix( aTestData[i].inStr2 );
                    ASSERT_EQ( expectedResult, result );
                }

                if( expectedResult )
                {
                    NumericFieldPtr f = builder.GetField( "test" );
                    ASSERT_TRUE( NULL != f );
                    ASSERT_STREQ( "test", f->GetName().c_str() );
                    ASSERT_STREQ( aTestData[i].outStr, f->GetTypeStr().c_str() );
                }
                else
                {
                    ASSERT_TRUE( NULL == builder.GetField( "test" ) );
                }
            }
        }
    };

    TEST_F( ObjectNumericFieldBuilderTest, ParseB1U )
    {
        // Bit-base unsigned types without fractional
        static TestDataType const sTestData[] =
        {
            { "u0",   "",     NULL },
            { "u1",   "",     "u1" },
            { "u8",   "",     "u8" },
            { "u16",  "",    "u16" },
            { "u32",  "",     "u32" },
            { "u64",  "",     "u64" },
            { "u65",  "",     NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Bit );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB1UF )
    {
        // Bit-base unsigned types with fractional
        static TestDataType const sTestData[] =
        {
            { "u0",   "0",    NULL },
            { "u0",   "1",    "u0.1" },
            { "u0",   "64",   "u0.64" },
            { "u1",   "0",    "u1" },
            { "u1",   "1",    "u1.1" },
            { "u1",   "63",   "u1.63" },
            { "u32",  "32",   "u32.32" },
            { "u63",  "1",    "u63.1" },
            { "u64",  "0",    "u64" },
            { "u64",  "",     "u64" },
            { "u1",   "64",   NULL },
            { "u64",  "1",    NULL },
            { "u64",  "64",   NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Bit );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB1S )
    {
        // Bit-base signed types without fractional
        static TestDataType const sTestData[] =
        {
            { "s0",   "",     NULL },
            { "s1",   "",     "s1" },
            { "s8",   "",     "s8" },
            { "s16",  "",     "s16" },
            { "s32",  "",     "s32" },
            { "s64",  "",     "s64" },
            { "s65",  "",     NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Bit );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB1SF )
    {
        // Bit-base signed types with fractional
        static TestDataType const sTestData[] =
        {
            { "s0",   "0",    NULL },
            { "s0",   "1",    "s0.1" },
            { "s0",   "64",   "s0.64" },
            { "s1",   "0",    "s1" },
            { "s1",   "1",    "s1.1" },
            { "s1",   "63",   "s1.63" },
            { "s32",  "32",   "s32.32" },
            { "s63",  "1",    "s63.1" },
            { "s64",  "0",    "s64" },
            { "s64",  "",     "s64" },
            { "s1",   "64",   NULL },
            { "s64",  "1",    NULL },
            { "s64",  "64",   NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Bit );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB8U )
    {
        // Byte-base unsigned types without fractional
        static TestDataType const sTestData[] =
        {
            { "u0",   "",     NULL },
            { "u1",   "",     "u8" },
            { "u2",   "",     "u16" },
            { "u3",   "",     "u24" },
            { "u7",   "",     "u56" },
            { "u8",   "",     "u64" },
            { "u9",   "",     NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Byte );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB8UF )
    {
        // Byte-base unsigned types with fractional
        static TestDataType const sTestData[] =
        {
            { "u0",   "0",    NULL },
            { "u0",   "1",    "u0.8" },
            { "u0",   "8",    "u0.64" },
            { "u1",   "0",    "u8" },
            { "u1",   "1",    "u8.8" },
            { "u1",   "7",    "u8.56" },
            { "u4",   "4",    "u32.32" },
            { "u7",   "1",    "u56.8" },
            { "u8",   "0",    "u64" },
            { "u8",   "",     "u64" },
            { "u1",   "8",    NULL },
            { "u8",   "1",    NULL },
            { "u8",   "8",    NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Byte );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB8S )
    {
        // Byte-base signed types without fractional
        static TestDataType const sTestData[] =
        {
            { "s0",   "",     NULL },
            { "s1",   "",     "s8" },
            { "s2",   "",     "s16" },
            { "s3",   "",     "s24" },
            { "s7",   "",     "s56" },
            { "s8",   "",     "s64" },
            { "s9",   "",     NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Byte );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseB8SF )
    {
        // Byte-base signed types with fractional
        static TestDataType const sTestData[] =
        {
            { "s0",   "0",    NULL },
            { "s0",   "1",    "s0.8" },
            { "s0",   "8",    "s0.64" },
            { "s1",   "0",    "s8" },
            { "s1",   "1",    "s8.8" },
            { "s1",   "7",    "s8.56" },
            { "s4",   "4",    "s32.32" },
            { "s7",   "1",    "s56.8" },
            { "s8",   "0",    "s64" },
            { "s8",   "",     "s64" },
            { "s1",   "8",    NULL },
            { "s8",   "1",    NULL },
            { "s8",   "8",    NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Byte );
    }

    TEST_F( ObjectNumericFieldBuilderTest, ParseInvalidSequences )
    {
        static TestDataType const sTestData[] =
        {
            { "",     "1",    NULL },
            { "a1",   "",     NULL },
            { "S1",   "",     NULL },
            { "U1",   "",     NULL },
            { "u1u",  "",     NULL },
            { "u1",   "u1",   NULL },
            { "u1",   "1u",   NULL },
            { "u",    "",     NULL },
            { "u",    "1",    NULL },
        };

        RunTest( sTestData, BFDP_COUNT_OF_ARRAY( sTestData ), BitBase::Bit );
    }

} // namespace BfsdlTests

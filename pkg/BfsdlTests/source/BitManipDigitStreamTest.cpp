/**
    BFDP BitManip Digit Stream Test

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

// External includes
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/BitManip/DigitStream.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class BitManipDigitStreamTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( BitManipDigitStreamTest, Create )
    {
        BitManip::DigitStream stream;

        ASSERT_EQ( Data::InvalidRadix, stream.GetRadix() );
        std::string outStr = stream.GetStr();
        ASSERT_STREQ( "", outStr.c_str() );
        ASSERT_EQ( 0U, stream.GetNumDigits() );

        // Nothing to iterate over
        BitManip::Digiterator iter = stream.GetIterator();
        ASSERT_TRUE( iter.IsDone() );
        ASSERT_EQ( 0U, iter.ReadDigit() );
    }

    TEST_F( BitManipDigitStreamTest, GetUint64 )
    {
        static uint64_t BFDP_CONSTEXPR X = 0ULL;

        BitManip::DigitStream stream;

        struct TestDataType
        {
            std::string input;
            Data::RadixType radix;
            bool result;
            uint64_t u64;
        } const testData[] =
        {
            { "0", 10, true, 0ULL },
            { "1", 10, true, 1ULL },
            { "18446744073709551615", 10, true, UINT64_MAX },
            { "18446744073709551616", 10, false, X },

            { "0", 2, true, 0ULL },
            { "1", 2, true, 1ULL },
            { "1111111111111111111111111111111111111111111111111111111111111111", 2, true, UINT64_MAX },
            { "10000000000000000000000000000000000000000000000000000000000000000", 2, false, X },

            { "0", 16, true, 0x0ULL },
            { "1", 16, true, 0x1ULL },
            { "FFFFFFFFFFFFFFFF", 16, true, UINT64_MAX },
            { "10000000000000000", 16, false, X },
        };
        static size_t const testCount = BFDP_COUNT_OF_ARRAY( testData );

        for( size_t i = 0; i < testCount; ++i )
        {
            SCOPED_TRACE
                (
                ::testing::Message( "[" ) << i << "]"
                    << " input=" << testData[i].input
                    << " radix=" << testData[i].radix
                );

            ASSERT_TRUE( stream.Set( testData[i].input, testData[i].radix ) );
            uint64_t out64;
            ASSERT_EQ( testData[i].result, stream.GetUint64( out64 ) );
            if( testData[i].result )
            {
                ASSERT_EQ( testData[i].u64, out64 );
            }

            stream.Reset();
        }
    }

    TEST_F( BitManipDigitStreamTest, IterateBase10 )
    {
        BitManip::DigitStream stream;

        ASSERT_TRUE( stream.Set( "109", 10 ) );
        ASSERT_TRUE( stream.IsDefined() );
        ASSERT_EQ( 3U, stream.GetNumDigits() );

        BitManip::Digiterator iter = stream.GetIterator();

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 1U, iter.ReadDigit() );

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 0U, iter.ReadDigit() );

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 9U, iter.ReadDigit() );

        ASSERT_TRUE( iter.IsDone() );
        ASSERT_EQ( 0U, iter.ReadDigit() ); // Read beyond end of stream is safe
    }

    TEST_F( BitManipDigitStreamTest, IterateBase2 )
    {
        BitManip::DigitStream stream;

        ASSERT_TRUE( stream.Set( "011", 2 ) );
        ASSERT_TRUE( stream.IsDefined() );
        ASSERT_EQ( 3U, stream.GetNumDigits() );

        BitManip::Digiterator iter = stream.GetIterator();

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 0U, iter.ReadDigit() );

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 1U, iter.ReadDigit() );

        ASSERT_FALSE( iter.IsDone() );
        ASSERT_EQ( 1U, iter.ReadDigit() );

        ASSERT_TRUE( iter.IsDone() );
        ASSERT_EQ( 0U, iter.ReadDigit() ); // Read beyond end of stream is safe
    }

    TEST_F( BitManipDigitStreamTest, SetDigits )
    {
        BitManip::DigitStream stream;

        struct TestDataType
        {
            std::string input;
            Data::RadixType radix;
            bool result;
            char const* output;
        } const testData[] =
        {
            // Invalid radix
            { "0", 1, false, "" },
            { "0", 37, false, "" },

            // Empty string
            { "", 2, true, "0" },
            { "", 8, true, "0" },

            // Invalid input
            { "1.", 2, false, "" },
            { "1e10", 2, false, "" },
            { "-1", 2, false, "" },

            // Base-2
            { "10110", 2, true, "10110" },
            { "0", 2, true, "0" },
            { "102", 2, false, "" },
            { "10a", 2, false, "" },

            // Base-8
            { "570", 8, true, "570" },
            { "579", 8, false, "" },

            // Base-10
            { "2990", 10, true, "2990" },
            { "1a", 10, false, "" },

            // Base-16
            { "126a", 16, true, "126a" },
            { "126A", 16, true, "126a" },
            { "126G", 16, false, "" }
        };
        static size_t const testCount = BFDP_COUNT_OF_ARRAY( testData );

        for( size_t i = 0; i < testCount; ++i )
        {
            SCOPED_TRACE
                (
                ::testing::Message( "[" ) << i << "]"
                    << " input=" << testData[i].input
                    << " radix=" << testData[i].radix
                );

            ASSERT_EQ( testData[i].result, stream.Set( testData[i].input, testData[i].radix ) );
            ASSERT_EQ( testData[i].result, stream.IsDefined() );
            if( testData[i].result )
            {
                ASSERT_EQ( testData[i].radix, stream.GetRadix() );
                ASSERT_EQ( testData[i].input.size(), stream.GetNumDigits() );
            }
            else
            {
                ASSERT_EQ( Data::InvalidRadix, stream.GetRadix() );
                ASSERT_EQ( 0U, stream.GetNumDigits() );
            }
            std::string outStr = stream.GetStr();
            ASSERT_STREQ( testData[i].output, outStr.c_str() );

            stream.Reset();
        }
    }

} // namespace BfsdlTests

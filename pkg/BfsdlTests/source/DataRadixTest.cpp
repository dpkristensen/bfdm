/**
    BFDP Data Radix Test

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
#include <cctype>
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/Data/Radix.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class DataRadixTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( DataRadixTest, ConvertBase )
    {
        struct TestDataType
        {
            Data::RadixType radix;
            char symbol;
            bool result;
            uint8_t value;
        } TestData[] =
        {
            // Base-1 number system not computationally possible
            {  1, '0', false, 0 },

            // Base-2
            {  2, '0', true, 0 },
            {  2, '1', true, 1 },
            {  2, '2', false, 2 },

            // Base-8
            {  8, '0', true, 0 },
            {  8, '7', true, 7 },
            {  8, '8', false, 8 },

            // Base-10
            { 10, '0', true, 0 },
            { 10, '9', true, 9 },
            { 10, 'A', false, 10 },
            { 10, 'a', false, 10 },

            // Base-16
            { 16, '0', true, 0 },
            { 16, 'f', true, 15 },
            { 16, 'F', true, 15 },
            { 16, 'g', false, 17 },
            { 16, 'G', false, 17 },

            // Base-36
            { 36, '0', true, 0 },
            { 36, 'z', true, 35 },
            { 36, 'Z', true, 35 },
            { 36, '[', false, 36 },
            { 36, '{', false, 36 },

            // Base-37 (not supported)
            { 37, '0', false, 0 },
            { 37, '9', false, 9 },
            { 37, 'a', false, 10 },
            { 37, 'A', false, 10 },
            { 37, 'z', false, 35 },
            { 37, 'Z', false, 35 },
        };
        static size_t const TestCount = BFDP_COUNT_OF_ARRAY( TestData );

        for( size_t i = 0; i < TestCount; ++i )
        {
            TestDataType& t = TestData[i];
            SCOPED_TRACE
                (
                ::testing::Message( "[" ) << i << "]"
                << " radix=" << t.radix
                << " char=" << t.symbol
                << " value=" << static_cast< unsigned int >( t.value )
                );

            uint8_t outValue;
            ASSERT_EQ( t.result, Data::ConvertBase( t.radix, t.symbol, outValue ) );
            if( t.result )
            {
                ASSERT_EQ( t.value, outValue );
            }

            char outChar;
            ASSERT_EQ( t.result, Data::ConvertBase( t.radix, t.value, outChar ) );
            if( t.result )
            {
                // Symbol output is lowercase
                ASSERT_EQ( std::tolower(t.symbol), outChar );
            }
        }
    }

    TEST_F( DataRadixTest, RadixProperties )
    {
        struct TestDataType
        {
            Data::RadixType radix;
            bool valid;
            size_t bits;
            bool powerOf2;
        } TestData[] =
        {
            {  0, false, 0, false },
            {  1, false, 0, false },
            {  2, true,  1, true  },
            {  3, true,  2, false },
            {  4, true,  2, true  },
            {  5, true,  3, false },
            {  7, true,  3, false },
            {  8, true,  3, true  },
            {  9, true,  4, false },
            { 10, true,  4, false },
            { 15, true,  4, false },
            { 16, true,  4, true  },
            { 17, true,  5, false },
            { 31, true,  5, false },
            { 32, true,  5, true  },
            { 33, true,  6, false },
            { 35, true,  6, false },
            { 36, true,  6, false },
            { 37, false, 0, false },
        };
        static size_t const TestCount = BFDP_COUNT_OF_ARRAY( TestData );

        for( size_t i = 0; i < TestCount; ++i )
        {
            TestDataType& t = TestData[i];
            SCOPED_TRACE( ::testing::Message( "radix=" ) << t.radix );

            ASSERT_EQ( t.valid, Data::IsValidRadix( t.radix ) );
            ASSERT_EQ( t.bits, Data::GetRadixBits( t.radix ) );
            ASSERT_EQ( t.powerOf2, Data::IsRadixPowerOf2( t.radix ) );
        }
    }

} // namespace BfsdlTests

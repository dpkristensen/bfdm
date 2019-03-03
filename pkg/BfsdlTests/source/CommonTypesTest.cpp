/**
    Tests for BFDP Common Types

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
#include "Bfdp/String.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    class CommonTypesTest
        : public ::testing::Test
    {
    };

    TEST_F( CommonTypesTest, IsWithinRange )
    {
        ASSERT_FALSE( IsWithinRange< uint8_t >( 1U, 0U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< uint8_t >( 1U, 1U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< uint8_t >( 1U, 2U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< uint8_t >( 1U, 3U, 3U ) );
        ASSERT_FALSE( IsWithinRange< uint8_t >( 1U, 4U, 3U ) );

        ASSERT_FALSE( IsWithinRange< int8_t >( -1, -2, 1 ) );
        ASSERT_TRUE(  IsWithinRange< int8_t >( -1, -1, 1 ) );
        ASSERT_TRUE(  IsWithinRange< int8_t >( -1,  0, 1 ) );
        ASSERT_TRUE(  IsWithinRange< int8_t >( -1,  1, 1 ) );
        ASSERT_FALSE( IsWithinRange< int8_t >( -1,  2, 1 ) );
    }

    TEST_F( CommonTypesTest, MemoryTypes )
    {
        // Verify assumptions about memory types
        ASSERT_EQ( sizeof( std::size_t ), sizeof( size_t ) );
        ASSERT_EQ( 1U, sizeof( Byte ) );
    }

    TEST_F( CommonTypesTest, NumericTypes )
    {
        // Verify assumptions about sized numeric types
        ASSERT_EQ( 1U, sizeof( uint8_t  ) );
        ASSERT_EQ( 2U, sizeof( uint16_t ) );
        ASSERT_EQ( 4U, sizeof( uint32_t ) );
        ASSERT_EQ( 8U, sizeof( uint64_t ) );

        ASSERT_EQ( 1U, sizeof( int8_t  ) );
        ASSERT_EQ( 2U, sizeof( int16_t ) );
        ASSERT_EQ( 4U, sizeof( int32_t ) );
        ASSERT_EQ( 8U, sizeof( int64_t ) );

        // Compare min/max values as LL and ULL to catch size errors for types < 64-bits
        ASSERT_EQ( -128LL,  INT8_MIN );
        ASSERT_EQ(  127LL,  INT8_MAX );
        ASSERT_EQ(    0ULL, UINT8_MIN );
        ASSERT_EQ(  255ULL, UINT8_MAX );

        ASSERT_EQ( -32768LL,  INT16_MIN );
        ASSERT_EQ(  32767LL,  INT16_MAX );
        ASSERT_EQ(      0ULL, UINT16_MIN );
        ASSERT_EQ(  65535ULL, UINT16_MAX );

        ASSERT_EQ( -2147483648LL,  INT32_MIN );
        ASSERT_EQ(  2147483647LL,  INT32_MAX );
        ASSERT_EQ(           0ULL, UINT32_MIN );
        ASSERT_EQ(  4294967295ULL, UINT32_MAX );

        ASSERT_EQ( -9223372036854775808LL,  INT64_MIN );
        ASSERT_EQ(  9223372036854775807LL,  INT64_MAX );
        ASSERT_EQ(                     0ULL, UINT64_MIN );
        ASSERT_EQ(  18446744073709551615ULL, UINT64_MAX );
    }

    TEST_F( CommonTypesTest, StrContains )
    {
        Byte const helloBytes[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f };
        Byte const hiBytes[] = { 0x48, 0x69 };
        std::string const helloStr = String( helloBytes, sizeof( helloBytes ) );
        std::string const hiStr = String( hiBytes, sizeof( hiBytes ) );

        ASSERT_STREQ( "Hello", helloStr.c_str() );
        ASSERT_STREQ( "Hi", hiStr.c_str() );

        ASSERT_FALSE( StrContains( helloStr, hiBytes, sizeof( hiBytes ) ) );
        ASSERT_FALSE( StrContains( hiStr, helloBytes, sizeof( helloBytes ) ) );

        ASSERT_TRUE( StrContains( helloStr, helloBytes, sizeof( helloBytes ) ) );
        ASSERT_TRUE( StrContains( hiStr, hiBytes, sizeof( hiBytes ) ) );

        Byte const loBytes[] = { 0x6c, 0x6f };
        ASSERT_TRUE( StrContains( helloStr, loBytes, sizeof( loBytes ) ) );
        ASSERT_FALSE( StrContains( hiStr, loBytes, sizeof( loBytes ) ) );

        Byte const elBytes[] = { 0x65, 0x6c };
        ASSERT_TRUE( StrContains( helloStr, elBytes, sizeof( elBytes ) ) );
        ASSERT_FALSE( StrContains( hiStr, elBytes, sizeof( elBytes ) ) );
    }

} // namespace BfsdlTests

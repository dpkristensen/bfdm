/**
    Tests for BFDP Common Types

    Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
        ASSERT_FALSE( IsWithinRange< UInt8 >( 1U, 0U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< UInt8 >( 1U, 1U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< UInt8 >( 1U, 2U, 3U ) );
        ASSERT_TRUE(  IsWithinRange< UInt8 >( 1U, 3U, 3U ) );
        ASSERT_FALSE( IsWithinRange< UInt8 >( 1U, 4U, 3U ) );

        ASSERT_FALSE( IsWithinRange< SInt8 >( -1U, -2U, 1U ) );
        ASSERT_TRUE(  IsWithinRange< SInt8 >( -1U, -1U, 1U ) );
        ASSERT_TRUE(  IsWithinRange< SInt8 >( -1U,  0U, 1U ) );
        ASSERT_TRUE(  IsWithinRange< SInt8 >( -1U,  1U, 1U ) );
        ASSERT_FALSE( IsWithinRange< SInt8 >( -1U,  2U, 1U ) );
    }

    TEST_F( CommonTypesTest, MemoryTypes )
    {
        // Verify assumptions about memory types
        ASSERT_EQ( sizeof( std::size_t ), sizeof( SizeT ) );
        ASSERT_EQ( 1U, sizeof( Byte ) );
    }

    TEST_F( CommonTypesTest, NumericTypes )
    {
        // Verify assumptions about sized numeric types
        ASSERT_EQ( 1U, sizeof( UInt8  ) );
        ASSERT_EQ( 2U, sizeof( UInt16 ) );
        ASSERT_EQ( 4U, sizeof( UInt32 ) );
        ASSERT_EQ( 8U, sizeof( UInt64 ) );

        ASSERT_EQ( 1U, sizeof( SInt8  ) );
        ASSERT_EQ( 2U, sizeof( SInt16 ) );
        ASSERT_EQ( 4U, sizeof( SInt32 ) );
        ASSERT_EQ( 8U, sizeof( SInt64 ) );

        // Compare min/max values as LL and ULL to catch size errors for types < 64-bits
        ASSERT_EQ( -128LL,  SInt8Min );
        ASSERT_EQ(  127LL,  SInt8Max );
        ASSERT_EQ(    0ULL, UInt8Min );
        ASSERT_EQ(  255ULL, UInt8Max );

        ASSERT_EQ( -32768LL,  SInt16Min );
        ASSERT_EQ(  32767LL,  SInt16Max );
        ASSERT_EQ(      0ULL, UInt16Min );
        ASSERT_EQ(  65535ULL, UInt16Max );

        ASSERT_EQ( -2147483648LL,  SInt32Min );
        ASSERT_EQ(  2147483647LL,  SInt32Max );
        ASSERT_EQ(           0ULL, UInt32Min );
        ASSERT_EQ(  4294967295ULL, UInt32Max );

        ASSERT_EQ( -9223372036854775808LL,  SInt64Min );
        ASSERT_EQ(  9223372036854775807LL,  SInt64Max );
        ASSERT_EQ(                     0ULL, UInt64Min );
        ASSERT_EQ(  18446744073709551615ULL, UInt64Max );
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

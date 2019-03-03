/**
    BFDP BitManip Mask Tests

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

#include "Bfdp/BitManip/Mask.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class BitManipMaskTest
        : public ::testing::Test
    {
        void SetUp()
        {
            // Uncomment the following lines to verify CreateMask does not work with invalid types
            //ASSERT_EQ( 0U, ( BitManip::CreateMask< int8_t >( 0 ) ) );
            //ASSERT_EQ( 0U, ( BitManip::CreateMask< float >( 0 ) ) );
        }
    };

    TEST_F( BitManipMaskTest, ExtractBits )
    {
        uint8_t const uv8 = 0xa7;
        ASSERT_EQ( 0x7U, BitManip::ExtractBits< uint8_t >( uv8, 4 ) );
        ASSERT_EQ( 0x7U, BitManip::ExtractBits< uint8_t >( uv8, 4, 0 ) );
        ASSERT_EQ( 0x9U, BitManip::ExtractBits< uint8_t >( uv8, 4, 2 ) );
        ASSERT_EQ( 0xaU, BitManip::ExtractBits< uint8_t >( uv8, 4, 4 ) );
        ASSERT_EQ( 0x5U, BitManip::ExtractBits< uint8_t >( uv8, 3, 5 ) );
        ASSERT_EQ( 0xa7U, BitManip::ExtractBits< uint8_t >( uv8, 8, 0 ) );

        // NOTE: expected value must be cast to int8_t because the == operator will perform
        // integer promotion and sign extension on the return value.
        int8_t const sv8 = -89; // 0xa7
        ASSERT_EQ( int8_t( 0x7 ), BitManip::ExtractBits< int8_t >( sv8, 4 ) );
        ASSERT_EQ( int8_t( 0x7 ), BitManip::ExtractBits< int8_t >( sv8, 4, 0 ) );
        ASSERT_EQ( int8_t( 0x9 ), BitManip::ExtractBits< int8_t >( sv8, 4, 2 ) );
        ASSERT_EQ( int8_t( 0xa ), BitManip::ExtractBits< int8_t >( sv8, 4, 4 ) );
        ASSERT_EQ( int8_t( 0x5 ), BitManip::ExtractBits< int8_t >( sv8, 3, 5 ) );
        ASSERT_EQ( int8_t( -89 ), BitManip::ExtractBits< int8_t >( sv8, 8, 0 ) );

        uint32_t const uv32 = 0x12345678UL;
        ASSERT_EQ( 0x00000008UL, BitManip::ExtractBits< uint32_t >( uv32, 4 ) );
        ASSERT_EQ( 0x00000018UL, BitManip::ExtractBits< uint32_t >( uv32, 5, 0 ) );
        ASSERT_EQ( 0x0000159eUL, BitManip::ExtractBits< uint32_t >( uv32, 16, 2 ) );
        ASSERT_EQ( 0x00000145UL, BitManip::ExtractBits< uint32_t >( uv32, 9, 12 ) );
        ASSERT_EQ( 0x091a2b3cUL, BitManip::ExtractBits< uint32_t >( uv32, 31, 1 ) );

        int32_t const sv32 = 0x12345678UL;
        ASSERT_EQ( 0x00000008UL, BitManip::ExtractBits< int32_t >( sv32, 4 ) );
        ASSERT_EQ( 0x00000018UL, BitManip::ExtractBits< int32_t >( sv32, 5, 0 ) );
        ASSERT_EQ( 0x0000159eUL, BitManip::ExtractBits< int32_t >( sv32, 16, 2 ) );
        ASSERT_EQ( 0x00000145UL, BitManip::ExtractBits< int32_t >( sv32, 9, 12 ) );
        ASSERT_EQ( 0x091a2b3cUL, BitManip::ExtractBits< int32_t >( sv32, 31, 1 ) );
    }

    TEST_F( BitManipMaskTest, MaskWidthZOffsetZ )
    {
        // Implicit offset = 0
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint8_t  >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint16_t >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint32_t >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint64_t >( 0 ) ) );

        // Explicit offset = 0
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint8_t  >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint16_t >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint32_t >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< uint64_t >( 0, 0 ) ) );

        // Explicit offset = 0
        ASSERT_EQ( 0, ( BitManip::CreateMask< int8_t  >( 0, 0 ) ) );
        ASSERT_EQ( 0, ( BitManip::CreateMask< int16_t >( 0, 0 ) ) );
        ASSERT_EQ( 0, ( BitManip::CreateMask< int32_t >( 0, 0 ) ) );
        ASSERT_EQ( 0, ( BitManip::CreateMask< int64_t >( 0, 0 ) ) );
    }

    TEST_F( BitManipMaskTest, MaskWidthNzOffsetZ )
    {
        // Implicit offset = 0
        ASSERT_EQ( 0x0000000000000001ULL, ( BitManip::CreateMask< uint64_t >( 1  ) ) );
        ASSERT_EQ( 0x00000000000000ffULL, ( BitManip::CreateMask< uint64_t >( 8  ) ) );
        ASSERT_EQ( 0x000000000000ffffULL, ( BitManip::CreateMask< uint64_t >( 16 ) ) );
        ASSERT_EQ( 0x00000000ffffffffULL, ( BitManip::CreateMask< uint64_t >( 32 ) ) );
        ASSERT_EQ( 0xffffffffffffffffULL, ( BitManip::CreateMask< uint64_t >( 64 ) ) );

        // Explicit offset = 0
        ASSERT_EQ( 0x0000000000000001ULL, ( BitManip::CreateMask< uint64_t >( 1,  0 ) ) );
        ASSERT_EQ( 0x00000000000000ffULL, ( BitManip::CreateMask< uint64_t >( 8,  0 ) ) );
        ASSERT_EQ( 0x000000000000ffffULL, ( BitManip::CreateMask< uint64_t >( 16, 0 ) ) );
        ASSERT_EQ( 0x00000000ffffffffULL, ( BitManip::CreateMask< uint64_t >( 32, 0 ) ) );
        ASSERT_EQ( 0xffffffffffffffffULL, ( BitManip::CreateMask< uint64_t >( 64, 0 ) ) );

        ASSERT_EQ( 0x0000000000000001LL, ( BitManip::CreateMask< int64_t >( 1,  0 ) ) );
        ASSERT_EQ( 0x00000000000000ffLL, ( BitManip::CreateMask< int64_t >( 8,  0 ) ) );
        ASSERT_EQ( 0x000000000000ffffLL, ( BitManip::CreateMask< int64_t >( 16, 0 ) ) );
        ASSERT_EQ( 0x00000000ffffffffLL, ( BitManip::CreateMask< int64_t >( 32, 0 ) ) );
        ASSERT_EQ( 0xffffffffffffffffLL, ( BitManip::CreateMask< int64_t >( 64, 0 ) ) );
    }

    TEST_F( BitManipMaskTest, MaskWidthNzOffsetNZ )
    {
        ASSERT_EQ( 0x0000000000000000ULL, ( BitManip::CreateMask< uint64_t >( 0,  1  ) ) );
        ASSERT_EQ( 0x0000000000000000ULL, ( BitManip::CreateMask< uint64_t >( 0,  63 ) ) );

        ASSERT_EQ( 0x0000000000000002ULL, ( BitManip::CreateMask< uint64_t >( 1,  1  ) ) );
        ASSERT_EQ( 0x0000000000000100ULL, ( BitManip::CreateMask< uint64_t >( 1,  8  ) ) );
        ASSERT_EQ( 0x0000000000010000ULL, ( BitManip::CreateMask< uint64_t >( 1,  16 ) ) );
        ASSERT_EQ( 0x0000000100000000ULL, ( BitManip::CreateMask< uint64_t >( 1,  32 ) ) );
        ASSERT_EQ( 0x8000000000000000ULL, ( BitManip::CreateMask< uint64_t >( 1,  63 ) ) );

        ASSERT_EQ( 0x0000000000000004ULL, ( BitManip::CreateMask< uint64_t >( 1,  2  ) ) );
        ASSERT_EQ( 0x0000000000000070ULL, ( BitManip::CreateMask< uint64_t >( 3,  4  ) ) );
        ASSERT_EQ( 0x00000000000007c0ULL, ( BitManip::CreateMask< uint64_t >( 5,  6  ) ) );
        ASSERT_EQ( 0x0000000000007f00ULL, ( BitManip::CreateMask< uint64_t >( 7,  8  ) ) );
        ASSERT_EQ( 0x000000000007fc00ULL, ( BitManip::CreateMask< uint64_t >( 9,  10 ) ) );

        ASSERT_EQ( 0x0000000000000004LL, ( BitManip::CreateMask< int64_t >( 1,  2  ) ) );
        ASSERT_EQ( 0x0000000000000070LL, ( BitManip::CreateMask< int64_t >( 3,  4  ) ) );
        ASSERT_EQ( 0x00000000000007c0LL, ( BitManip::CreateMask< int64_t >( 5,  6  ) ) );
        ASSERT_EQ( 0x0000000000007f00LL, ( BitManip::CreateMask< int64_t >( 7,  8  ) ) );
        ASSERT_EQ( 0x000000000007fc00LL, ( BitManip::CreateMask< int64_t >( 9,  10 ) ) );

        ASSERT_EQ( 0xfffffffffffffffeULL, ( BitManip::CreateMask< uint64_t >( 63,  1 ) ) );
        ASSERT_EQ( 0xfffffffffffffffcULL, ( BitManip::CreateMask< uint64_t >( 62,  2 ) ) );
        ASSERT_EQ( 0xfffffffffffffff8ULL, ( BitManip::CreateMask< uint64_t >( 61,  3 ) ) );
        ASSERT_EQ( 0xfffffffffffffff0ULL, ( BitManip::CreateMask< uint64_t >( 60,  4 ) ) );
    }

    TEST_F( BitManipMaskTest, ReplaceBits )
    {
        uint8_t const uv8 = 0xa7U;
        ASSERT_EQ( 0xaf, BitManip::ReplaceBits< uint8_t >( uv8, 0xfU, 4 ) );
        ASSERT_EQ( 0xaf, BitManip::ReplaceBits< uint8_t >( uv8, 0xfU, 4, 0 ) );
        ASSERT_EQ( 0x9b, BitManip::ReplaceBits< uint8_t >( uv8, 0x6U, 4, 2 ) );
        ASSERT_EQ( 0x77, BitManip::ReplaceBits< uint8_t >( uv8, 0x7U, 4, 4 ) );
        ASSERT_EQ( 0x27, BitManip::ReplaceBits< uint8_t >( uv8, 0x1U, 3, 5 ) );

        // NOTE: expected value must be cast to int8_t because the == operator will perform
        // integer promotion and sign extension on the return value.
        int8_t const sv8 = -89; // 0xa7
        ASSERT_EQ( int8_t( -81 ), BitManip::ReplaceBits< int8_t >( sv8, 0xf, 4 ) );
        ASSERT_EQ( int8_t( -81 ), BitManip::ReplaceBits< int8_t >( sv8, 0xf, 4, 0 ) );
        ASSERT_EQ( int8_t( -101 ), BitManip::ReplaceBits< int8_t >( sv8, 0x6, 4, 2 ) );
        ASSERT_EQ( int8_t( 0x77 ), BitManip::ReplaceBits< int8_t >( sv8, 0x7, 4, 4 ) );
        ASSERT_EQ( int8_t( 0x27 ), BitManip::ReplaceBits< int8_t >( sv8, 0x1, 3, 5 ) );

        uint32_t const uv32 = 0x12345678UL;
        ASSERT_EQ( 0x1234567eUL, BitManip::ReplaceBits< uint32_t >( uv32, 0xeUL, 4 ) );
        ASSERT_EQ( 0x1234566eUL, BitManip::ReplaceBits< uint32_t >( uv32, 0xeUL, 5, 0 ) );
        ASSERT_EQ( 0x1236af34UL, BitManip::ReplaceBits< uint32_t >( uv32, 0xabcdUL, 16, 2 ) );
        ASSERT_EQ( 0x122ff678UL, BitManip::ReplaceBits< uint32_t >( uv32, 0x0ffUL, 9, 12 ) );
        ASSERT_EQ( 0x12345678UL, BitManip::ReplaceBits< uint32_t >( uv32, 0x91a2b3cUL, 31, 1 ) );

        int32_t const sv32 = 0x12345678L;
        ASSERT_EQ( 0x1234567eUL, BitManip::ReplaceBits< int32_t >( sv32, 0xeL, 4 ) );
        ASSERT_EQ( 0x1234566eUL, BitManip::ReplaceBits< int32_t >( sv32, 0xeL, 5, 0 ) );
        ASSERT_EQ( 0x1236af34UL, BitManip::ReplaceBits< int32_t >( sv32, 0xabcdL, 16, 2 ) );
        ASSERT_EQ( 0x122ff678UL, BitManip::ReplaceBits< int32_t >( sv32, 0x0ffL, 9, 12 ) );
        ASSERT_EQ( 0x12345678UL, BitManip::ReplaceBits< int32_t >( sv32, 0x91a2b3cL, 31, 1 ) );
    }

} // namespace BfsdlTests

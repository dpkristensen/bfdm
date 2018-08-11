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
            //ASSERT_EQ( 0U, ( BitManip::CreateMask< SInt8 >( 0 ) ) );
            //ASSERT_EQ( 0U, ( BitManip::CreateMask< float >( 0 ) ) );
        }
    };

    TEST_F( BitManipMaskTest, WidthZOffsetZ )
    {
        // Implicit offset = 0
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt8  >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt16 >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt32 >( 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt64 >( 0 ) ) );

        // Explicit offset = 0
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt8  >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt16 >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt32 >( 0, 0 ) ) );
        ASSERT_EQ( 0U, ( BitManip::CreateMask< UInt64 >( 0, 0 ) ) );
    }

    TEST_F( BitManipMaskTest, WidthNzOffsetZ )
    {
        // Implicit offset = 0
        ASSERT_EQ( 0x0000000000000001ULL, ( BitManip::CreateMask< UInt64 >( 1  ) ) );
        ASSERT_EQ( 0x00000000000000ffULL, ( BitManip::CreateMask< UInt64 >( 8  ) ) );
        ASSERT_EQ( 0x000000000000ffffULL, ( BitManip::CreateMask< UInt64 >( 16 ) ) );
        ASSERT_EQ( 0x00000000ffffffffULL, ( BitManip::CreateMask< UInt64 >( 32 ) ) );
        ASSERT_EQ( 0xffffffffffffffffULL, ( BitManip::CreateMask< UInt64 >( 64 ) ) );

        // Explicit offset = 0
        ASSERT_EQ( 0x0000000000000001ULL, ( BitManip::CreateMask< UInt64 >( 1,  0 ) ) );
        ASSERT_EQ( 0x00000000000000ffULL, ( BitManip::CreateMask< UInt64 >( 8,  0 ) ) );
        ASSERT_EQ( 0x000000000000ffffULL, ( BitManip::CreateMask< UInt64 >( 16, 0 ) ) );
        ASSERT_EQ( 0x00000000ffffffffULL, ( BitManip::CreateMask< UInt64 >( 32, 0 ) ) );
        ASSERT_EQ( 0xffffffffffffffffULL, ( BitManip::CreateMask< UInt64 >( 64, 0 ) ) );
    }

    TEST_F( BitManipMaskTest, WidthNzOffsetNZ )
    {
        ASSERT_EQ( 0x0000000000000000ULL, ( BitManip::CreateMask< UInt64 >( 0,  1  ) ) );
        ASSERT_EQ( 0x0000000000000000ULL, ( BitManip::CreateMask< UInt64 >( 0,  63 ) ) );

        ASSERT_EQ( 0x0000000000000002ULL, ( BitManip::CreateMask< UInt64 >( 1,  1  ) ) );
        ASSERT_EQ( 0x0000000000000100ULL, ( BitManip::CreateMask< UInt64 >( 1,  8  ) ) );
        ASSERT_EQ( 0x0000000000010000ULL, ( BitManip::CreateMask< UInt64 >( 1,  16 ) ) );
        ASSERT_EQ( 0x0000000100000000ULL, ( BitManip::CreateMask< UInt64 >( 1,  32 ) ) );
        ASSERT_EQ( 0x8000000000000000ULL, ( BitManip::CreateMask< UInt64 >( 1,  63 ) ) );

        ASSERT_EQ( 0x0000000000000004ULL, ( BitManip::CreateMask< UInt64 >( 1,  2  ) ) );
        ASSERT_EQ( 0x0000000000000070ULL, ( BitManip::CreateMask< UInt64 >( 3,  4  ) ) );
        ASSERT_EQ( 0x00000000000007c0ULL, ( BitManip::CreateMask< UInt64 >( 5,  6  ) ) );
        ASSERT_EQ( 0x0000000000007f00ULL, ( BitManip::CreateMask< UInt64 >( 7,  8  ) ) );
        ASSERT_EQ( 0x000000000007fc00ULL, ( BitManip::CreateMask< UInt64 >( 9,  10 ) ) );

        ASSERT_EQ( 0xfffffffffffffffeULL, ( BitManip::CreateMask< UInt64 >( 63,  1 ) ) );
        ASSERT_EQ( 0xfffffffffffffffcULL, ( BitManip::CreateMask< UInt64 >( 62,  2 ) ) );
        ASSERT_EQ( 0xfffffffffffffff8ULL, ( BitManip::CreateMask< UInt64 >( 61,  3 ) ) );
        ASSERT_EQ( 0xfffffffffffffff0ULL, ( BitManip::CreateMask< UInt64 >( 60,  4 ) ) );
    }

} // namespace BfsdlTests
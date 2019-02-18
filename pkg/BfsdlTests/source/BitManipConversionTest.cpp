/**
    BFDP BitManip Conversion Test

    Copyright 2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/BitManip/Conversion.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class BitManipConversionTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( BitManipConversionTest, Assumptions )
    {
        ASSERT_EQ( 8U, BitManip::BitsPerByte );
        ASSERT_EQ( BitManip::MaxBits, BitManip::MaxBytes * BitManip::BitsPerByte );
    }

    TEST_F( BitManipConversionTest, BitsToBytes )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace wksp;

        ASSERT_EQ( 0U, BitManip::BitsToBytes( 0 ) );
        ASSERT_EQ( 1U, BitManip::BitsToBytes( 1 ) );
        ASSERT_EQ( 1U, BitManip::BitsToBytes( 7 ) );
        ASSERT_EQ( 1U, BitManip::BitsToBytes( 8 ) );
        ASSERT_EQ( 2U, BitManip::BitsToBytes( 9 ) );
        ASSERT_EQ( 2U, BitManip::BitsToBytes( 16 ) );
        ASSERT_EQ( 3U, BitManip::BitsToBytes( 17 ) );
        ASSERT_EQ( 3U, BitManip::BitsToBytes( 24 ) );
        ASSERT_EQ( 4U, BitManip::BitsToBytes( 25 ) );
        ASSERT_EQ( 7U, BitManip::BitsToBytes( 56 ) );
        ASSERT_EQ( 8U, BitManip::BitsToBytes( 57 ) );
        ASSERT_EQ( 8U, BitManip::BitsToBytes( 64 ) );
        ASSERT_EQ( BitManip::MaxBytes, BitManip::BitsToBytes( BitManip::MaxBits ) );

        wksp.ExpectMisuseError();
        ASSERT_EQ( BitManip::MaxBytes, BitManip::BitsToBytes( std::numeric_limits< SizeT >::max() ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );
    }

    TEST_F( BitManipConversionTest, BytesToBits )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace wksp;

        ASSERT_EQ(  0U, BitManip::BytesToBits( 0 ) );
        ASSERT_EQ(  8U, BitManip::BytesToBits( 1 ) );
        ASSERT_EQ( 16U, BitManip::BytesToBits( 2 ) );
        ASSERT_EQ( 64U, BitManip::BytesToBits( 8 ) );
        ASSERT_EQ( BitManip::MaxBits, BitManip::BytesToBits( BitManip::MaxBytes ) );

        wksp.ExpectMisuseError();
        ASSERT_EQ( BitManip::MaxBits, BitManip::BytesToBits( std::numeric_limits< SizeT >::max() ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );
    }

} // namespace BfsdlTests

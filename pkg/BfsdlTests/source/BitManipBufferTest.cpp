/**
    BFDP BitManip Buffer Test

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
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/Conversion.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class BitManipBufferTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( BitManipBufferTest, CreateAlignedBuffer )
    {
        static Byte const data[] = { 0x01, 0xC2, 0x3f };
        static SizeT const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static SizeT const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
        BitManip::BitBuffer buf( data, dataSizeBits );

        ASSERT_EQ( dataSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( dataSizeBits, buf.GetDataBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), data, dataSizeBytes ) );
    }

    TEST_F( BitManipBufferTest, CreateDefaultBuffer )
    {
        BitManip::BitBuffer buf;

        ASSERT_EQ( 0U, buf.GetCapacityBits() );
        ASSERT_EQ( 0U, buf.GetCapacityBytes() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_EQ( NULL, buf.GetDataPtr() );
    }

    TEST_F( BitManipBufferTest, CreateEmptyBuffer1 )
    {
        BitManip::BitBuffer buf( 0 );

        ASSERT_EQ( 0U, buf.GetCapacityBits() );
        ASSERT_EQ( 0U, buf.GetCapacityBytes() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_EQ( NULL, buf.GetDataPtr() );
    }

    TEST_F( BitManipBufferTest, CreateEmptyBuffer2 )
    {
        BitManip::BitBuffer buf( NULL, 0 );

        ASSERT_EQ( 0U, buf.GetCapacityBits() );
        ASSERT_EQ( 0U, buf.GetCapacityBytes() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_EQ( NULL, buf.GetDataPtr() );
    }

    TEST_F( BitManipBufferTest, CreateUnalignedBuffer )
    {
        static Byte const data[] = { 0x82, 0xa5, 0x3f };
        static SizeT const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static SizeT const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
        BitManip::BitBuffer buf( data, 20 );

        ASSERT_EQ( dataSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 20U, buf.GetDataBits() );
        ASSERT_EQ(  3U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), data, dataSizeBytes ) );
    }

} // namespace BfsdlTests

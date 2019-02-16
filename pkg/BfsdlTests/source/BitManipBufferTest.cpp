/**
    BFDP BitManip Buffer Test

    Copyright 2018-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
        ASSERT_FALSE( buf.IsEmpty() );
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
        ASSERT_TRUE( buf.IsEmpty() );
    }

    TEST_F( BitManipBufferTest, CreateEmptyBuffer1 )
    {
        BitManip::BitBuffer buf( 0 );

        ASSERT_EQ( 0U, buf.GetCapacityBits() );
        ASSERT_EQ( 0U, buf.GetCapacityBytes() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_EQ( NULL, buf.GetDataPtr() );
        ASSERT_TRUE( buf.IsEmpty() );
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
        ASSERT_FALSE( buf.IsEmpty() );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), data, dataSizeBytes ) );
    }

    TEST_F( BitManipBufferTest, ResizeBufferNoPreserve )
    {
        static SizeT const dataSizeBits = 12;
        static SizeT const dataSizeBytes = BitManip::BitsToBytes( dataSizeBits );

        BitManip::BitBuffer buf;

        // Ensure no crash occurs when setting with no buffer.
        buf.MemSet( 0x42 );

        ASSERT_TRUE( buf.ResizeNoPreserve( dataSizeBits ) );
        buf.MemSet( 0x7E );

        ASSERT_EQ( BitManip::BytesToBits( dataSizeBytes ), buf.GetCapacityBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( dataSizeBits, buf.GetDataBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetDataBytes() );
        ASSERT_FALSE( buf.IsEmpty() );
        for( SizeT i = 0; i < dataSizeBytes; ++i )
        {
            ASSERT_EQ( 0x7E, buf.GetDataPtr()[i] );
        }
    }

    TEST_F( BitManipBufferTest, ResizeBufferPreserve )
    {
        static Byte const initData[] = { 0xab, 0xcd, 0xef };
        static SizeT const initSizeBytes = BFDP_COUNT_OF_ARRAY( initData );
        static SizeT const initSizeBits = BitManip::BytesToBits( initSizeBytes );

        BitManip::BitBuffer buf( initData, initSizeBits );

        // Resize smaller
        ASSERT_TRUE( buf.ResizePreserve( 12 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 12, buf.GetDataBits() );
        ASSERT_EQ( 2, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        // Resize EVEN SMALLER
        ASSERT_TRUE( buf.ResizePreserve( 6 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 6, buf.GetDataBits() );
        ASSERT_EQ( 1, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        // Resize larger, within capacity
        ASSERT_TRUE( buf.ResizePreserve( 15 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 15, buf.GetDataBits() );
        ASSERT_EQ( 2, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        // Resize larger, beyond capacity by 1 byte
        ASSERT_TRUE( buf.ResizePreserve( 30 ) );
        ASSERT_EQ( 4, buf.GetCapacityBytes() );
        ASSERT_EQ( 30, buf.GetDataBits() );
        ASSERT_EQ( 4, buf.GetDataBytes() );
        Byte newData30[] = { 0xab, 0xcd };
        // Since the new bytes are uninitialized, only verify the original data up to the last
        // size (the 3rd byte is lost because it is not part of the data)
        ASSERT_TRUE( ArraysMatch( newData30, buf.GetDataPtr(), BFDP_COUNT_OF_ARRAY( newData30 ) ) );

        // For sanity in cases below, initialize the new byte
        buf.GetDataPtr()[2] = 0x88;
        buf.GetDataPtr()[3] = 0x99;

        // Resize again, initializing the new memory
        ASSERT_TRUE( buf.ResizePreserve( 36, 0x12 ) );
        ASSERT_EQ( 5, buf.GetCapacityBytes() );
        ASSERT_EQ( 36, buf.GetDataBits() );
        ASSERT_EQ( 5, buf.GetDataBytes() );
        Byte newData36[] = { 0xab, 0xcd, 0x88, 0x99, 0x12 };
        ASSERT_TRUE( ArraysMatch( newData36, buf.GetDataPtr(), 5 ) );

        // Resize again, initializing the new memory to another value
        ASSERT_TRUE( buf.ResizePreserve( 50, 0x34 ) );
        ASSERT_EQ( 7, buf.GetCapacityBytes() );
        ASSERT_EQ( 50, buf.GetDataBits() );
        ASSERT_EQ( 7, buf.GetDataBytes() );
        Byte newData42[] = { 0xab, 0xcd, 0x88, 0x99, 0x12, 0x34, 0x34 };
        ASSERT_TRUE( ArraysMatch( newData42, buf.GetDataPtr(), 7 ) );
    }

    TEST_F( BitManipBufferTest, WriteToBuffer )
    {
        static Byte const data[] = { 0x65, 0x66, 0x67 };
        static SizeT const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static SizeT const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
        BitManip::BitBuffer buf( data, dataSizeBits );

        buf.GetDataPtr()[1] = 0x42;

        static Byte const newData[] = { 0x65, 0x42, 0x67 };
        BFDP_CTIME_ASSERT
            (
            BFDP_COUNT_OF_ARRAY( data ) == BFDP_COUNT_OF_ARRAY( newData ),
            Data_Size_Mismatch
            );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), newData, dataSizeBytes ) );
    }

} // namespace BfsdlTests

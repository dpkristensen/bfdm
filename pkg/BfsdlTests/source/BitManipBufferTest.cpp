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
    protected:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

        ::testing::AssertionResult VerifyGuarantees( BitManip::BitBuffer const& aBuffer )
        {
            size_t const capBits = aBuffer.GetCapacityBits();
            size_t const capBytes = aBuffer.GetCapacityBytes();
            size_t const dataBits = aBuffer.GetDataBits();
            size_t const dataBitsDiv8 = dataBits / 8U;
            size_t const dataBytes = aBuffer.GetDataBytes();
            size_t const dataBytesMult8 = dataBytes * 8U;
            std::string errMsg;
            if( capBits != ( capBytes * 8 ) )
            {
                errMsg = "Unaligned Capacity";
            }
            else if( dataBits > capBits )
            {
                errMsg = "Data != Capacity (bits)";
            }
            else if( dataBytes > capBytes )
            {
                errMsg = "Data != Capacity (bytes)";
            }
            else if( ( dataBytesMult8 - dataBits ) > 8U )
            {
                errMsg = "Data Byte::Bit Conversion";
            }
            else if( ( dataBytes - dataBitsDiv8 ) > 1U )
            {
                errMsg = "Data Bit::Byte Conversion";
            }

            return errMsg.empty()
                ? ::testing::AssertionSuccess()
                : ::testing::AssertionFailure() << errMsg << std::endl
                    << "  capBits=" << capBits << std::endl
                    << "  capBytes=" << capBytes << std::endl
                    << "  dataBits=" << dataBits << std::endl
                    << "  dataBytes=" << dataBytes;
        }
    };

    TEST_F( BitManipBufferTest, CreateAlignedBuffer )
    {
        static Byte const data[] = { 0x01, 0xC2, 0x3f };
        static size_t const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static size_t const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
        BitManip::BitBuffer buf( data, dataSizeBits );

        ASSERT_EQ( dataSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( dataSizeBits, buf.GetDataBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetDataBytes() );
        ASSERT_FALSE( buf.IsEmpty() );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), data, dataSizeBytes ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
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

        ASSERT_TRUE( VerifyGuarantees( buf ) );
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

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, CreateEmptyBuffer2 )
    {
        BitManip::BitBuffer buf( NULL, 0 );

        ASSERT_EQ( 0U, buf.GetCapacityBits() );
        ASSERT_EQ( 0U, buf.GetCapacityBytes() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_EQ( NULL, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, CreateUnalignedBuffer )
    {
        static Byte const data[] = { 0x82, 0xa5, 0x3f };
        static size_t const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static size_t const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
        BitManip::BitBuffer buf( data, 20 );

        ASSERT_EQ( dataSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( dataSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 20U, buf.GetDataBits() );
        ASSERT_EQ(  3U, buf.GetDataBytes() );
        ASSERT_FALSE( buf.IsEmpty() );
        ASSERT_TRUE( ArraysMatch( buf.GetDataPtr(), data, dataSizeBytes ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, ResizeBufferNoPreserve )
    {
        static size_t const dataSizeBits = 12;
        static size_t const dataSizeBytes = BitManip::BitsToBytes( dataSizeBits );

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
        for( size_t i = 0; i < dataSizeBytes; ++i )
        {
            ASSERT_EQ( 0x7E, buf.GetDataPtr()[i] );
        }

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, ResizeBufferPreserve )
    {
        static Byte const initData[] = { 0xab, 0xcd, 0xef };
        static size_t const initSizeBytes = BFDP_COUNT_OF_ARRAY( initData );
        static size_t const initSizeBits = BitManip::BytesToBits( initSizeBytes );

        BitManip::BitBuffer buf( initData, initSizeBits );

        // Resize smaller
        ASSERT_TRUE( buf.ResizePreserve( 12 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 12U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize EVEN SMALLER
        ASSERT_TRUE( buf.ResizePreserve( 6 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 6U, buf.GetDataBits() );
        ASSERT_EQ( 1U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize larger, within capacity
        ASSERT_TRUE( buf.ResizePreserve( 15 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( 15U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize larger, beyond capacity by 1 byte
        ASSERT_TRUE( buf.ResizePreserve( 30 ) );
        ASSERT_EQ( 4U, buf.GetCapacityBytes() );
        ASSERT_EQ( 30U, buf.GetDataBits() );
        ASSERT_EQ( 4U, buf.GetDataBytes() );
        Byte newData30[] = { 0xab, 0xcd };
        // Since the new bytes are uninitialized, only verify the original data up to the last
        // size (the 3rd byte is lost because it is not part of the data)
        ASSERT_TRUE( ArraysMatch( newData30, buf.GetDataPtr(), BFDP_COUNT_OF_ARRAY( newData30 ) ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // For sanity in cases below, initialize the new byte
        buf.GetDataPtr()[2] = 0x88;
        buf.GetDataPtr()[3] = 0x99;

        // Resize again, initializing the new memory
        ASSERT_TRUE( buf.ResizePreserve( 36, 0x12 ) );
        ASSERT_EQ( 5U, buf.GetCapacityBytes() );
        ASSERT_EQ( 36U, buf.GetDataBits() );
        ASSERT_EQ( 5U, buf.GetDataBytes() );
        Byte newData36[] = { 0xab, 0xcd, 0x88, 0x99, 0x12 };
        ASSERT_TRUE( ArraysMatch( newData36, buf.GetDataPtr(), 5 ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize again, initializing the new memory to another value
        ASSERT_TRUE( buf.ResizePreserve( 50, 0x34 ) );
        ASSERT_EQ( 7U, buf.GetCapacityBytes() );
        ASSERT_EQ( 50U, buf.GetDataBits() );
        ASSERT_EQ( 7U, buf.GetDataBytes() );
        Byte newData42[] = { 0xab, 0xcd, 0x88, 0x99, 0x12, 0x34, 0x34 };
        ASSERT_TRUE( ArraysMatch( newData42, buf.GetDataPtr(), 7 ) );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, SetBufferDataSizeBits )
    {
        static Byte const initData[] = { 0xab, 0xcd, 0xef };
        static size_t const initSizeBytes = BFDP_COUNT_OF_ARRAY( initData );
        static size_t const initSizeBits = BitManip::BytesToBits( initSizeBytes );

        BitManip::BitBuffer buf( initData, initSizeBits );
        Byte const* initPtr = buf.GetDataPtr();

        // Set size smaller (bits)
        ASSERT_TRUE( buf.SetDataBits( 10 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );
        ASSERT_EQ( 10U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Set size to zero (bits)
        ASSERT_TRUE( buf.SetDataBits( 0 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Set size larger, within capacity (bits)
        ASSERT_TRUE( buf.SetDataBits( 15 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( 15U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize larger, beyond capacity by 1 bit (fails)
        ASSERT_FALSE( buf.SetDataBits( 25 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( 15U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, SetBufferDataSizeBytes )
    {
        static Byte const initData[] = { 0xab, 0xcd, 0xef };
        static size_t const initSizeBytes = BFDP_COUNT_OF_ARRAY( initData );
        static size_t const initSizeBits = BitManip::BytesToBits( initSizeBytes );

        BitManip::BitBuffer buf( initData, initSizeBits );
        Byte const* initPtr = buf.GetDataPtr();

        // Set size smaller (bytes)
        ASSERT_TRUE( buf.SetDataBytes( 2 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );
        ASSERT_EQ( 16U, buf.GetDataBits() );
        ASSERT_EQ( 2U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Set size to zero (bytes)
        ASSERT_TRUE( buf.SetDataBytes( 0 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );
        ASSERT_EQ( 0U, buf.GetDataBits() );
        ASSERT_EQ( 0U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Set size larger, within capacity (bytes)
        ASSERT_TRUE( buf.SetDataBytes( 3 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( 24U, buf.GetDataBits() );
        ASSERT_EQ( 3U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );

        // Resize larger, beyond capacity by 1 byte (fails)
        ASSERT_FALSE( buf.SetDataBytes( 4 ) );
        ASSERT_EQ( initSizeBytes, buf.GetCapacityBytes() );
        ASSERT_EQ( initSizeBits, buf.GetCapacityBits() );
        ASSERT_EQ( 24U, buf.GetDataBits() );
        ASSERT_EQ( 3U, buf.GetDataBytes() );
        ASSERT_TRUE( ArraysMatch( initData, buf.GetDataPtr(), initSizeBytes ) );
        ASSERT_EQ( initPtr, buf.GetDataPtr() );

        ASSERT_TRUE( VerifyGuarantees( buf ) );
    }

    TEST_F( BitManipBufferTest, WriteToBuffer )
    {
        static Byte const data[] = { 0x65, 0x66, 0x67 };
        static size_t const dataSizeBytes = BFDP_COUNT_OF_ARRAY( data );
        static size_t const dataSizeBits = BitManip::BytesToBits( BFDP_COUNT_OF_ARRAY( data ) );
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

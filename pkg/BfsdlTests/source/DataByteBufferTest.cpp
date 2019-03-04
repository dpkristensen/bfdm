/**
    BFDP Data ByteBuffer Test

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

#include "gtest/gtest.h"

#include "Bfdp/Data/ByteBuffer.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using Bfdp::Data::ByteBuffer;

    namespace DataByteBufferTestInternal
    {
        static Byte* const sNullPtr = NULL;
        static Byte const* const sConstNullPtr = NULL;
        static Byte const sTestData[] = { 0x31U, 0x32U, 0x33U };
        static size_t const sTestDataSz = BFDP_COUNT_OF_ARRAY( sTestData );
    }

    using namespace DataByteBufferTestInternal;

    class DataByteBufferTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( DataByteBufferTest, CreateEmpty )
    {
        ByteBuffer buffer;

        ASSERT_EQ( sNullPtr, buffer.GetPtr() );
        ASSERT_EQ( sConstNullPtr, buffer.GetConstPtr() );
        ASSERT_EQ( 0U, buffer.GetSize() );

        // Memory operations do nothing
        buffer.Clear();
        buffer.MemSet( 1U );
        ASSERT_EQ( 0U, buffer.CopyFrom( sTestData, sTestDataSz ) );

        ASSERT_EQ( sNullPtr, buffer.GetPtr() );
        ASSERT_EQ( sConstNullPtr, buffer.GetConstPtr() );
        ASSERT_EQ( 0U, buffer.GetSize() );
        ASSERT_STREQ( "", buffer.GetString().c_str() );
        ASSERT_STREQ( "", buffer.GetString( 1U ).c_str() );
    }

    TEST_F( DataByteBufferTest, CreateFailure )
    {
        ByteBuffer buffer;

        ASSERT_FALSE( buffer.Allocate( SIZE_MAX ) );

        ASSERT_EQ( sNullPtr, buffer.GetPtr() );
        ASSERT_EQ( sConstNullPtr, buffer.GetConstPtr() );
        ASSERT_EQ( 0U, buffer.GetSize() );
        ASSERT_STREQ( "", buffer.GetString().c_str() );
        ASSERT_STREQ( "", buffer.GetString( 1U ).c_str() );
    }

    TEST_F( DataByteBufferTest, CreateSuccess )
    {
        ByteBuffer buffer;
        ByteBuffer const& constBuffer = buffer;
        size_t const allocSize = 5U;

        ASSERT_TRUE( buffer.Allocate( allocSize ) );

        ASSERT_NE( sNullPtr, buffer.GetPtr() );
        ASSERT_NE( sConstNullPtr, buffer.GetConstPtr() );
        ASSERT_EQ( allocSize, buffer.GetSize() );

        buffer.Clear();
        for( size_t i = 0; i < allocSize; ++i )
        {
            ASSERT_EQ( 0U, buffer[i] );
            ASSERT_EQ( 0U, constBuffer[i] );
        }

        buffer.MemSet( 0x39U );
        for( size_t i = 0; i < allocSize; ++i )
        {
            ASSERT_EQ( 0x39U, buffer[i] );
            ASSERT_EQ( 0x39U, constBuffer[i] );
        }

        ASSERT_EQ( sTestDataSz, buffer.CopyFrom( sTestData, sTestDataSz ) );
        for( size_t i = 0; i < sTestDataSz; ++i )
        {
            ASSERT_EQ( sTestData[i], buffer[i] );
            ASSERT_EQ( sTestData[i], constBuffer[i] );
        }

        ASSERT_STREQ( "12399", buffer.GetString().c_str() );
        ASSERT_STREQ( "1239", buffer.GetString( 4U ).c_str() );
        ASSERT_STREQ( "12399", buffer.GetString( allocSize * 2 ).c_str() );
    }

    TEST_F( DataByteBufferTest, CreateShort )
    {
        ByteBuffer buffer;
        size_t const allocSize = 2U;

        ASSERT_TRUE( buffer.Allocate( allocSize ) );

        ASSERT_NE( sNullPtr, buffer.GetPtr() );
        ASSERT_NE( sConstNullPtr, buffer.GetConstPtr() );
        ASSERT_EQ( allocSize, buffer.GetSize() );

        buffer.MemSet( 0x39 );
        ASSERT_EQ( allocSize, buffer.CopyFrom( sTestData, sTestDataSz ) );
        for( size_t i = 0; i < allocSize; ++i )
        {
            ASSERT_EQ( sTestData[i], buffer[i] );
        }

        ASSERT_STREQ( "12", buffer.GetString().c_str() );
        ASSERT_STREQ( "1", buffer.GetString( 1U ).c_str() );
        ASSERT_STREQ( "12", buffer.GetString( allocSize * 2 ).c_str() );
    }

} // namespace BfsdlTests

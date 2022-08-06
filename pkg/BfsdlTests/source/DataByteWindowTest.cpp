/**
    BFDP Data ByteWindow Test

    Copyright 2022, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#include "Bfdp/Data/ByteWindow.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using Bfdp::Data::ByteWindow;

    class DataByteWindowTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( DataByteWindowTest, CreateEmpty )
    {
        ByteWindow window;

        // Check initial state
        ASSERT_EQ( 0U, window.GetBeginCounter() );
        ASSERT_EQ( 0U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 0U, window.GetRawBuffer().GetSize() );

        // Test out of bounds element access
        ASSERT_EQ( 0U, window.GetByte(0) );
        ASSERT_EQ( 0U, window.GetByte(30) );
    }

    TEST_F( DataByteWindowTest, InitNonZero )
    {
        ByteWindow window;

        // Check Init to non-zero size works
        ASSERT_TRUE( window.Init( 3U ) );

        // Check initial state
        ASSERT_EQ( 0U, window.GetBeginCounter() );
        ASSERT_EQ( 0U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 3U, window.GetRawBuffer().GetSize() );

        // Test element access
        ASSERT_EQ( 0U, window.GetByte(0) );
        ASSERT_EQ( 0U, window.GetByte(1) );
        ASSERT_EQ( 0U, window.GetByte(2) );
        ASSERT_EQ( 0U, window.GetByte(3) );

        // Put some data in:
        // [1 2]
        Byte const testData12[] = { 0x01, 0x02 };
        window.PushData( testData12, BFDP_COUNT_OF_ARRAY( testData12 ) );
        ASSERT_EQ( 0U, window.GetBeginCounter() ); // Window begin hasn't moved yet
        ASSERT_EQ( 2U, window.GetEndCounter() ); // Window end moved
        ASSERT_EQ( 2U, window.GetSize() );
        ASSERT_EQ( 0x01, window.GetByte(0) );
        ASSERT_EQ( 0x02, window.GetByte(1) );
        ASSERT_EQ( 0x00, window.GetByte(2) );
        ASSERT_EQ( 0x00, window.GetByte(3) );
        ASSERT_EQ( 0x01, window.GetRawBuffer()[0] );
        ASSERT_EQ( 0x02, window.GetRawBuffer()[1] );
        // Can't check raw[2]; state of memory is uninitialized

        // Put some more data in
        // 1 [2 3 4]
        Byte const testData34[] = { 0x03, 0x04 };
        window.PushData( testData34, BFDP_COUNT_OF_ARRAY( testData34 ) );
        ASSERT_EQ( 1U, window.GetBeginCounter() ); // Window begin moved 1 byte
        ASSERT_EQ( 4U, window.GetEndCounter() ); // Window end moved 2 bytes
        ASSERT_EQ( 3U, window.GetSize() ); // Window is now full
        ASSERT_EQ( 0x02, window.GetByte(0) );
        ASSERT_EQ( 0x03, window.GetByte(1) );
        ASSERT_EQ( 0x04, window.GetByte(2) );
        ASSERT_EQ( 0x00, window.GetByte(3) ); // Out of bounds, not modified
        ASSERT_EQ( 0x04, window.GetRawBuffer()[0] );
        ASSERT_EQ( 0x02, window.GetRawBuffer()[1] );
        ASSERT_EQ( 0x03, window.GetRawBuffer()[2] );

        // Put some more data in
        // 1 2 [3 4 5]
        Byte const testData5[] = { 0x05 };
        window.PushData( testData5, BFDP_COUNT_OF_ARRAY( testData5 ) );
        ASSERT_EQ( 2U, window.GetBeginCounter() ); // Window begin moved 1 byte
        ASSERT_EQ( 5U, window.GetEndCounter() ); // Window end moved 2 bytes
        ASSERT_EQ( 3U, window.GetSize() ); // Window is now full
        ASSERT_EQ( 0x03, window.GetByte(0) );
        ASSERT_EQ( 0x04, window.GetByte(1) );
        ASSERT_EQ( 0x05, window.GetByte(2) );
        ASSERT_EQ( 0x00, window.GetByte(3) ); // Out of bounds, not modified
        ASSERT_EQ( 0x04, window.GetRawBuffer()[0] );
        ASSERT_EQ( 0x05, window.GetRawBuffer()[1] );
        ASSERT_EQ( 0x03, window.GetRawBuffer()[2] );

        // Put no data in
        // 1 2 [3 4 5]
        window.PushData( NULL, 0U );
        ASSERT_EQ( 2U, window.GetBeginCounter() ); // Window begin moved 0 bytes
        ASSERT_EQ( 5U, window.GetEndCounter() ); // Window end moved 0 bytes
        ASSERT_EQ( 3U, window.GetSize() ); // Size unchanged
        ASSERT_EQ( 0x03, window.GetByte(0) ); // Data unchanged
        ASSERT_EQ( 0x04, window.GetByte(1) );
        ASSERT_EQ( 0x05, window.GetByte(2) );
        ASSERT_EQ( 0x00, window.GetByte(3) ); // Out of bounds, not modified
        ASSERT_EQ( 0x04, window.GetRawBuffer()[0] );
        ASSERT_EQ( 0x05, window.GetRawBuffer()[1] );
        ASSERT_EQ( 0x03, window.GetRawBuffer()[2] );

        // Put way too much data in
        // 1 2 3 4 5 12 34 56 78 90 [ab cd ef]
        Byte const testDataTooManyThings[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef };
        window.PushData( testDataTooManyThings, BFDP_COUNT_OF_ARRAY( testDataTooManyThings ) );
        ASSERT_EQ( 10U, window.GetBeginCounter() ); // Window begin moved 8 bytes
        ASSERT_EQ( 13U, window.GetEndCounter() ); // Window end moved 8 bytes
        ASSERT_EQ( 3U, window.GetSize() ); // Size unchanged
        ASSERT_EQ( 0xab, window.GetByte(0) ); // Data is last 3 bytes added
        ASSERT_EQ( 0xcd, window.GetByte(1) );
        ASSERT_EQ( 0xef, window.GetByte(2) );
        ASSERT_EQ( 0x00, window.GetByte(3) ); // Out of bounds, not modified
        // Cannot make any assumptions about the underlying buffer data - implementation detail

        // Reset to initial state
        window.Reset();
        ASSERT_EQ( 0U, window.GetBeginCounter() );
        ASSERT_EQ( 0U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 0U, window.GetByte(0) ); // Data elements now out of range
        ASSERT_EQ( 0U, window.GetByte(1) );
        ASSERT_EQ( 0U, window.GetByte(2) );
    }

    TEST_F( DataByteWindowTest, InitZero )
    {
        ByteWindow window;

        // Check Init to zero size works
        ASSERT_TRUE( window.Init( 0U ) );

        // Check initial state
        ASSERT_EQ( 0U, window.GetBeginCounter() );
        ASSERT_EQ( 0U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 0U, window.GetRawBuffer().GetSize() );

        // Test out of bounds element access
        ASSERT_EQ( 0U, window.GetByte(0) );
        ASSERT_EQ( 0U, window.GetByte(30) );

        // Put some data in
        Byte const testData12[] = { 0x01, 0x02 };
        window.PushData( testData12, BFDP_COUNT_OF_ARRAY( testData12 ) );
        ASSERT_EQ( 2U, window.GetBeginCounter() ); // Window is empty, so both counters moved
        ASSERT_EQ( 2U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 0U, window.GetByte(0) );
        ASSERT_EQ( 0U, window.GetByte(1) );

        // Reset to initial state
        window.Reset();
        ASSERT_EQ( 0U, window.GetBeginCounter() );
        ASSERT_EQ( 0U, window.GetEndCounter() );
        ASSERT_EQ( 0U, window.GetSize() );
        ASSERT_EQ( 0U, window.GetByte(0) );
        ASSERT_EQ( 0U, window.GetByte(1) );
    }

} // namespace BfsdlTests

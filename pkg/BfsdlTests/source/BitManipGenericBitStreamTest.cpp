/**
    BFDP BitManip GenericBitStream Test

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

// External includes
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class BitManipGenericBitStreamTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        BitManip::BitBuffer mBuffer;
    };

    TEST_F( BitManipGenericBitStreamTest, CreateEmptyStream )
    {
        BitManip::GenericBitStream stream( mBuffer );

        Byte value;
        ASSERT_FALSE( stream.ReadBits( &value, 1 ) );
        ASSERT_FALSE( stream.ReadValue( value ) );
    }

    TEST_F( BitManipGenericBitStreamTest, ReadWrite )
    {
        static SizeT const NumBits = 10;

        struct RwTestElement
        {
            Byte value;
            SizeT size;
            bool result;
        };

        // Allocate the buffer and fill with garbage (for debugging)
        ASSERT_TRUE( mBuffer.ResizeNoPreserve( NumBits ) );
        mBuffer.MemSet( 0xCC );

        BitManip::GenericBitStream stream( mBuffer );
        RwTestElement const inputData[] =
        {
            { 0x5, 3U, true },
            { 0xa, 4U, true },
            { 0x0, 2U, true },
            { 0x1, 1U, true },
            { 0x1, 1U, false }
        };
        SizeT const testCount = BFDP_COUNT_OF_ARRAY( inputData );

        // Write some data elements as const input
        for( SizeT i = 0u; i < testCount; ++i )
        {
            RwTestElement const& e = inputData[i];
            SCOPED_TRACE( ::testing::Message( "i=" ) << i );
            ASSERT_EQ( e.result, stream.WriteBits( &e.value, e.size ) );
        }
        ASSERT_EQ( NumBits, stream.GetPosBits() );

        stream.SeekBits( 0u );
        ASSERT_EQ( 0u, stream.GetPosBits() );

        // Create a buffer for output and clear it.
        Byte outputData[testCount];
        std::memset( outputData, 0u, testCount );

        // Read them back as non-const output
        for( SizeT i = 0u; i < testCount; ++i )
        {
            RwTestElement const& e = inputData[i];
            SCOPED_TRACE( ::testing::Message( "i=" ) << i );
            ASSERT_EQ( e.result, stream.ReadBits( &outputData[i], e.size ) );
            if( e.result )
            {
                ASSERT_EQ( e.value, outputData[i] );
            }
        }
        ASSERT_EQ( NumBits, stream.GetPosBits() );
    }

} // namespace BfsdlTests

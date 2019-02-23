/**
    BFDP Unicode Iterator Tests

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
#include <string>
#include "gtest/gtest.h"

// Internal includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/Iterator.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class UnicodeIteratorTest
        : public ::testing::Test
    {
    protected:
        typedef Unicode::StrIterator< Unicode::Utf8Converter > Utf8TestIterator;
        typedef Unicode::StrIterator< Unicode::AsciiConverter > AsciiTestIterator;

        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    static Byte const* const NULL_BYTE_CONST_PTR = (Byte const*)0;

    TEST_F( UnicodeIteratorTest, EmptyAsciiIterator )
    {
        AsciiTestIterator emptyIter = AsciiTestIterator( std::string() );

        ASSERT_FALSE( emptyIter );
        ASSERT_TRUE( !emptyIter );
        ASSERT_FALSE( emptyIter.HasError() );
        ASSERT_EQ( 0, emptyIter.GetIndex() );
        // Cannot place expectation on address of empty buffer
    }

    TEST_F( UnicodeIteratorTest, EmptyUtf8Iterator )
    {
        Utf8TestIterator emptyIter = Utf8TestIterator( std::string() );

        ASSERT_FALSE( emptyIter );
        ASSERT_TRUE( !emptyIter );
        ASSERT_FALSE( emptyIter.HasError() );
        ASSERT_EQ( 0, emptyIter.GetIndex() );
        // Cannot place expectation on address of empty buffer
}

    TEST_F( UnicodeIteratorTest, ValidAsciiIteratorPostIncrement )
    {
        static Unicode::CodePoint data[] = { 0x31, 0x32, 0x33, 0x00, 0x61, 0x62, 0x63, 0x7f };
        static SizeT numValues = BFDP_COUNT_OF_ARRAY( data );
        AsciiTestIterator iter = AsciiTestIterator( std::string( "123\0abc\x7f", numValues ) );

        ASSERT_TRUE( iter );
        ASSERT_FALSE( !iter );
        ASSERT_FALSE( iter.HasError() );
        ASSERT_EQ( 0, iter.GetIndex() );
        ASSERT_NE( NULL_BYTE_CONST_PTR, &iter );

        Unicode::CodePoint cp = Unicode::InvalidCodePoint;
        while( iter )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << iter.GetIndex() );

            ASSERT_LT( iter.GetIndex(), numValues );
            cp = *iter;
            ASSERT_EQ( data[iter.GetIndex()], cp );
            ASSERT_FALSE( iter.HasError() );
            ASSERT_EQ( cp, iter++ );
            ASSERT_FALSE( iter.HasError() );
        }

        ASSERT_EQ( numValues, iter.GetIndex() );
    }

    TEST_F( UnicodeIteratorTest, ValidAsciiIteratorPreIncrement )
    {
        static Unicode::CodePoint data[] = { 0x7f, 0x20, 0x39 };
        static SizeT numValues = BFDP_COUNT_OF_ARRAY( data );
        AsciiTestIterator iter = AsciiTestIterator( "\x7f 9" );

        while( iter )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << &iter );

            SizeT i = iter.GetIndex();
            ASSERT_LT( i, numValues );
            ASSERT_EQ( data[i], *iter );
            ASSERT_FALSE( iter.HasError() );
            Unicode::CodePoint nextCp = ( ( i + 1 ) < numValues )
                ? data[i + 1]
                : Unicode::InvalidCodePoint; // Increment past end
            ASSERT_EQ( nextCp, ++iter );
            ASSERT_FALSE( iter.HasError() );
        }

        ASSERT_EQ( numValues, iter.GetIndex() );
    }

    TEST_F( UnicodeIteratorTest, ValidUtf8Iterator )
    {
        Utf8TestIterator emptyIter = Utf8TestIterator( std::string() );

        ASSERT_FALSE( emptyIter );
        ASSERT_TRUE( !emptyIter );
        ASSERT_FALSE( emptyIter.HasError() );
    }

} // namespace BfsdlTests

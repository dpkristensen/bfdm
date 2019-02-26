/**
    BFDP Lexer Symbol Buffer Test

    Copyright 2016-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#include <gtest/gtest.h>

#include "Bfdp/Lexer/StaticSymbolBuffer.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class SymbolBufferTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( SymbolBufferTest, Static )
    {
        Lexer::StaticSymbolBuffer< 2 > buffer;

        // Initial state
        ASSERT_EQ( 0U, buffer.GetSize() );

        // Adding values up to boundary
        ASSERT_TRUE( buffer.Add( 100U ) );
        ASSERT_TRUE( buffer.Add( 200U ) );
        ASSERT_FALSE( buffer.Add( 300U ) );

        // Modified state
        ASSERT_EQ( 2U, buffer.GetSize() );
        ASSERT_EQ( 100U, buffer.GetSymbolAt( 0 ) );
        ASSERT_EQ( 200U, buffer.GetSymbolAt( 1 ) );

        // Reset state
        buffer.Clear();
        ASSERT_EQ( 0U, buffer.GetSize() );

        // Re-modified state
        ASSERT_TRUE( buffer.Add( 0x1234 ) );
        ASSERT_TRUE( buffer.Add( 0x5678 ) );
        ASSERT_EQ( 2U, buffer.GetSize() );
        ASSERT_EQ( 0x5678, buffer.GetSymbolAt( 1 ) );
        ASSERT_EQ( 0x1234, buffer.GetSymbolAt( 0 ) );
    }

    TEST_F( SymbolBufferTest, StaticWithDifferentSize )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer;

        // Test boundary condition with a different size
        ASSERT_TRUE( buffer.Add( 100U ) );
        ASSERT_TRUE( buffer.Add( 200U ) );
        ASSERT_TRUE( buffer.Add( 300U ) );
        ASSERT_TRUE( buffer.Add( 400U ) );
        ASSERT_TRUE( buffer.Add( 500U ) );
        ASSERT_FALSE( buffer.Add( 600U ) );

        // Resulting state
        ASSERT_EQ( 5U, buffer.GetSize() );
        ASSERT_EQ( 100U, buffer.GetSymbolAt( 0 ) );
        ASSERT_EQ( 200U, buffer.GetSymbolAt( 1 ) );
        ASSERT_EQ( 300U, buffer.GetSymbolAt( 2 ) );
        ASSERT_EQ( 400U, buffer.GetSymbolAt( 3 ) );
        ASSERT_EQ( 500U, buffer.GetSymbolAt( 4 ) );
    }

} // namespace BfsdlTests

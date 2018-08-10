/**
    BFDP Lexer Symbol Category Test

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

#include <gtest/gtest.h>

#include "Bfdp/Lexer/RangeSymbolCategory.hpp"
#include "Bfdp/Lexer/StringSymbolCategory.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class SymbolCategoryTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( SymbolCategoryTest, Range1 )
    {
        Lexer::RangeSymbolCategory category( 7, 99, true );

        ASSERT_EQ( 7, category.GetCategory() );
        ASSERT_FALSE( category.Contains( 0 ) );
        ASSERT_FALSE( category.Contains( 98 ) );
        ASSERT_TRUE( category.Contains( 99 ) );
        ASSERT_FALSE( category.Contains( 100 ) );
        ASSERT_FALSE( category.Contains( Unicode::InvalidCodePoint ) );
        ASSERT_TRUE( category.ShouldConcatenate() );

        Lexer::RangeSymbolCategory categoryConcatenateFalse( 7, 99, false );
        ASSERT_FALSE( categoryConcatenateFalse.ShouldConcatenate() );
    }

    TEST_F( SymbolCategoryTest, Range2 )
    {
        Lexer::RangeSymbolCategory category( 42, 65, 90, true );

        ASSERT_EQ( 42, category.GetCategory() );
        ASSERT_FALSE( category.Contains( 0 ) );
        ASSERT_FALSE( category.Contains( 64 ) );
        ASSERT_TRUE( category.Contains( 65 ) );
        ASSERT_TRUE( category.Contains( 66 ) );
        ASSERT_TRUE( category.Contains( 89 ) );
        ASSERT_TRUE( category.Contains( 90 ) );
        ASSERT_FALSE( category.Contains( 91 ) );
        ASSERT_FALSE( category.Contains( Unicode::InvalidCodePoint ) );
        ASSERT_TRUE( category.ShouldConcatenate() );

        Lexer::RangeSymbolCategory categoryConcatenateFalse( 42, 65, 90, false );
        ASSERT_FALSE( categoryConcatenateFalse.ShouldConcatenate() );
    }

    TEST_F( SymbolCategoryTest, String )
    {
        Lexer::StringSymbolCategory category( 42, "ab \x86", true );

        ASSERT_EQ( 42, category.GetCategory() );
        ASSERT_FALSE( category.Contains( 0 ) ); // NULL
        ASSERT_FALSE( category.Contains( 65 ) ); // A
        ASSERT_FALSE( category.Contains( 96 ) ); // `
        ASSERT_TRUE( category.Contains( 97 ) ); // a
        ASSERT_TRUE( category.Contains( 98 ) ); // b
        ASSERT_FALSE( category.Contains( 99 ) ); // c
        ASSERT_FALSE( category.Contains( 0x86 ) ); // CP1256 value
        ASSERT_FALSE( category.Contains( 0x186 ) ); // LATIN CAPITAL LETTER AE
        ASSERT_FALSE( category.Contains( 8224 ) ); // LATIN SMALL LETTER A WITH RING ABOVE
        ASSERT_TRUE( category.ShouldConcatenate() );

        Lexer::StringSymbolCategory categoryConcatenateFalse( 42, "ab \x86", false );
        ASSERT_FALSE( categoryConcatenateFalse.ShouldConcatenate() );
    }

} // namespace BfsdlTests

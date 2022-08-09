/**
    BFDP Lexer Test

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

// External includes
#include <gtest/gtest.h>
#include <string>

// Internal includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Lexer/StaticSymbolBuffer.hpp"
#include "Bfdp/Lexer/StringSymbolCategory.hpp"
#include "Bfdp/Lexer/Symbolizer.hpp"
#include "Bfdp/Unicode/CodingMap.hpp"
#include "BfsdlTests/MockSymbolObserver.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using Bfdp::Unicode::GetCodec;

    class LexerTest
        : public ::testing::Test
    {
    protected:
        void ReadInChunks
            (
            Lexer::Symbolizer& aLexer,
            Byte const * aBuffer,
            size_t const aChunkSize[],
            size_t const aNumChunks
            )
        {
            Byte const * buf = aBuffer;
            for( size_t i = 0; i < aNumChunks; ++i )
            {
                size_t bytesRead = 0;
                SCOPED_TRACE( ::testing::Message( "i = " ) << i );
                ASSERT_TRUE( aLexer.Parse( buf, aChunkSize[i], bytesRead ) );
                ASSERT_EQ( aChunkSize[i], bytesRead );
                buf += aChunkSize[i];
            }
        }

        void SetUp()
        {
            SetDefaultErrorHandlers();

            mCodingIdMs1252 = Unicode::GetCodingId( "MS-1252" );
            mCodingIdUtf8 = Unicode::GetCodingId( "UTF8" );
        }

        Unicode::CodingId mCodingIdMs1252;
        Unicode::CodingId mCodingIdUtf8;
    };

    TEST_F( LexerTest, NoCategories )
    {
        Lexer::StaticSymbolBuffer< 3 > buffer;
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        static Byte const testBytes[] = "abc123xyz"; // implicit NULL at end

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( testBytes, BFDP_COUNT_OF_ARRAY( testBytes ), bytesRead ) );
        ASSERT_EQ( 10U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: abc/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: 123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNext( std::string( "Unmapped: ") + '\0' + "/1" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, OneByte )
    {
        Lexer::StaticSymbolBuffer< 3 > buffer;
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( Char( "X" ), 1, bytesRead ) );
        ASSERT_EQ( 1U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: X/1" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, OneSymbol )
    {
        Lexer::StaticSymbolBuffer< 3 > buffer;
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( Char( "XXX" ), 3, bytesRead ) );
        ASSERT_EQ( 3U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: XXX/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, OneCategory )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer;
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        // Add the category; byte order in category does not matter
        Lexer::StringSymbolCategory category5( 5, std::string( "A" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category5 ) );

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( Char( "AAAAA" ), 5, bytesRead ) );
        ASSERT_EQ( 5U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Mapped: 5/AAAAA/5" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, Utf8Output )
    {
        Lexer::StaticSymbolBuffer< 1 > buffer;
        MockSymbolObserver observer;

        // Input is in Code Page 1252
        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        std::string const LEFT_SINGLE_QUOTATION_MARK = "\xE2\x80\x98"; // In UTF-8

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( Char( "\x41\x91\x42" ), 3, bytesRead ) );
        ASSERT_EQ( 3U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: A/1" ) );
        ASSERT_TRUE( observer.VerifyNext( std::string( "Unmapped: " ) + LEFT_SINGLE_QUOTATION_MARK + "/1" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: B/1" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, Utf8Input )
    {
        Lexer::StaticSymbolBuffer< 1 > buffer;
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdUtf8 ) );

        std::string const NERD_FACE = "\xF0\x9F\xA4\x93"; // Unicode 0x1f913 in UTF-8

        size_t bytesRead = 0U;
        ASSERT_TRUE( lexer.Parse( Char( "\x43\xF0\x9F\xA4\x93\x44" ), 6, bytesRead ) );
        ASSERT_EQ( 6U, bytesRead );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: C/1" ) );
        ASSERT_TRUE( observer.VerifyNext( std::string( "Unmapped: " ) + NERD_FACE + "/1" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: D/1" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, MultipleBuffersUncategorized )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer; // Larger than the chunks being read
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        static Byte const * testBytes = Char( "abc123xyz" );
        size_t chunks[] = { 3, 3, 3 };

        ASSERT_NO_FATAL_FAILURE
            (
            ReadInChunks( lexer, testBytes, chunks, BFDP_COUNT_OF_ARRAY( chunks ) )
            );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: abc/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: 123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, MultipleBuffersWithOneCategory )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer; // Larger than the chunks being read
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        // Add the category; byte order in category does not matter
        Lexer::StringSymbolCategory category42( 42, std::string( "321" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category42 ) );

        static Byte const * testBytes = Char( "abc123xyz" );
        size_t chunks[] = { 3, 3, 3 };

        ASSERT_NO_FATAL_FAILURE
        (
            ReadInChunks( lexer, testBytes, chunks, BFDP_COUNT_OF_ARRAY( chunks ) )
        );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Unmapped: abc/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 42/123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, MultipleBuffersWithMultipleCategories )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer; // Larger than the chunks being read
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        // Add the categories; byte order in category does not matter
        Lexer::StringSymbolCategory category1( 1, std::string( "cba" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category1 ) );
        Lexer::StringSymbolCategory category2( 2, std::string( "312" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category2 ) );
        Lexer::StringSymbolCategory category3( 3, std::string( "xyz" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category3 ) );

        static Byte const * testBytes = Char( "abcqq123xyz" );
        size_t chunks[] = { 3, 2, 3, 3 };

        ASSERT_NO_FATAL_FAILURE
        (
            ReadInChunks( lexer, testBytes, chunks, BFDP_COUNT_OF_ARRAY( chunks ) )
        );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Mapped: 1/abc/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: qq/2" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 2/123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 3/xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, IntermediateUncategorizedData )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer; // Larger than the chunks being read
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        // Add the category; byte order in category does not matter
        Lexer::StringSymbolCategory category1( 1, std::string( "abc" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category1 ) );
        Lexer::StringSymbolCategory category2( 2, std::string( "xyz" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category2 ) );

        static Byte const * testBytes = Char( "abc123xyz" );
        size_t chunks[] = { 3, 3, 3 };

        ASSERT_NO_FATAL_FAILURE
        (
            ReadInChunks( lexer, testBytes, chunks, BFDP_COUNT_OF_ARRAY( chunks ) )
        );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Mapped: 1/abc/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: 123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 2/xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( LexerTest, UnconcatenatedCategory )
    {
        Lexer::StaticSymbolBuffer< 5 > buffer; // Larger than the chunks being read
        MockSymbolObserver observer;

        Lexer::Symbolizer lexer( observer, buffer, GetCodec( mCodingIdMs1252 ) );

        // Add the category; byte order in category does not matter
        Lexer::StringSymbolCategory category1( 1, std::string( "abc" ), false );
        ASSERT_TRUE( lexer.AddCategory( &category1 ) );
        Lexer::StringSymbolCategory category2( 2, std::string( "xyz" ), true );
        ASSERT_TRUE( lexer.AddCategory( &category2 ) );

        static Byte const * testBytes = Char( "abc123xyz" );
        size_t chunks[] = { 3, 3, 3 };

        ASSERT_NO_FATAL_FAILURE
        (
            ReadInChunks( lexer, testBytes, chunks, BFDP_COUNT_OF_ARRAY( chunks ) )
        );
        lexer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Mapped: 1/a/1" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 1/b/1" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 1/c/1" ) );
        ASSERT_TRUE( observer.VerifyNext( "Unmapped: 123/3" ) );
        ASSERT_TRUE( observer.VerifyNext( "Mapped: 2/xyz/3" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

} // namespace BfsdlTests

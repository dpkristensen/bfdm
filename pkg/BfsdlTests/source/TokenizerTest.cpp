/**
    BFSDL Parser Tokenizer Tests

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

#include <cstring>

#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Objects/NumericLiteral.hpp"
#include "BfsdlParser/Token/Tokenizer.hpp"
#include "BfsdlTests/MockTokenObserver.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using namespace BfsdlParser;

    class TokenizerTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( TokenizerTest, AsciiTest )
    {
        MockTokenObserver observer;
        Token::Tokenizer tokenizer( observer );
        ASSERT_TRUE( tokenizer.IsInitOk() );
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;


        // Test non-ASCII characters are treated as an error
        char const * testData = "\x80";
        size_t bytesRead = 0;
        size_t dataLen = std::strlen( testData );

        errWorkspace.ExpectRunTimeError();
        ASSERT_FALSE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData ), dataLen, bytesRead ) );
        errWorkspace.VerifyRunTimeError();

        ASSERT_EQ( 0U, bytesRead );
        tokenizer.EndParsing();
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( TokenizerTest, WhitespaceIgnored )
    {
        MockTokenObserver observer;
        Token::Tokenizer tokenizer( observer );
        ASSERT_TRUE( tokenizer.IsInitOk() );
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        char const * testData = "  \n \r \t\r\n";

        // No tokens are observed

        size_t bytesRead = 0;
        size_t dataLen = std::strlen( testData );
        ASSERT_TRUE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData ), dataLen, bytesRead ) );
        ASSERT_EQ( dataLen, bytesRead );
        tokenizer.EndParsing();
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( TokenizerTest, Comments )
    {
        MockTokenObserver observer;
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        struct TestData
        {
            char const* input;
            bool parseOk;
            char const* output;
        };

        // Test condition table
        static TestData const testData[] =
        { //  Input             Parse OK    Output
            { "foo // baz",     true,       "foo" },
            { "//foo\nbar",     true,       "bar" },
            { "/ foo\nbar",     false,       NULL },
            { "/ /foo\nbar",    false,       NULL },
            { "*/foo\nbar",     false,       NULL },
            { "/*foo\nbar",     true,        NULL }, // Error during EndParsing()
            { "/*foo*/\nbar",   true,        "bar" },
            { "/*foo\n*/bar",   true,        "bar" },
            { "/*foo\n//*/bar", true,        "bar" },
            { "/*//foo\n*/bar", true,        "bar" },
            { "/*//foo\nbar*/ baz", true,    "baz" },
            { "/*\n//foo*/ baz", true,       "baz" },
        };
        static size_t const numTests = BFDP_COUNT_OF_ARRAY( testData );

        // Loop through test conditions
        for( size_t i = 0; i < numTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << testData[i].input << std::endl );

            Token::Tokenizer tokenizer( observer );
            ASSERT_TRUE( tokenizer.IsInitOk() );
            size_t bytesRead = 0;
            size_t dataLen = std::strlen( testData[i].input );

            // Set expectations for this iteration
            errWorkspace.ExpectRunTimeError( ( testData[i].output == NULL ) );

            // Parse data for this iteration
            ASSERT_EQ( testData[i].parseOk, tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
            tokenizer.EndParsing();

            // Verify postconditions
            errWorkspace.VerifyRunTimeError();
            if( testData[i].output )
            {
                ASSERT_TRUE( observer.VerifyNext( testData[i].output ) );
            }
            ASSERT_TRUE( observer.VerifyNone() );

            if( testData[i].output )
            {
                ASSERT_EQ( dataLen, bytesRead );
            }
        }
    }

    TEST_F( TokenizerTest, ControlCharacters )
    {
        MockTokenObserver observer;
        Token::Tokenizer tokenizer( observer );
        ASSERT_TRUE( tokenizer.IsInitOk() );
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Test control characters in pairs to ensure they are not concatenated
        char const * testData = "]]::[[;;";

        size_t bytesRead = 0;
        size_t dataLen = std::strlen( testData );
        ASSERT_TRUE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData ), dataLen, bytesRead ) );
        ASSERT_EQ( dataLen, bytesRead );
        tokenizer.EndParsing();

        ASSERT_TRUE( observer.VerifyNext( "Control: ]" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: ]" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: :" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: :" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: [" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: [" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: ;" ) );
        ASSERT_TRUE( observer.VerifyNext( "Control: ;" ) );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( TokenizerTest, LiteralWithSymbolizerError )
    {
        static char const* DATA = "#\xc2\xb9";
        size_t const DATA_LEN = std::strlen( DATA );

        MockTokenObserver observer;
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        Token::Tokenizer tokenizer( observer );
        ASSERT_TRUE( tokenizer.IsInitOk() );
        size_t bytesRead = 0;

        // Parsing will result in an error from the symbolizer, and leave the tokenizer in an unfinished state
        errWorkspace.ExpectRunTimeError();
        ASSERT_FALSE( tokenizer.Parse( reinterpret_cast< Byte const * >( DATA ), DATA_LEN, bytesRead ) );
        errWorkspace.VerifyRunTimeError();

        tokenizer.EndParsing();
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( TokenizerTest, NumericLiteral )
    {
        MockTokenObserver observer;
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        struct TestData
        {
            char const* input;
            bool parseOk;
            char const* output;
        };

        // Test condition table
        // NOTE: This does not need to be extremely verbose
        static TestData const testData[] =
        { //  Input             Parse OK    Output
            { "#",              true,       NULL }, // Parse() returns OK on incomplete literal
            { "##",             false,      NULL },
            { "#b:01111011#",   true,       "+01111011" },
            { "#d:123#",        true,       "+123" },
            { "#x:7b#",         true,       "+7b" },
            { "#o:8#",          false,      NULL },

            // Unterminated numeric literals
            { "#d:123",         true,       NULL }, // Error detected after Parse()
            { "#d:123 ",        true,       NULL }, // Error detected after Parse()
            { "#d:123\t",       true,       NULL }, // Error detected after Parse()
            { "#d:123;",        false,      NULL },
        };
        static size_t const numTests = BFDP_COUNT_OF_ARRAY( testData );

        // Loop through test conditions
        for( size_t i = 0; i < numTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << testData[i].input << std::endl );

            Token::Tokenizer tokenizer( observer );
            ASSERT_TRUE( tokenizer.IsInitOk() );
            size_t bytesRead = 0;
            size_t dataLen = std::strlen( testData[i].input );

            // Set expectations for this iteration
            errWorkspace.ExpectRunTimeError( ( testData[i].output == NULL ) );

            // Parse data for this iteration
            ASSERT_EQ( testData[i].parseOk, tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
            tokenizer.EndParsing();

            // Verify postconditions
            errWorkspace.VerifyRunTimeError();
            if( testData[i].output )
            {
                std::string value = std::string( "NumericLiteral: " ) + testData[i].output;
                ASSERT_TRUE( observer.VerifyNext( value ) );
            }
            ASSERT_TRUE( observer.VerifyNone() );

            if( testData[i].output )
            {
                ASSERT_EQ( dataLen, bytesRead );
            }
        }
    }

    TEST_F( TokenizerTest, StringLiteral )
    {
        MockTokenObserver observer;
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        struct TestData
        {
            char const* input;
            bool parseOk;
            char const* output;
        };

        // Test condition table
        // NOTE: This does not need to be extremely verbose.  Output is always in utf8
        static TestData const testData[] =
        { //  Input             Parse OK    Output
            { "\"",             true,       NULL }, // Parse() returns OK on incomplete literal
            { "\"\"",           true,       "" },
            { "\"abc123\"",     true,       "61 62 63 31 32 33" },
            // Hex escape sequences
            { "\"\\x62c3\"",    true,       "62 63 33" },
            { "\"\\0x62c3\"",   false,      NULL },
            { "\"\\1x62c3\"",   true,       "06 32 63 33" },
            { "\"\\4x62c3\"",   true,       "e6 8b 83" },
            { "\"\\8x12345678\"", true,     "fc 92 8d 85 99 b8" },
            { "\"\\9x123456780\"", false,   NULL },
            { "\"\\x80\"",      true,       "c2 80" },
            { "\"\\x00\"",      true,       "00" },
            { "\"\\x0g\"",      false,      NULL },
            { "\"\\x0z\"",      false,      NULL },
            { "\"\\x\"",        false,      NULL },
            { "\"\\x1\"",       false,      NULL },
            // Ascii escape sequences
            { "\"\\a41\"",      true,       "41" },
            { "\"\\a7f\"",      true,       "7f" },
            { "\"\\a80\"",      false,      NULL },
            { "\"\\0a7f\"",     false,      NULL },
            { "\"\\2a7f\"",     false,      NULL },
            { "\"\\a0g\"",      false,      NULL },
            { "\"\\a0z\"",      false,      NULL },
            { "\"\\a\"",        false,      NULL },
            { "\"\\a7\"",       false,      NULL },
            // MS-1252 escape sequences
            { "\"\\w41\"",      true,       "41" },
            { "\"\\w7e\"",      true,       "7e" },
            { "\"\\w7f\"",      false,      NULL },
            { "\"\\w80\"",      true,       "e2 82 ac" },
            { "\"\\w8c\"",      true,       "c5 92" },
            { "\"\\w8d\"",      false,      NULL },
            { "\"\\w8e\"",      true,       "c5 bd" },
            { "\"\\0w7f\"",     false,      NULL },
            { "\"\\2w7f\"",     false,      NULL },
            { "\"\\w0g\"",      false,      NULL },
            { "\"\\w0z\"",      false,      NULL },
            { "\"\\w\"",        false,      NULL },
            { "\"\\w7\"",       false,      NULL },
            // Unicode escape sequences
            { "\"\\u0000\"",        true,       "00" },
            { "\"\\u00b9\"",        true,       "c2 b9" },
            { "\"\\4u000b9\"",      true,       "0b 39" },
            { "\"\\5u000b9\"",      true,       "c2 b9" },
            { "\"\\5u1f913\"",      true,       "f0 9f a4 93" },
            { "\"\\8u7fffffff\"",   true,       "fd bf bf bf bf bf" },
            { "\"\\8u80000000\"",   false,      NULL },
            { "\"\\9u0000000b9\"",  false,      NULL },
            { "\"\\0u1234\"",       false,      NULL },
            { "\"\\u000g\"",        false,      NULL },
            { "\"\\u000z\"",        false,      NULL },
            { "\"\\u\"",            false,      NULL },
            { "\"\\u7\"",           false,      NULL },
            { "\"\\u123\"",         false,      NULL },
            // Other escape sequences
            { "\"\\\"\"",       true,       "22" },
            { "\"\\t\"",        true,       "08" },
            { "\"\\r\"",        true,       "0d" },
            { "\"\\n\"",        true,       "0a" },
            { "\"\\\\\"",       true,       "5c" },
            // Multiple escape sequences together
            { "\"\\\"\\t\\r\\n\\\\t\"", true, "22 08 0d 0a 5c 74" },
            // Unterminated string literal
            { "\"abc123 ",      true,       NULL }, // Error detected after Parse()
            { "\"abc123\t",     true,       NULL }, // Error detected after Parse()
            { "\"abc123;",      true,       NULL }, // Error detected after Parse()
        };
        static size_t const numTests = BFDP_COUNT_OF_ARRAY( testData );

        // Loop through test conditions
        for( size_t i = 0; i < numTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << testData[i].input << std::endl );

            Token::Tokenizer tokenizer( observer );
            ASSERT_TRUE( tokenizer.IsInitOk() );
            size_t bytesRead = 0;
            size_t dataLen = std::strlen( testData[i].input );

            // Set expectations for this iteration
            errWorkspace.ExpectRunTimeError( testData[i].output == NULL );

            // Parse data for this iteration
            ASSERT_EQ( testData[i].parseOk, tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
            tokenizer.EndParsing();

            // Verify postconditions
            errWorkspace.VerifyRunTimeError();
            if( testData[i].output )
            {
                std::string value = std::string( "StringLiteral: " ) + testData[i].output;
                ASSERT_TRUE( observer.VerifyNext( value ) );
            }
            ASSERT_TRUE( observer.VerifyNone() );

            if( testData[i].output )
            {
                ASSERT_EQ( dataLen, bytesRead );
            }
        }
    }

    TEST_F( TokenizerTest, WordTest )
    {
        MockTokenObserver observer;
        SetMockErrorHandlers();

        struct TestData
        {
            char const* input;
            char const* output[4];
        };

        // Test condition table
        static TestData const testData[] =
        { //  Input             Output (NULL termilnated list)
            { "foo",            { "foo", NULL } },
            { "fOo",            { "fOo", NULL } },
            { "foo bar",        { "foo", "bar", NULL } },
            { " bar baz ",      { "bar", "baz", NULL } },
            { "a_b cd1 ef_2_",  { "a_b", "cd1", "ef_2_", NULL } },
        };
        static size_t const numTests = BFDP_COUNT_OF_ARRAY( testData );

        // Loop through test conditions
        for( size_t i = 0; i < numTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << testData[i].input << std::endl );

            Token::Tokenizer tokenizer( observer );
            ASSERT_TRUE( tokenizer.IsInitOk() );
            size_t bytesRead = 0;
            size_t dataLen = std::strlen( testData[i].input );

            // Parse data for this iteration
            ASSERT_TRUE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
            tokenizer.EndParsing();

            // Verify postconditions
            for( char const* const* outPtr = testData[i].output; *outPtr != NULL; ++outPtr )
            {
                ASSERT_TRUE( observer.VerifyNext( *outPtr ) );
            }
            ASSERT_TRUE( observer.VerifyNone() );

            ASSERT_EQ( dataLen, bytesRead );
        }
    }

} // namespace BfsdlTests

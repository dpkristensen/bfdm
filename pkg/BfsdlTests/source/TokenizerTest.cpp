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
            char const* output;
        };

        // Test condition table
        // NOTE: This does not need to be extremely verbose
        static TestData const testData[] =
        { //  Input             Output
            { "#",              NULL },
            { "##",             NULL },
            { "#b:01111011#",   "+01111011" },
            { "#d:123#",        "+123" },
            { "#x:7b#",         "+7b" },
            { "#o:8#",          NULL },
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
            ASSERT_TRUE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
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
            char const* output;
        };

        // Test condition table
        // NOTE: This does not need to be extremely verbose.  Output is always in utf8
        static TestData const testData[] =
        { //  Input             Output
            { "\"",             NULL },
            { "\"\"",           "" },
            { "\"abc123\"",     "61 62 63 31 32 33" },
            // Hex escape sequences
            { "\"\\x62c3\"",    "62 63 33" },
            { "\"\\0x62c3\"",   NULL },
            { "\"\\1x62c3\"",   "06 32 63 33" },
            { "\"\\4x62c3\"",   "e6 8b 83" },
            { "\"\\8x12345678\"", "fc 92 8d 85 99 b8" },
            { "\"\\9x123456780\"", NULL },
            { "\"\\x80\"",      "c2 80" },
            { "\"\\x00\"",      "00" },
            { "\"\\x0g\"",      NULL },
            { "\"\\x0z\"",      NULL },
            { "\"\\x\"",        NULL },
            { "\"\\x1\"",       NULL },
            // Ascii escape sequences
            { "\"\\a41\"",      "41" },
            { "\"\\a7f\"",      "7f" },
            { "\"\\a80\"",      NULL },
            { "\"\\0a7f\"",     NULL },
            { "\"\\2a7f\"",     NULL },
            { "\"\\a0g\"",      NULL },
            { "\"\\a0z\"",      NULL },
            { "\"\\a\"",        NULL },
            { "\"\\a7\"",       NULL },
            // MS-1252 escape sequences
            { "\"\\w41\"",      "41" },
            { "\"\\w7e\"",      "7e" },
            { "\"\\w7f\"",      NULL },
            { "\"\\w80\"",      "e2 82 ac" },
            { "\"\\w8c\"",      "c5 92" },
            { "\"\\w8d\"",      NULL },
            { "\"\\w8e\"",      "c5 bd" },
            { "\"\\0w7f\"",     NULL },
            { "\"\\2w7f\"",     NULL },
            { "\"\\w0g\"",      NULL },
            { "\"\\w0z\"",      NULL },
            { "\"\\w\"",        NULL },
            { "\"\\w7\"",       NULL },
            // Unicode escape sequences
            { "\"\\u0000\"",        "00" },
            { "\"\\u00b9\"",        "c2 b9" },
            { "\"\\4u000b9\"",      "0b 39" },
            { "\"\\5u000b9\"",      "c2 b9" },
            { "\"\\5u1f913\"",      "f0 9f a4 93" },
            { "\"\\8u7fffffff\"",   "fd bf bf bf bf bf" },
            { "\"\\8u80000000\"",   NULL },
            { "\"\\9u0000000b9\"",  NULL },
            { "\"\\0u1234\"",       NULL },
            { "\"\\u000g\"",        NULL },
            { "\"\\u000z\"",        NULL },
            { "\"\\u\"",            NULL },
            { "\"\\u7\"",           NULL },
            { "\"\\u123\"",         NULL },
            // Other escape sequences
            { "\"\\\"\"",       "22" },
            { "\"\\t\"",        "08" },
            { "\"\\r\"",        "0d" },
            { "\"\\n\"",        "0a" },
            { "\"\\\\\"",       "5c" },
            // Multiple escape sequences together
            { "\"\\\"\\t\\r\\n\\\\t\"", "22 08 0d 0a 5c 74" },
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
            ASSERT_TRUE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData[i].input ), dataLen, bytesRead ) );
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

} // namespace BfsdlTests

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
        SizeT bytesRead = 0;
        SizeT dataLen = std::strlen( testData );

        errWorkspace.ExpectRunTimeError();
        ASSERT_FALSE( tokenizer.Parse( reinterpret_cast< Byte const * >( testData ), dataLen, bytesRead ) );
        errWorkspace.VerifyRunTimeError();

        ASSERT_EQ( 0, bytesRead );
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

        SizeT bytesRead = 0;
        SizeT dataLen = std::strlen( testData );
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

        SizeT bytesRead = 0;
        SizeT dataLen = std::strlen( testData );
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
            { "##",             NULL },
            { "#b:01111011#",   "+01111011" },
            { "#d:123#",        "+123" },
            { "#x:7b#",         "+7b" },
            { "#o:8#",          NULL },
        };
        static SizeT const numTests = BFDP_COUNT_OF_ARRAY( testData );

        // Loop through test conditions
        for( SizeT i = 0; i < numTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "input=" ) << testData[i].input << std::endl );

            Token::Tokenizer tokenizer( observer );
            ASSERT_TRUE( tokenizer.IsInitOk() );
            SizeT bytesRead = 0;
            SizeT dataLen = std::strlen( testData[i].input );

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

} // namespace BfsdlTests

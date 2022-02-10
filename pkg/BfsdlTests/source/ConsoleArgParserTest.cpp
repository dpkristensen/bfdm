/**
    Tests for BFDP Argument Parser

    Copyright 2020, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#include "Bfdp/Console/ArgParser.hpp"

#include <list>
#include <sstream>

#include "Bfdp/Macros.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    using Bfdp::Console::ArgParser;
    using Bfdp::Console::Param;

    class ConsoleArgParserTest
        : public ::testing::Test
    {
    public:
        void TearDown()
        {
            if( !mObservedCallbacks.empty() )
            {
                FAIL() << "Unexpected callback:" << std::endl
                        << "Expected: <none>" << std::endl
                        << "  Actual: " << mObservedCallbacks.front();
            }
        }

    protected:
        static uint32_t const sMAGIC = 0x12345678;

        struct CallbackContext
        {
            uint32_t mMagic;
            std::string mType;
            std::list< std::string >* mCaptureList;
        };

        static int CallbackFunc
            (
            ArgParser const& aParser,
            Param const& aParam,
            std::string const& aValue,
            uintptr_t const aUserdata
            )
        {
            CallbackContext* ctx = reinterpret_cast< CallbackContext* >( aUserdata );

            if( ctx == nullptr )
            {
                ADD_FAILURE() << "Callback context is NULL!";
            }
            else if( ctx->mMagic != sMAGIC )
            {
                ADD_FAILURE() << "Callback magic mismatch!";
            }
            std::stringstream ss;

            ss << ctx->mType << "." << aParser.GetName() << ":" << aParam.GetShortDescription()
                << "=" << aValue;
            ctx->mCaptureList->push_back( ss.str() );

            return 0;
        }

        ::testing::AssertionResult ParseWithCallbacksImpl
            (
            int const aExpectedResult,
            ArgParser& aParser,
            char const* const* const aArgV,
            int const aArgC
            )
        {
            CallbackContext ctx = { sMAGIC };
            ctx.mType = "parser";
            ctx.mCaptureList = &mObservedCallbacks;
            uintptr_t userdata = reinterpret_cast< uintptr_t >( &ctx );

            int result = aParser.Parse( aArgV, aArgC, &ConsoleArgParserTest::CallbackFunc, userdata );

            if( result != aExpectedResult )
            {
                return ::testing::AssertionFailure() << "Result mismatch:" << std::endl
                    << "Expected: " << aExpectedResult << std::endl
                    << "  Actual: " << result;
            }

            return ::testing::AssertionSuccess();
        }

        template< int COUNT >
        ::testing::AssertionResult ParseWithCallbacks
            (
            int const aExpectedResult,
            ArgParser& aParser,
            char const* const (&aArgV)[COUNT]
            )
        {
            return ParseWithCallbacksImpl( aExpectedResult, aParser, aArgV, COUNT );
        }


        ::testing::AssertionResult VerifyCallback
            (
            std::string const& aExpectedResult
            )
        {
            if( mObservedCallbacks.empty() )
            {
               return ::testing::AssertionFailure() << "Missing callback:" << std::endl
                    << "Expected: " << aExpectedResult << std::endl
                    << "  Actual: <none>";
            }

            std::string actualResult = mObservedCallbacks.front();
            mObservedCallbacks.pop_front();

            if( actualResult != aExpectedResult )
            {
               return ::testing::AssertionFailure() << "Callback mismatch:" << std::endl
                    << "Expected: " << aExpectedResult << std::endl
                    << "  Actual: " << actualResult;
            }

            return ::testing::AssertionSuccess();
        }

        std::list< std::string > mObservedCallbacks;
    };

    TEST_F( ConsoleArgParserTest, Callbacks )
    {
        CallbackContext paramTContext = { sMAGIC };
        paramTContext.mType = "param";
        paramTContext.mCaptureList = &mObservedCallbacks;
        ArgParser parser = ArgParser()
            .Add( Param::CreateLong( "foo", 'f' )
                .SetValueName( "bar" ) )
            .Add( Param::CreateShort( 't' )
                .SetOptional()
                .SetCallback( &ConsoleArgParserTest::CallbackFunc )
                .SetUserdataPtr( &paramTContext ) )
            .Add( Param::CreatePositional( "baz" )
                .SetOptional() );

        // Test sequence 1
        ASSERT_TRUE( ParseWithCallbacks( 0, parser, { "cmd1", "--foo", "testABC", "BAZvalue" } ) );
        ASSERT_TRUE( VerifyCallback( "parser.cmd1:foo <bar>=testABC" ) );
        // -t optional with no value, not observed
        ASSERT_TRUE( VerifyCallback( "parser.cmd1:baz=BAZvalue" ) );

        // Test sequence 2
        ASSERT_TRUE( ParseWithCallbacks( 0, parser, { "cmd2", "--foo", "testDEF", "-t" } ) );
        ASSERT_TRUE( VerifyCallback( "parser.cmd2:foo <bar>=testDEF" ) );
        ASSERT_TRUE( VerifyCallback( "param.cmd2:t=" ) );
    }

    TEST_F( ConsoleArgParserTest, DefaultHelp )
    {
        ArgParser parser = ArgParser()
            .AddHelp();

        char const* testArgs0[] = { "test0" };
        char const* testArgs1[] = { "test1", "foo" };
        char const* testArgs2[] = { "test2", "-h" };
        char const* testArgs3[] = { "test3", "/h" };
        char const* testArgs4[] = { "test4", "--help" };
        char const* testArgs5[] = { NULL };
        char const** testArgs6 = NULL;

        ClearErrorHandlers(); // These will throw errors, so ignore them.

        ASSERT_EQ( 0, parser.Parse( testArgs0, BFDP_COUNT_OF_ARRAY( testArgs0 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs1, BFDP_COUNT_OF_ARRAY( testArgs1 ) ) );
        ASSERT_EQ( 0, parser.Parse( testArgs2, BFDP_COUNT_OF_ARRAY( testArgs2 ) ) );
        ASSERT_EQ( 0, parser.Parse( testArgs3, BFDP_COUNT_OF_ARRAY( testArgs3 ) ) );
        ASSERT_EQ( 0, parser.Parse( testArgs4, BFDP_COUNT_OF_ARRAY( testArgs4 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs5, BFDP_COUNT_OF_ARRAY( testArgs5 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs6, 0 ) );
        ASSERT_EQ( 1, parser.Parse( testArgs6, 1 ) );
    }

    TEST_F( ConsoleArgParserTest, NoArgs )
    {
        ArgParser parser;
        char const* testArgs0[] = { "test0" };
        char const* testArgs1[] = { "test1", "foo" };
        char const* testArgs2[] = { "test2", "-h" };
        char const* testArgs3[] = { "test3", "/h" };
        char const* testArgs4[] = { "test4", "--help" };
        char const* testArgs5[] = { NULL };
        char const** testArgs6 = NULL;

        ClearErrorHandlers(); // These will throw errors, so ignore them.

        ASSERT_EQ( 0, parser.Parse( testArgs0, BFDP_COUNT_OF_ARRAY( testArgs0 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs1, BFDP_COUNT_OF_ARRAY( testArgs1 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs2, BFDP_COUNT_OF_ARRAY( testArgs2 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs3, BFDP_COUNT_OF_ARRAY( testArgs3 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs4, BFDP_COUNT_OF_ARRAY( testArgs4 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs5, BFDP_COUNT_OF_ARRAY( testArgs5 ) ) );
        ASSERT_EQ( 1, parser.Parse( testArgs6, 0 ) );
        ASSERT_EQ( 1, parser.Parse( testArgs6, 1 ) );
    }

    TEST_F( ConsoleArgParserTest, SetNameAfterParse )
    {
        ArgParser parser;

        // Initially empty
        std::string name = parser.GetName();
        ASSERT_STREQ( "", name.c_str() );

        // Verify name is empty after a parse
        char const* testArgs0[] = { "test0" };
        ASSERT_EQ( 0, parser.Parse( testArgs0, BFDP_COUNT_OF_ARRAY( testArgs0 ) ) );
        name = parser.GetName();
        ASSERT_STREQ( "", name.c_str() );

        // Set the name to something
        parser.SetName( "MyCommand" );
        name = parser.GetName();
        ASSERT_STREQ( "MyCommand", name.c_str() );

        // Verify another parse doesn't change it back
        ASSERT_EQ( 0, parser.Parse( testArgs0, BFDP_COUNT_OF_ARRAY( testArgs0 ) ) );
        name = parser.GetName();
        ASSERT_STREQ( "MyCommand", name.c_str() );

        // Verify another parse with a different command name doesn't change it
        char const* testArgs1[] = { "test1" };
        ASSERT_EQ( 0, parser.Parse( testArgs1, BFDP_COUNT_OF_ARRAY( testArgs1 ) ) );
        name = parser.GetName();
        ASSERT_STREQ( "MyCommand", name.c_str() );
    }

    TEST_F( ConsoleArgParserTest, SetNameBeforeParse )
    {
        ArgParser parser;

        // Initially empty
        std::string name = parser.GetName();
        ASSERT_STREQ( "", name.c_str() );

        // Set the name to something
        parser.SetName( "MyCommand" );
        name = parser.GetName();
        ASSERT_STREQ( "MyCommand", name.c_str() );

        // Change it
        parser.SetName( "Command2" );
        name = parser.GetName();
        ASSERT_STREQ( "Command2", name.c_str() );

        // Verify parse doesn't change it
        char const* testArgs0[] = { "test0" };
        ASSERT_EQ( 0, parser.Parse( testArgs0, BFDP_COUNT_OF_ARRAY( testArgs0 ) ) );
        name = parser.GetName();
        ASSERT_STREQ( "Command2", name.c_str() );
    }

} // namespace BfsdlTests

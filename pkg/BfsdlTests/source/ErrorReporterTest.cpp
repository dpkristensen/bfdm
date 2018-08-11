/**
    BFDP Error Reporter Tests

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

// External includes
#include <list>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "BfsdlTests/TestUtil.hpp"

#define BFDP_MODULE "ErrorReporterTest"

namespace BfsdlTests
{

    using namespace Bfdp;

    namespace ERT_Internal
    {
        // Type declarations are in a namespace to prevent accidental One Definition Rule
        // violations with other tests

        struct ErrorData
        {
            UInt line;
            char const* module;
            char const* text;
        };
    }

    using namespace ERT_Internal;

    static char const* const module = "Bfdp";
    static char const* const text1 = "This is a test";
    static char const* const text2 = "This is another test";
    static char const* const text3 = "This is yet another test";

    static std::list< ErrorData > gInternalEvents;
    static std::list< ErrorData > gMisuseEvents;
    static std::list< ErrorData > gRunTimeEvents;

    static void InternalErrorHandler
        (
        char const* const aModuleName,
        UInt const aLine,
        char const* const aErrorText
        )
    {
        ErrorData e;
        e.line = aLine;
        e.module = aModuleName;
        e.text = aErrorText;

        gInternalEvents.push_back( e );
    }

    static void MisuseErrorHandler
        (
        char const* const aModuleName,
        UInt const aLine,
        char const* const aErrorText
        )
    {
        ErrorData e;
        e.line = aLine;
        e.module = aModuleName;
        e.text = aErrorText;

        gMisuseEvents.push_back( e );
    }

    static void RunTimeErrorHandler
        (
        char const* const aModuleName,
        UInt const aLine,
        char const* const aErrorText
        )
    {
        ErrorData e;
        e.line = aLine;
        e.module = aModuleName;
        e.text = aErrorText;

        gRunTimeEvents.push_back( e );
    }

    class ErrorReporterTest
        : public ::testing::Test
    {
    private:
        void SetUp()
        {
            gInternalEvents.clear();
            gMisuseEvents.clear();
            gRunTimeEvents.clear();

            ErrorReporter::SetInternalErrorHandler( InternalErrorHandler );
            ErrorReporter::SetMisuseErrorHandler( MisuseErrorHandler );
            ErrorReporter::SetRunTimeErrorHandler( RunTimeErrorHandler );
        }
    };

    TEST_F( ErrorReporterTest, NoListeners )
    {
        ClearErrorHandlers();

        BFDP_MISUSE_ERROR( text1 );
        BFDP_RUNTIME_ERROR( text2 );

        ASSERT_EQ( 0, gInternalEvents.size() );
        ASSERT_EQ( 0, gMisuseEvents.size() );
        ASSERT_EQ( 0, gRunTimeEvents.size() );
    }

    TEST_F( ErrorReporterTest, Macros )
    {
        BFDP_INTERNAL_ERROR( text1 );  UInt internalLine = __LINE__;
        BFDP_MISUSE_ERROR( text2 );   UInt misuseLine = __LINE__;
        BFDP_RUNTIME_ERROR( text3 );  UInt runtimeLine = __LINE__;

        ASSERT_EQ( 1, gInternalEvents.size() );
        ASSERT_EQ( internalLine, gInternalEvents.front().line );
        ASSERT_STREQ( BFDP_MODULE, gInternalEvents.front().module );
        ASSERT_STREQ( text1, gInternalEvents.front().text );

        ASSERT_EQ( 1, gMisuseEvents.size() );
        ASSERT_EQ( misuseLine, gMisuseEvents.front().line );
        ASSERT_STREQ( BFDP_MODULE, gMisuseEvents.front().module );
        ASSERT_STREQ( text2, gMisuseEvents.front().text );

        ASSERT_EQ( 1, gRunTimeEvents.size() );
        ASSERT_EQ( runtimeLine, gRunTimeEvents.front().line );
        ASSERT_STREQ( BFDP_MODULE, gRunTimeEvents.front().module );
        ASSERT_STREQ( text3, gRunTimeEvents.front().text );
    }

    TEST_F( ErrorReporterTest, InternalError )
    {
        static char const* fakeModule = "InternalErrorModule";

        ErrorReporter::ReportInternalError( fakeModule, 123, text3 );

        ASSERT_EQ( 1, gInternalEvents.size() );
        ASSERT_EQ( 123, gInternalEvents.front().line );
        ASSERT_STREQ( fakeModule, gInternalEvents.front().module );
        ASSERT_STREQ( text3, gInternalEvents.front().text );

        ASSERT_EQ( 0, gMisuseEvents.size() );

        ASSERT_EQ( 0, gRunTimeEvents.size() );
    }

    TEST_F( ErrorReporterTest, MisuseError )
    {
        static char const* fakeModule = "MisuseErrorModule";

        ErrorReporter::ReportMisuseError( fakeModule, 456, text1 );

        ASSERT_EQ( 0, gInternalEvents.size() );

        ASSERT_EQ( 1, gMisuseEvents.size() );
        ASSERT_EQ( 456, gMisuseEvents.front().line );
        ASSERT_STREQ( fakeModule, gMisuseEvents.front().module );
        ASSERT_STREQ( text1, gMisuseEvents.front().text );

        ASSERT_EQ( 0, gRunTimeEvents.size() );
    }

    TEST_F( ErrorReporterTest, RunTimeError )
    {
        static char const* fakeModule = "RunTimeErrorModule";

        ErrorReporter::ReportRunTimeError( fakeModule, 789, text2 );

        ASSERT_EQ( 0, gInternalEvents.size() );

        ASSERT_EQ( 0, gMisuseEvents.size() );

        ASSERT_EQ( 1, gRunTimeEvents.size() );
        ASSERT_EQ( 789, gRunTimeEvents.front().line );
        ASSERT_STREQ( fakeModule, gRunTimeEvents.front().module );
        ASSERT_STREQ( text2, gRunTimeEvents.front().text );
    }

} // namespace BfsdlTests
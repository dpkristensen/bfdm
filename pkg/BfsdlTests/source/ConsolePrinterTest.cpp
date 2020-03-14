/**
    Tests for BFDP Console Printer

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

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

#include "Bfdp/Macros.hpp"
#include "Bfdp/Console/Printer.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    class ConsolePrinterTest
        : public ::testing::Test
    {
    protected:
        static size_t BFDP_CONSTEXPR MaxBufLen = 15; // Should be enough for all test cases

        testing::AssertionResult VerifyLine
            (
            std::stringstream& aStream,
            std::string const& aExpected
            )
        {
            char buf[MaxBufLen];
            if( aStream.getline( buf, MaxBufLen ).fail() )
            {
                return testing::AssertionFailure() << "Stream error!";
            }
            // Use string to prevent accidental buffer overflow
            std::string actual( buf, 0, MaxBufLen );

            if( aExpected != actual )
            {
                return testing::AssertionFailure()
                    << "FAIL:" << std::endl
                    << "  Expected: '" << aExpected << "'" << std::endl
                    << "  Actual:   '" << actual << "'";
            }

            return testing::AssertionSuccess();
        }

        testing::AssertionResult VerifyEnd
            (
            std::stringstream& aStream
            )
        {
            char buf[MaxBufLen];
            if( aStream.getline( buf, MaxBufLen ).fail() )
            {
                if( aStream.eof() )
                {
                    return ::testing::AssertionSuccess();
                }

                return testing::AssertionFailure() << "Stream error!";
            }

            // Use string to prevent accidental buffer overflow
            std::string actual( buf, 0, MaxBufLen );

            return testing::AssertionFailure()
                << "Unexpected text at end: '" << actual << "'";
        }
    };

    TEST_F( ConsolePrinterTest, BasicOutput )
    {
        std::stringstream ss;
        Console::Printer p( ss, "--" );

        p << "1" << "2";
        p.Put( "3" );
        p.Print( "." );
        p.Print( "4" );
        p.Indent();
        p.Print( "4.1" );
        p.Indent();
        p.Print( "4.1.1" );
        p.Unindent();
        p.Print( "4.2" );
        p.Unindent();
        p.Print( "5" );
        p.Unindent(); // Underflow indent ignored
        p.Print( "6" );

        ss.seekg( 0 );
        ASSERT_TRUE( VerifyLine( ss, "1" ) );
        ASSERT_TRUE( VerifyLine( ss, "2" ) );
        ASSERT_TRUE( VerifyLine( ss, "3." ) );
        ASSERT_TRUE( VerifyLine( ss, "4" ) );
        ASSERT_TRUE( VerifyLine( ss, "--4.1" ) );
        ASSERT_TRUE( VerifyLine( ss, "----4.1.1" ) );
        ASSERT_TRUE( VerifyLine( ss, "--4.2" ) );
        ASSERT_TRUE( VerifyLine( ss, "5" ) );
        ASSERT_TRUE( VerifyLine( ss, "6" ) );
        ASSERT_TRUE( VerifyEnd( ss ) );
    }

    TEST_F( ConsolePrinterTest, ManualIndent )
    {
        std::stringstream ss;
        Console::Printer p( ss, "  " );

        p.Print( "1" );
        p.PrintIndent(); // No effect since there is no indent
        p.Print( "2" );
        p.Indent();
        p.Put( "2.1" );
        p.Indent();
        p.PrintIndent();
        p.Print( "2x" );
        p.Unindent();
        p.Put( "2.2" );
        p.PrintIndent();
        p.Print( "1x" );
        p.Unindent();
        p << "3";

        ss.seekg( 0 );
        ASSERT_TRUE( VerifyLine( ss, "1" ) );
        ASSERT_TRUE( VerifyLine( ss, "2" ) );
        ASSERT_TRUE( VerifyLine( ss, "  2.1    2x" ) );
        ASSERT_TRUE( VerifyLine( ss, "  2.2  1x" ) );
        ASSERT_TRUE( VerifyLine( ss, "3" ) );
        ASSERT_TRUE( VerifyEnd( ss ) );
    }

    TEST_F( ConsolePrinterTest, PutIndentOnce )
    {
        std::stringstream ss;
        Console::Printer p( ss, "  " );

        p << "A";
        p.Indent();
        p.Put( "A" ); // Indent only added here
        p.Put( "." );
        p.Print( "2" );

        p.Unindent();
        p << "B";

        ss.seekg( 0 );
        ASSERT_TRUE( VerifyLine( ss, "A" ) );
        ASSERT_TRUE( VerifyLine( ss, "  A.2" ) );
        ASSERT_TRUE( VerifyLine( ss, "B" ) );
        ASSERT_TRUE( VerifyEnd( ss ) );
    }

} // namespace BfsdlTests

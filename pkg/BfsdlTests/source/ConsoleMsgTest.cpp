/**
    Tests for BFDP Console Msg

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

#include "Bfdp/Console/Msg.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    class ConsoleMsgTest
        : public ::testing::Test
    {
    protected:
        // Helper function to copy a Msg object without worrying about compiler semantics
        Console::Msg CopyMsg
            (
            Console::Msg const& aMsg
            )
        {
            return aMsg;
        }
    };

    TEST_F( ConsoleMsgTest, ConstructDefault )
    {
        Console::Msg obj;

        ASSERT_TRUE( obj.Empty() );
        BFDP_ASSERT_STREQ( "", obj );
        BFDP_ASSERT_STREQ( "", obj.Get() );
    }

    TEST_F( ConsoleMsgTest, ConstructCharConst )
    {
        // Explicit Construction
        Console::Msg explicitObj( "ExplicitChar" );
        ASSERT_FALSE( explicitObj.Empty() );
        BFDP_ASSERT_STREQ( "ExplicitChar", explicitObj );
        BFDP_ASSERT_STREQ( "ExplicitChar", explicitObj.Get() );

        // Implicit construction
        Console::Msg implicitObj = "ImplicitChar";
        ASSERT_FALSE( implicitObj.Empty() );
        BFDP_ASSERT_STREQ( "ImplicitChar", implicitObj );
        BFDP_ASSERT_STREQ( "ImplicitChar", implicitObj.Get() );
    }

    TEST_F( ConsoleMsgTest, ConstructString )
    {
        // Explicit Construction
        Console::Msg explicitObj( std::string( "ExplicitString" ) );
        ASSERT_FALSE( explicitObj.Empty() );
        BFDP_ASSERT_STREQ( "ExplicitString", explicitObj );

        // Implicit construction
        Console::Msg implicitObj = std::string( "ImplicitString" );
        ASSERT_FALSE( implicitObj.Empty() );
        BFDP_ASSERT_STREQ( "ImplicitString", implicitObj );
    }

    TEST_F( ConsoleMsgTest, Copy )
    {
        // Assign Empty Object
        Console::Msg assignEmpty;
        assignEmpty = Console::Msg();
        ASSERT_TRUE( assignEmpty.Empty() );
        BFDP_ASSERT_STREQ( "", assignEmpty );

        // Assign Non-Empty Object
        Console::Msg assignNonEmpty;
        assignNonEmpty = Console::Msg( "AssignNonEmpty" );
        ASSERT_FALSE( assignNonEmpty.Empty() );
        BFDP_ASSERT_STREQ( "AssignNonEmpty", assignNonEmpty );

        // Copy Empty Object
        Console::Msg copyEmpty = CopyMsg( Console::Msg() );
        ASSERT_TRUE( copyEmpty.Empty() );
        BFDP_ASSERT_STREQ( "", copyEmpty );

        // Copy Non-Empty Object
        Console::Msg copyNonEmpty = CopyMsg( Console::Msg( "CopyNonEmpty" ) );
        ASSERT_FALSE( copyNonEmpty.Empty() );
        BFDP_ASSERT_STREQ( "CopyNonEmpty", copyNonEmpty );
    }

    TEST_F( ConsoleMsgTest, Append )
    {
        Console::Msg msg( "1" );

        msg << " 2";
        ASSERT_FALSE( msg.Empty() );
        BFDP_ASSERT_STREQ( "1 2", msg );

        msg << " 3" << " 4";
        ASSERT_FALSE( msg.Empty() );
        BFDP_ASSERT_STREQ( "1 2 3 4", msg );
    }

} // namespace BfsdlTests

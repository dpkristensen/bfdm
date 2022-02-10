/**
    Tests for BFDP Command-line Parameter Test

    Copyright 2021, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#include "Bfdp/Console/Param.hpp"

#include "Bfdp/Macros.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    using Bfdp::Console::Param;

    class ConsoleParamTest
        : public ::testing::Test
    {
    };

    TEST_F( ConsoleParamTest, ConstructCommand )
    {
        Param p = Param::CreateCommand();

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_TRUE( p.IsPositional() );
        ASSERT_TRUE( p.IsTerminator() );

        // Commands don't match by name at all
        ASSERT_FALSE( p.MatchLongName( "command" ) );
        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_FALSE( p.MatchShortName( "command" ) );
        ASSERT_FALSE( p.MatchShortName( "c" ) );
    }

    TEST_F( ConsoleParamTest, ConstructCommandNamed )
    {
        Param p = Param::CreateCommand( "cmd_name" );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_TRUE( p.IsPositional() );
        ASSERT_TRUE( p.IsTerminator() );

        // Commands don't match by name at all
        ASSERT_FALSE( p.MatchLongName( "cmd_name" ) );
        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_FALSE( p.MatchShortName( "cmd_name" ) );
        ASSERT_FALSE( p.MatchShortName( "c" ) );
    }

    TEST_F( ConsoleParamTest, ConstructLongName )
    {
        Param p = Param::CreateLong( "copy" );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );

        ASSERT_TRUE( p.MatchLongName( "copy" ) );
        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_FALSE( p.MatchShortName( "copy" ) ); // Invalid use case
        ASSERT_FALSE( p.MatchShortName( "c" ) ); // Invalid use case
    }

    TEST_F( ConsoleParamTest, ConstructLongAndShortName )
    {
        Param p = Param::CreateLong( "copy", 'c' );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );

        ASSERT_TRUE( p.MatchLongName( "copy" ) );
        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_FALSE( p.MatchShortName( "copy" ) ); // Invalid use case
        ASSERT_TRUE( p.MatchShortName( "c" ) );
    }

    TEST_F( ConsoleParamTest, ConstructPositional )
    {
        Param p = Param::CreatePositional( "copy_src" );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_TRUE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );

        // Positionals don't match by name at all
        ASSERT_FALSE( p.MatchLongName( "copy_src" ) );
        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_FALSE( p.MatchShortName( "copy_src" ) );
        ASSERT_FALSE( p.MatchShortName( "c" ) );
    }

    TEST_F( ConsoleParamTest, ConstructShortName )
    {
        Param p = Param::CreateShort( 'c' );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );

        ASSERT_FALSE( p.MatchLongName( "c" ) );
        ASSERT_TRUE( p.MatchShortName( "c" ) );
    }

    TEST_F( ConsoleParamTest, SetDefaultValue )
    {
        Param p = Param::CreateShort( 'c' )
            .SetDefault( "herp" );

        ASSERT_TRUE( StrEq( "default: herp", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_TRUE( p.HasValue() );
        ASSERT_TRUE( StrEq( "herp", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_TRUE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetDefaultValueAndName )
    {
        Param p = Param::CreateShort( 'c' )
            .SetDefault( "herp", "derp" );

        ASSERT_TRUE( StrEq( "default: herp", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_TRUE( p.HasValue() );
        ASSERT_TRUE( StrEq( "herp", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "derp", p.GetValueName() ) );
        ASSERT_TRUE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetCallChaining )
    {
        int someData = 42;

        Param p = Param::CreateLong( "copy", 'c' )
            .SetDescription( "Copy stuff" )
            .SetValueName( "copy_src" )
            .SetDefault( "~/Documents/foo.txt" )
            .SetUserdata(  reinterpret_cast< uintptr_t >( &someData ) );

        ASSERT_TRUE( StrEq( "Copy stuff (default: ~/Documents/foo.txt)", p.GetDescription() ) );
        ASSERT_NE( 0U, p.GetUserdata() );
        int* ptr = reinterpret_cast< int* >( p.GetUserdata() );
        ASSERT_EQ( *ptr, 42 );
        ASSERT_TRUE( p.HasValue() );
        ASSERT_TRUE( StrEq( "~/Documents/foo.txt", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "copy_src", p.GetValueName() ) );
        ASSERT_TRUE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetOptional )
    {
        Param p = Param::CreateShort( 'c' )
            .SetOptional();

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_TRUE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetTerminator )
    {
        Param p = Param::CreateShort( 'c' )
            .SetTerminator();

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_TRUE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetUserdata )
    {
        Param p = Param::CreateShort( 'c' )
            .SetUserdata( 42U );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 42U, p.GetUserdata() );
        ASSERT_FALSE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "value", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

    TEST_F( ConsoleParamTest, SetValueName )
    {
        Param p = Param::CreateShort( 'c' )
            .SetValueName( "derp" );

        ASSERT_TRUE( StrEq( "", p.GetDescription() ) );
        ASSERT_EQ( 0U, p.GetUserdata() );
        ASSERT_TRUE( p.HasValue() );
        ASSERT_TRUE( StrEq( "", p.GetDefaultValue() ) );
        ASSERT_TRUE( StrEq( "derp", p.GetValueName() ) );
        ASSERT_FALSE( p.IsOptional() );
        ASSERT_FALSE( p.IsPositional() );
        ASSERT_FALSE( p.IsTerminator() );
    }

} // namespace BfsdlTests

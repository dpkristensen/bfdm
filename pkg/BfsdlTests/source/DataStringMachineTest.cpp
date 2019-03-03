/**
    BFDP Data StringMachine Test

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

#include "Bfdp/Data/StringMachine.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/Ms1252Converter.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    class DataStringMachineTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    using namespace Bfdp;
    using Bfdp::Data::StringMachine;

    TEST_F( DataStringMachineTest, AppendConverted )
    {
        StringMachine machine;
        std::string strVal;

        ASSERT_TRUE( machine.AppendUnicode( 88U ) );

        Unicode::Ms1252Converter ms1252;
        ASSERT_TRUE( machine.AppendString( ms1252, "Y\x8e" ) );
        ASSERT_STREQ( "XY\xc5\xbd", machine.GetUtf8String().c_str() );

        Unicode::AsciiConverter ascii;
        ASSERT_FALSE( machine.AppendString( ascii, "\x8e" ) );
    }

    TEST_F( DataStringMachineTest, AppendEmpty )
    {
        StringMachine machine;
        std::string strVal;

        machine.AppendUtf8( "" );

        ASSERT_TRUE( machine.IsDefined() );
        ASSERT_TRUE( machine.IsEmpty() );
        strVal = machine.GetUtf8String();
        ASSERT_TRUE( strVal.empty() );
        strVal = machine.GetUtf8HexString();
        ASSERT_TRUE( strVal.empty() );
    }

    TEST_F( DataStringMachineTest, AppendUtf8 )
    {
        StringMachine machine;
        std::string strVal;

        machine.AppendUtf8( "abc" );
        machine.AppendUtf8( "\xc2\xb9" );
        machine.AppendUtf8( "\xc2\xb2" );
        machine.AppendUtf8( "\xc2\xb3" );

        ASSERT_TRUE( machine.IsDefined() );
        ASSERT_FALSE( machine.IsEmpty() );
        strVal = machine.GetUtf8String();
        ASSERT_STREQ( "abc\xc2\xb9\xc2\xb2\xc2\xb3", strVal.c_str() );
        strVal = machine.GetUtf8HexString();
        ASSERT_STREQ( "616263c2b9c2b2c2b3", strVal.c_str() );
        strVal = machine.GetUtf8HexString( " " );
        ASSERT_STREQ( "61 62 63 c2 b9 c2 b2 c2 b3", strVal.c_str() );
        strVal = machine.GetUtf8HexString( ", ", "0x" );
        ASSERT_STREQ( "0x61, 0x62, 0x63, 0xc2, 0xb9, 0xc2, 0xb2, 0xc2, 0xb3", strVal.c_str() );
    }

    TEST_F( DataStringMachineTest, CreateDefault )
    {
        StringMachine machine;
        std::string strVal;

        ASSERT_FALSE( machine.IsDefined() );
        ASSERT_TRUE( machine.IsEmpty() );
        strVal = machine.GetUtf8String();
        ASSERT_TRUE( strVal.empty() );
        strVal = machine.GetUtf8HexString();
        ASSERT_TRUE( strVal.empty() );
    }

    TEST_F( DataStringMachineTest, DefineEmpty )
    {
        StringMachine machine;
        std::string strVal;

        machine.SetDefined();

        ASSERT_TRUE( machine.IsDefined() );
        ASSERT_TRUE( machine.IsEmpty() );
        strVal = machine.GetUtf8String();
        ASSERT_TRUE( strVal.empty() );
        strVal = machine.GetUtf8HexString();
        ASSERT_TRUE( strVal.empty() );
    }

    TEST_F( DataStringMachineTest, GetConvertedString )
    {
        StringMachine machine;
        std::string strVal;

        machine.AppendUtf8( "XY\xc5\xbd" );

        Unicode::Ms1252Converter ms1252;
        ASSERT_TRUE( machine.GetString( ms1252, strVal ) );
        ASSERT_STREQ( "XY\x8e", strVal.c_str() );

        Unicode::AsciiConverter ascii;
        ASSERT_FALSE( machine.GetString( ascii, strVal ) );
    }

} // namespace BfsdlTests

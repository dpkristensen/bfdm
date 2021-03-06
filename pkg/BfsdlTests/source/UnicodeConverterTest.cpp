/**
    BFDP Unicode Converter Tests

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
#include <cstring>
#include "gtest/gtest.h"

// Internal includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/String.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/CodingMap.hpp"
#include "Bfdp/Unicode/Ms1252Converter.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class UnicodeConverterTest
        : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

        Unicode::AsciiConverter ascii;
        Unicode::Ms1252Converter ms1252;
        Unicode::Utf8Converter utf8;
    };

    TEST_F( UnicodeConverterTest, ConversionSize )
    {
        ASSERT_EQ( 1U, ascii.GetMaxBytes() );
        ASSERT_EQ( 1U, ms1252.GetMaxBytes() );
        ASSERT_EQ( 6U, utf8.GetMaxBytes() );
    }

    TEST_F( UnicodeConverterTest, CodingMap )
    {
        static struct TestData
        {
            char const* text;
            bool valid;
        } const sTestData[] =
        {
            { "ASCII",       true },
            { "ASC",        false },
            { "Ascii",      false },
            { "MS-1252",     true },
            { "MS",         false },
            { "MS-1253",    false },
            { "MS1252",     false },
            { "ms-1252",    false },
            { "HP-7J",      false },
            { "IBM-1",      false },
            { "IEC-62106",  false },
            { "ISO-8859-1", false },
            { "UTF8",        true },
            { "UTF-8",      false },
            { "Utf8",       false },
        };
        static size_t BFDP_CONSTEXPR sNumTests = BFDP_COUNT_OF_ARRAY( sTestData );

        for( size_t i = 0; i < sNumTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "coding=" ) << sTestData[i].text );

            std::string coding = sTestData[i].text;
            ASSERT_EQ( sTestData[i].valid, Bfdp::Unicode::IsValidCoding( coding ) );
            Bfdp::Unicode::IConverterPtr ptr = Bfdp::Unicode::GetCodec( Bfdp::Unicode::GetCodingId( coding ) );
            ASSERT_EQ( sTestData[i].valid, ( ptr != NULL ) );
        }
    }

    TEST_F( UnicodeConverterTest, ASCII )
    {
        struct TestData
        {
            Byte bVal;
            Unicode::CodePoint uVal;
        };

        TestData const test[] =
        {
            { 0,              0 },
            { Char( '0' ),   48 },
            { Char( '~' ),  126 },
            { 0x7F,         127 }
        };

        Byte buf;
        Unicode::CodePoint cp;

        // Test conversion table of valid values
        for( size_t i = 0; i < BFDP_COUNT_OF_ARRAY( test ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            // Test decoding
            buf = test[i].bVal;
            cp = Unicode::InvalidCodePoint;
            ASSERT_EQ( 1U, ascii.ConvertBytes( &buf, 1, cp ) );
            ASSERT_EQ( test[i].uVal, cp );

            // Test direct conversion
            ASSERT_TRUE( Unicode::AsciiConverter::GetUnicode( test[i].bVal, cp ) );
            ASSERT_EQ( test[i].uVal, cp );

            // Test encoding
            buf = 128; // Not a valid code
            cp = test[i].uVal;
            ASSERT_EQ( 1U, ascii.ConvertSymbol( cp, &buf, 1 ) );
            ASSERT_EQ( test[i].bVal, buf );
        }

        // Test conversion of undefined values
        Byte const invalidBytes[] = { 0x80, 0xFF };
        cp = 89; // Valid code point
        for( size_t i = 0U; i < BFDP_COUNT_OF_ARRAY( invalidBytes ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            // Test decoding
            ASSERT_EQ( 0U, ascii.ConvertBytes( &invalidBytes[i], 1, cp ) );
            ASSERT_EQ( 89, cp ); // Unchanged

            // Test direct conversion
            ASSERT_FALSE( Unicode::AsciiConverter::GetUnicode( invalidBytes[i], cp ) );
            ASSERT_EQ( 89, cp ); // Unchanged
        }

        SetMockErrorHandlers();
        MockErrorHandler::Workspace wksp;

        // Test invalid inputs
        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ascii.ConvertBytes( NULL, 1, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ascii.ConvertBytes( &buf, 0, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ascii.ConvertSymbol( cp, NULL, 1U ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ascii.ConvertSymbol( cp, &buf, 0U ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );
    }

    TEST_F( UnicodeConverterTest, MS_1252 )
    {
        struct TestData
        {
            Byte bVal;
            Unicode::CodePoint uVal;
        };

        TestData const test[] =
        {
            { 0,              0 },
            { Char( '0' ),   48 },
            { Char( '~' ),  126 },
            { 0x80,        8364 },
            { 0x81,         129 },
            { 0x8E,         381 },
            { 0x96,        8211 },
            { 0x97,        8212 },
            { 0x9F,         376 },
            { 0xA0,         160 },
            { 0xFF,         255 }
        };

        Byte buf;
        Unicode::CodePoint cp;

        // Test conversion table of valid values
        for( size_t i = 0; i < BFDP_COUNT_OF_ARRAY( test ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            // Test decoding
            buf = test[i].bVal;
            cp = Unicode::InvalidCodePoint;
            ASSERT_EQ( 1U, ms1252.ConvertBytes( &buf, 1, cp ) );
            ASSERT_EQ( test[i].uVal, cp );

            // Test direct conversion
            ASSERT_TRUE( Unicode::Ms1252Converter::GetUnicode( test[i].bVal, cp ) );
            ASSERT_EQ( test[i].uVal, cp );

            // Test encoding
            buf = 127; // Not a valid code
            cp = test[i].uVal;
            ASSERT_EQ( 1U, ms1252.ConvertSymbol( cp, &buf, 1 ) );
            ASSERT_EQ( test[i].bVal, buf );
        }

        // Test conversion of undefined values
        Byte const invalidBytes[] = { 0x7F, 0x8D, 0x8F, 0x90, 0x9D };
        cp = 89; // Valid code point
        for( size_t i = 0; i < BFDP_COUNT_OF_ARRAY( invalidBytes ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            // Test decoding
            ASSERT_EQ( 0U, ms1252.ConvertBytes( &invalidBytes[i], 1, cp ) );
            ASSERT_EQ( 89, cp ); // Unchanged

            // Test direct conversion
            ASSERT_FALSE( Unicode::Ms1252Converter::GetUnicode( invalidBytes[i], cp ) );
            ASSERT_EQ( 89, cp ); // Unchanged
        }

        SetMockErrorHandlers();
        MockErrorHandler::Workspace wksp;

        // Test invalid inputs
        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ms1252.ConvertBytes( NULL, 1, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ms1252.ConvertBytes( &buf, 0, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ms1252.ConvertSymbol( cp, NULL, 1 ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, ms1252.ConvertSymbol( cp, &buf, 0 ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );
    }

    TEST_F( UnicodeConverterTest, UTF8 )
    {
        struct TestData
        {
            size_t const numBytes;
            Byte const* bVal;
            Unicode::CodePoint uVal;
        };

        TestData const test[] =
        {
            { 1, Char( "\0" ),                          0x00000000 },
            { 1, Char( "0" ),                           0x00000030 },
            { 1, Char( "~" ),                           0x0000007e },
            { 1, Char( "\x7f" ),                        0x0000007f },
            { 2, Char( "\xc2\x80" ),                    0x00000080 },
            { 2, Char( "\xde\xaa" ),                    0x000007aa },
            { 2, Char( "\xdf\xbf" ),                    0x000007ff },
            { 3, Char( "\xeb\x9d\xbb" ),                0x0000b77b },
            { 3, Char( "\xef\xbf\xbf" ),                0x0000ffff },
            { 4, Char( "\xf5\xae\xbd\xbd" ),            0x0016ef7d },
            { 4, Char( "\xf7\xbf\xbf\xbf" ),            0x001fffff },
            { 5, Char( "\xfa\xb7\x9e\xbe\xbf" ),        0x02ddefbf },
            { 5, Char( "\xfb\xbf\xbf\xbf\xbf" ),        0x03ffffff },
            { 6, Char( "\xfd\x9b\xaf\x9f\x9f\xaf" ),    0x5bbdf7ef },
            { 6, Char( "\xfd\xbf\xbf\xbf\xbf\xbf" ),    0x7fffffff }
        };

        Byte buf[7];
        Unicode::CodePoint cp;

        // Test conversion table of valid values
        for( size_t i = 0; i < BFDP_COUNT_OF_ARRAY( test ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            ASSERT_GE( sizeof( buf ), test[i].numBytes );
            std::memset( buf, 0, sizeof( buf ) );
            std::memcpy( buf, test[i].bVal, test[i].numBytes );

            // Test decoding
            cp = Unicode::InvalidCodePoint;
            ASSERT_EQ( test[i].numBytes, utf8.ConvertBytes( buf, test[i].numBytes, cp ) );
            ASSERT_EQ( test[i].uVal, cp );

            // Test encoding
            memset( buf, 0xff, sizeof( buf ) ); // not a valid code
            cp = test[i].uVal;
            ASSERT_EQ( test[i].numBytes, utf8.ConvertSymbol( cp, buf, 6 ) );
            ASSERT_TRUE( ArraysMatch( test[i].bVal, buf, test[i].numBytes ) );
        }

        // Test conversion of undefined values
        TestData const invalidBytes[] =
        {
            { 2, Char( "\x80\x80" ) }, // Invalid byte 0: 10xxxxxx
            { 2, Char( "\xc0\x00" ) }, // Invalid followup byte (#2 in sequence): 00xxxxxx
            { 2, Char( "\xc0\xc0" ) }, // Invalid followup byte (#2 in sequence): 110xxxxx
            { 7, Char( "\xfe\x80\x80\x80\x80\x80\x80" ) }, // Invalid: 7-byte sequence
            { 5, Char( "\xfc\x80\x80\x80\x80" ) }, // Short sequence
            { 5, Char( "\xf8\x80\x00\x80\x80" ) }, // Invaild followup byte (#3 in sequence)
            { 5, Char( "\xf8\x80\x80\x80\xc0" ) }  // Invalid followup byte (#4/last in sequence)
        };
        cp = 42; // Valid code point
        for( size_t i = 0; i < BFDP_COUNT_OF_ARRAY( invalidBytes ); ++i )
        {
            SCOPED_TRACE( ::testing::Message( "i = " ) << i );

            ASSERT_GE( sizeof( buf ), test[i].numBytes );
            std::memset( buf, 0, sizeof( buf ) );
            std::memcpy( buf, invalidBytes[i].bVal, invalidBytes[i].numBytes );

            ASSERT_EQ( 0U, utf8.ConvertBytes( buf, invalidBytes[i].numBytes, cp ) );
            ASSERT_EQ( 42, cp ); // Unchanged
        }

        // Special case: 6-byte 0 (happens when incorrectly converting 0x8000000 to UTF8)
        // NOTE: This cannot be detected as invalid, since bit 32 has already been lost
        std::memcpy( buf, Char( "\xfc\x80\x80\x80\x80\x80" ), 6 );
        ASSERT_EQ( 6U, utf8.ConvertBytes( buf, 6, cp ) );
        ASSERT_EQ( 0U, cp );

        // Special case: 32 bit data is invalid
        ASSERT_EQ( 0U, utf8.ConvertSymbol( 0x80000000, buf, 6 ) );
        ASSERT_EQ( 0U, utf8.ConvertSymbol( 0xffffffff, buf, 6 ) );

        SetMockErrorHandlers();
        MockErrorHandler::Workspace wksp;

        // Test invalid inputs
        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, utf8.ConvertBytes( NULL, 6, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, utf8.ConvertBytes( buf, 0, cp ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, utf8.ConvertSymbol( cp, NULL, 6U ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, utf8.ConvertSymbol( cp, buf, 5U ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );

        wksp.ExpectMisuseError();
        ASSERT_EQ( 0U, utf8.ConvertSymbol( cp, buf, 0U ) );
        ASSERT_NO_FATAL_FAILURE( wksp.VerifyMisuseError() );
    }

} // namespace BfsdlTests

/**
    BFDP Object NumericValueBuilder Tests

    Copyright 2023, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#include <algorithm>

#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "BfsdlParser/Objects/Common.hpp"
#include "BfsdlParser/Objects/NumericValueBuilder.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::NumericFieldProperties;
    using BfsdlParser::Objects::NumericValueBuilder;

    class ObjectsNumericValueBuilderTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    protected:
        ::testing::AssertionResult ParseData
            (
            NumericFieldProperties const& aProps,
            std::string const& aTestData,
            size_t const aMaxBitSize = 8
            )
        {
            ::testing::AssertionResult result = SetProps( aProps );
            if( !result )
            {
                return result;
            }

            size_t testDataBits = Bfdp::BitManip::BytesToBits( aTestData.length() );
            Bfdp::BitManip::BitBuffer buffer
                (
                reinterpret_cast< Bfdp::Byte const* >( aTestData.c_str() ),
                testDataBits
                );
            size_t actualSize = buffer.GetDataBits();
            if( actualSize != testDataBits )
            {
                return ::testing::AssertionFailure() << "Failed to allocate buffer";
            }
            Bfdp::BitManip::GenericBitStream bitstream( buffer );
            uint64_t readValue;
            Bfdp::Byte* readPtr = reinterpret_cast< Bfdp::Byte* >( &readValue );
            for( size_t procBits = 0; procBits < testDataBits; procBits += aMaxBitSize )
            {
                size_t startPosBits = bitstream.GetPosBits();
                size_t readSizeBits = std::min( aMaxBitSize, mBuilder.GetBitsTillComplete() );
                if( readSizeBits == 0 )
                {
                    return ::testing::AssertionFailure() << "Unexpected builder failure with " << readSizeBits << "b @ " << startPosBits << "b";
                }
                readValue = 0;
                if( !bitstream.ReadBits( readPtr, readSizeBits ) )
                {
                    return ::testing::AssertionFailure() << "Unexpected stream failure with " << readSizeBits << "b @ " << startPosBits << "b";
                }
                if( !mBuilder.ParseBits( readValue, readSizeBits ) )
                {
                    return ::testing::AssertionFailure() << "Parse failure @ " << startPosBits << "b while parsing 0x" << std::hex << readValue << std::dec;
                }
                if( mBuilder.IsComplete() )
                {
                    break;
                }
            }
            if( bitstream.GetPosBits() != testDataBits )
            {
                return ::testing::AssertionFailure() << "Incomplete parse:  " << bitstream.GetPosBits() << "b/" << testDataBits << "b";
            }
            return ::testing::AssertionSuccess();
        }

        ::testing::AssertionResult ParseSignedData
            (
            NumericFieldProperties const& aProps,
            std::string const& aTestData,
            int64_t const aExpectedValue,
            size_t const aMaxBitSize = 8
            )
        {
            ::testing::AssertionResult result = ParseData( aProps, aTestData, aMaxBitSize );
            if( !result )
            {
                return result;
            }

            int64_t parsedValue = mBuilder.GetRawS64();
            if( parsedValue != aExpectedValue )
            {
                int64_t expectedAbs = ( aExpectedValue < 0 ) ? ( aExpectedValue * -1 ) : aExpectedValue;
                char expectedSign = ( aExpectedValue < 0 ) ? '-' : '+';
                int64_t parsedAbs = ( parsedValue < 0 ) ? ( parsedValue * -1 ) : parsedValue;
                char parsedSign = ( parsedValue < 0 ) ? '-' : '+';
                std::stringstream ss;
                ss << "Wrong parse result:" << std::endl
                    << "Expected: " << aExpectedValue << " (" << expectedSign << "0x" << std::hex << expectedAbs << ")" << std::dec << std::endl
                    << "  Actual: " << parsedValue << " (" << parsedSign << "0x" << std::hex << parsedAbs << ")" << std::dec;
                std::string msg = ss.str();
                return ::testing::AssertionFailure() << msg;
            }
            return ::testing::AssertionSuccess();
        }

        ::testing::AssertionResult ParseUnsignedData
            (
            NumericFieldProperties const& aProps,
            std::string const& aTestData,
            uint64_t const aExpectedValue,
            size_t const aMaxBitSize = 8
            )
        {
            ::testing::AssertionResult result = ParseData( aProps, aTestData, aMaxBitSize );
            if( !result )
            {
                return result;
            }

            uint64_t parsedValue = mBuilder.GetRawU64();
            if( parsedValue != aExpectedValue )
            {
                std::stringstream ss;
                ss << "Wrong parse result:" << std::endl
                    << "Expected: " << aExpectedValue << " (" << "0x" << std::hex << aExpectedValue << ")" << std::dec << std::endl
                    << "  Actual: " << parsedValue << " (" << "0x" << std::hex << parsedValue << ")" << std::dec;
                std::string msg = ss.str();
                return ::testing::AssertionFailure() << msg;
            }
            return ::testing::AssertionSuccess();
        }

        ::testing::AssertionResult SetProps
            (
            NumericFieldProperties const& aProps,
            bool const aExpectOk = true
            )
        {
            // Check result of set operation
            bool actualOk = mBuilder.SetFieldProperties( aProps );
            if( aExpectOk != actualOk )
            {
                return ::testing::AssertionFailure()
                    << "SetFieldProperties():" << std::endl
                    << "  Expected: " << aExpectOk << std::endl
                    << "  Actual:   " << actualOk;
            }
            size_t expectedBits = actualOk
                ? aProps.mIntegralBits + aProps.mFractionalBits
                : 0U;
            // Check postcondition: HasProperties() matches return code
            actualOk = mBuilder.HasProperties();
            if( aExpectOk != actualOk )
            {
                return ::testing::AssertionFailure()
                    << "HasProperties():" << std::endl
                    << "  Expected: " << aExpectOk << std::endl
                    << "  Actual:   " << actualOk;
            }
            // Check postcondition: IsComplete() always false
            actualOk = mBuilder.IsComplete();
            if( actualOk )
            {
                return ::testing::AssertionFailure()
                    << "IsComplete():" << std::endl
                    << "  Expected: " << false << std::endl
                    << "  Actual:   " << actualOk;
            }
            size_t actualBits = mBuilder.GetBitsTillComplete();
            if( expectedBits != actualBits )
            {
                return ::testing::AssertionFailure()
                    << "GetBitsTillComplete():" << std::endl
                    << "  Expected: " << expectedBits << std::endl
                    << "  Actual:   " << actualBits;
            }
            return ::testing::AssertionSuccess();
        }

        NumericValueBuilder mBuilder;
    };

    TEST_F( ObjectsNumericValueBuilderTest, ChangeStates )
    {
        // Test default state
        ASSERT_FALSE( mBuilder.IsComplete() );
        ASSERT_FALSE( mBuilder.HasProperties() );
        ASSERT_EQ( 0U, mBuilder.GetBitsTillComplete() );

        // Reset - still empty
        mBuilder.Reset();
        ASSERT_FALSE( mBuilder.IsComplete() );
        ASSERT_FALSE( mBuilder.HasProperties() );
        ASSERT_EQ( 0U, mBuilder.GetBitsTillComplete() );

        // Set properties without parsing - has properties but incomplete
        ASSERT_TRUE( mBuilder.SetFieldProperties( NumericFieldProperties( false, 1U, 1U ) ) );
        ASSERT_FALSE( mBuilder.IsComplete() );
        ASSERT_TRUE( mBuilder.HasProperties() );
        ASSERT_EQ( 2U, mBuilder.GetBitsTillComplete() );

        // Reset again - empty again
        mBuilder.Reset();
        ASSERT_FALSE( mBuilder.IsComplete() );
        ASSERT_FALSE( mBuilder.HasProperties() );
        ASSERT_EQ( 0U, mBuilder.GetBitsTillComplete() );
    }

    TEST_F( ObjectsNumericValueBuilderTest, ParseSF )
    {
        // Parse signed fixed-point numbers
        NumericFieldProperties props( true, 12, 4 );

        ASSERT_TRUE( ParseSignedData( props, "\x4b\xa7", 19367LL ) );
        ASSERT_TRUE( ParseSignedData( props, "\xb4\x59", -19367LL ) ); // 2's compliment of 0x4ba7
        ASSERT_TRUE( ParseSignedData( props, "\xcb\xa7", -13401LL ) );
    }

    TEST_F( ObjectsNumericValueBuilderTest, ParseSI )
    {
        // Parse signed integral numbers
        NumericFieldProperties props( true, 16, 0 );

        ASSERT_TRUE( ParseSignedData( props, "\x4b\xa7", 19367LL ) );
        ASSERT_TRUE( ParseSignedData( props, "\xb4\x59", -19367LL ) ); // 2's compliment of 0x4ba7
        ASSERT_TRUE( ParseSignedData( props, "\xcb\xa7", -13401LL ) );
    }

    TEST_F( ObjectsNumericValueBuilderTest, ParseUF )
    {
        // Parse unsigned fixed-point numbers
        NumericFieldProperties props( false, 12, 4 );

        ASSERT_TRUE( ParseUnsignedData( props, "\x4b\xa7", 19367LL ) );
        ASSERT_TRUE( ParseUnsignedData( props, "\xb4\x59", 46169ULL ) ); // 2's compliment of 0x4ba7
        ASSERT_TRUE( ParseUnsignedData( props, "\xcb\xa7", 52135ULL ) );
    }

    TEST_F( ObjectsNumericValueBuilderTest, ParseUI )
    {
        // Parse unsigned integral numbers
        NumericFieldProperties props( false, 16, 0 );

        ASSERT_TRUE( ParseUnsignedData( props, "\x4b\xa7", 19367ULL ) );
        ASSERT_TRUE( ParseUnsignedData( props, "\xb4\x59", 46169ULL ) ); // 2's compliment of 0x4ba7
        ASSERT_TRUE( ParseUnsignedData( props, "\xcb\xa7", 52135ULL ) );
    }

    TEST_F( ObjectsNumericValueBuilderTest, PropertyValidity )
    {
        ASSERT_TRUE( SetProps( NumericFieldProperties( false, 1U, 0U ), true ) )
            << "u1.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( false, 0U, 1U ), true ) )
            << "u0.1";
        ASSERT_TRUE( SetProps( NumericFieldProperties( false, 0U, 0U ), false ) )
            << "u0.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( false, 64U, 0U ), true ) )
            << "u64.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( false, 0U, 64U ), true ) )
            << "u0.64";

        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 1U, 0U ), false ) )
            << "s1.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 2U, 0U ), true ) )
            << "s2.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 1U, 1U ), true ) )
            << "s1.1";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 0U, 0U ), false ) )
            << "s0.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 64U, 0U ), true ) )
            << "s64.0";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 0U, 64U ), false ) )
            << "s0.64";
        ASSERT_TRUE( SetProps( NumericFieldProperties( true, 1U, 63U ), true ) )
            << "s1.63";
    }

} // namespace BfsdlTests

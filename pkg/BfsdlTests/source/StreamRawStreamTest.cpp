/**
    Tests for BFDP Stream RawStream

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

#include "Bfdp/Stream/RawStream.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/MockStreamObserver.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{
    using namespace Bfdp;

    using Stream::Control;
    using Stream::RawStream;
    using Stream::StreamPtr;

    class StreamRawStreamTest
        : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

        void TearDown()
        {
            // Deconstruct stream
            mStream.reset();
            mInputStdStream.str( std::string() );
            mInputData = std::string();
            ASSERT_TRUE( mOutput.VerifyNone() );
        }

        ::testing::AssertionResult CreateRawStream
            (
            std::string const& aInitialData,
            bool const aExpectValid = true
            )
        {
            mInputData = aInitialData;
            mInputStdStream.str( mInputData );
            mStream = std::make_shared< RawStream >( "RawTest", mInputStdStream, mOutput );
            if( !mStream )
            {
                return ::testing::AssertionFailure() << "Failed to create raw stream";
            }
            bool actualValid = mStream->IsValid();
            if( aExpectValid != actualValid )
            {
                return ::testing::AssertionFailure() << "Validity postcondition failure:" << std::endl
                    << "Expected: " << aExpectValid << std::endl
                    << "Actual:   " << actualValid;
            }
            return ::testing::AssertionSuccess();
        }

        MockStreamObserver mOutput;
        StreamPtr mStream;

    private:
        std::string mInputData;
        std::stringstream mInputStdStream;
    };

    TEST_F( StreamRawStreamTest, ParseData1 )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "\xab\xcd" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        mOutput.DoReadUint( 16 );
        mOutput.DoEndOfStream();

        // Reading completes without error
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        // Data will be read as bits of a number.
        //
        // The default interpretation is ByteOrder::LE and BitOrder::LE; to
        // illustrate this in terms of the position of the bit in the bit
        // stream, 2 bytes will have these positions:
        //
        // ----- LSB -----   -------- MSB --------
        // MSb         LSb   MSb               LSb
        // 7,6,5,4,3,2,1,0   15,14,13,12,11,10,9,8
        //
        // When reading these into a 16-bit number, we will interpret as the
        // computer's native representation of a number with data organized
        // as follows:
        //
        // ------------ UINT 16 ----------------
        // MSb                               LSb
        // 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
        //
        // e.g., Reading 16 bits from \xab\xcd would result in 0xcdab as one
        // would normally expect on most little-endian systems.

        ASSERT_TRUE( mOutput.VerifyNext( "Read U16: 0xcdab" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "EndOfStream" ) );
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_FALSE( mStream->HasError() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 2U, mStream->GetTotalProcessedBytes() );
    }

    TEST_F( StreamRawStreamTest, ParseData2 )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "\xab\xcd" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        mOutput.DoReadUint( 10 );
        mOutput.DoReadUint( 6 );
        mOutput.DoEndOfStream();

        // Reading completes without error
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        // Data will be read as bits of a number.
        //
        // See comments in ParseData1 for the foundational concepts.
        //
        // This becomes more challenging conceptually when there are partial
        // bits left over in a byte.  So with 2 numbers read in order
        // identified as:
        //
        // A = 10 bit number
        // B = 6 bit number
        //
        // The following positional layout is observed for the input bits:
        //
        // ----- LSB -----   -------- MSB --------
        // MSb         LSb   MSb               LSb
        // 7,6,5,4,3,2,1,0   15,14,13,12,11,10,9,8
        //
        // This translates to reading the number A consuming the first 10
        // bits (positions 0 through 9), and B consuming the next 6 bits
        // (positions 10 through 15).
        //
        // -------- A --------
        // MSb             LSb
        // 9,8,7,6,5,4,3,2,1,0
        //
        // ------- B -------
        // MSb           LSb
        // 15,14,13,12,11,10
        ASSERT_TRUE( mOutput.VerifyNext( "Read U10: 0x1ab" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "Read U6: 0x33" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "EndOfStream" ) );
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_FALSE( mStream->HasError() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 2U, mStream->GetTotalProcessedBytes() );
    }

    TEST_F( StreamRawStreamTest, ParseData3 )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "\xab\xcd\xef" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        mOutput.DoReadUint( 4 );
        mOutput.DoReadUint( 10 );
        mOutput.DoReadUint( 3 );
        mOutput.DoReadUint( 5 );
        // Stop early and leave the last 2 bits unread
        mOutput.DoReturn( Control::Stop );

        // Reading completes without error
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        // Data will be read as bits of a number.
        //
        // See comments in ParseData1 and ParseData2 to understand how these
        // values are produced.
        ASSERT_TRUE( mOutput.VerifyNext( "Read U4: 0xb" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "Read U10: 0xda" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "Read U3: 0x7" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "Read U5: 0x17" ) );
        ASSERT_TRUE( mOutput.VerifyNext( "Return Stop" ) );
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_FALSE( mStream->HasError() );
        ASSERT_EQ( 6U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 2U, mStream->GetTotalProcessedBytes() );
    }

    TEST_F( StreamRawStreamTest, ParseNoData )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        // Reading completes without error
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_FALSE( mStream->HasError() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBytes() );
    }

    TEST_F( StreamRawStreamTest, ParseStop )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "\xab\xcd\xef" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        mOutput.DoReturn( Control::Stop );

        // Reading completes without error, but no data
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        ASSERT_TRUE( mOutput.VerifyNext( "Return Stop" ) );
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_FALSE( mStream->HasError() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBytes() );
    }

    TEST_F( StreamRawStreamTest, ParseWithoutReadingData )
    {
        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWorkspace;

        // Create a stream and test its validity.

        ASSERT_TRUE( CreateRawStream( "\xab\xcd\xef" ) );

        // Check pre-parse state
        ASSERT_TRUE( mStream->IsValid() );
        ASSERT_FALSE( mStream->HasError() );

        // Tell the parser to continue twice in a row without reading any data
        // so it will stop automatically to prevent an infinite loop.
        mOutput.DoReturn( Control::Continue );

        // Reading starts, but produces an error
        errWorkspace.ExpectRunTimeError();
        ASSERT_TRUE( mStream->ReadStream() );

        // Check log first, which will show the most relevant data for failures.
        ASSERT_TRUE( mOutput.VerifyNext( "Return Continue" ) );
        ASSERT_TRUE( mOutput.VerifyNone() );

        // Check postconditions
        ASSERT_TRUE( mStream->HasError() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBits() );
        ASSERT_EQ( 0U, mStream->GetTotalProcessedBytes() );

    }

} // namespace BfsdlTests

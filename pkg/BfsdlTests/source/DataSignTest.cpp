/**
    BFDP Data Sign Test

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

#include "gtest/gtest.h"

#include "Bfdp/Data/Sign.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    class DataSignTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    using Bfdp::Data::Sign;

    TEST_F( DataSignTest, Compare )
    {
        ASSERT_TRUE( Sign::Positive == Sign( Sign::Positive ) );
        ASSERT_FALSE( Sign::Negative == Sign( Sign::Positive ) );

        ASSERT_TRUE( Sign::Negative != Sign( Sign::Positive ) );
        ASSERT_FALSE( Sign::Positive != Sign( Sign::Positive ) );
    }

    TEST_F( DataSignTest, ConstructWithValue )
    {
        Sign unspecifiedSign( Sign::Unspecified );
        ASSERT_EQ( Sign::Unspecified, unspecifiedSign );
        ASSERT_FALSE( unspecifiedSign.IsSpecified() );
        ASSERT_STREQ( "?", unspecifiedSign.GetExplicitStr() );
        ASSERT_STREQ( "", unspecifiedSign.GetStr() );

        Sign posSign( Sign::Positive );
        ASSERT_EQ( Sign::Positive, posSign );
        ASSERT_TRUE( posSign.IsSpecified() );
        ASSERT_STREQ( "+", posSign.GetExplicitStr() );
        ASSERT_STREQ( "", posSign.GetStr() );

        Sign negativeSign( Sign::Negative );
        ASSERT_EQ( Sign::Negative, negativeSign );
        ASSERT_TRUE( negativeSign.IsSpecified() );
        ASSERT_STREQ( "-", negativeSign.GetExplicitStr() );
        ASSERT_STREQ( "-", negativeSign.GetStr() );
    }

    TEST_F( DataSignTest, DefaultState )
    {
        Sign defaultSign;

        ASSERT_EQ( Sign::Unspecified, defaultSign );
        ASSERT_FALSE( defaultSign.IsSpecified() );
        ASSERT_STREQ( "?", defaultSign.GetExplicitStr() );
        ASSERT_STREQ( "", defaultSign.GetStr() );
    }

} // namespace BfsdlTests

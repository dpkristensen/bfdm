/**
    BFDP BitManip Buffer Test

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

#include "gtest/gtest.h"

#include "BfsdlParser/Data/SignType.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    class SignTypeTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    using BfsdlParser::Data::SignType;

    TEST_F( SignTypeTest, Compare )
    {
        ASSERT_TRUE( SignType::Positive == SignType( SignType::Positive ) );
        ASSERT_FALSE( SignType::Negative == SignType( SignType::Positive ) );

        ASSERT_TRUE( SignType::Negative != SignType( SignType::Positive ) );
        ASSERT_FALSE( SignType::Positive != SignType( SignType::Positive ) );
    }

    TEST_F( SignTypeTest, ConstructWithValue )
    {
        SignType unspecifiedSign( SignType::Unspecified );
        ASSERT_EQ( SignType::Unspecified, unspecifiedSign );
        ASSERT_FALSE( unspecifiedSign.IsSpecified() );

        SignType posSign( SignType::Positive );
        ASSERT_EQ( SignType::Positive, posSign );
        ASSERT_TRUE( posSign.IsSpecified() );

        SignType negativeSign( SignType::Negative );
        ASSERT_EQ( SignType::Negative, negativeSign );
        ASSERT_TRUE( negativeSign.IsSpecified() );
    }

    TEST_F( SignTypeTest, DefaultState )
    {
        SignType defaultSign;

        ASSERT_EQ( SignType::Unspecified, defaultSign );
        ASSERT_FALSE( defaultSign.IsSpecified() );
    }

} // namespace BfsdlTests

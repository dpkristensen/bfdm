/**
    BFDP Data FlexNumber Test

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

#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/Data/FlexNumber.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    class DataFlexNumberTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    using namespace Bfdp;
    using Data::FlexNumber;
    using Data::Sign;

    TEST_F( DataFlexNumberTest, ComponentStates )
    {
        static struct TestDataType
        {
            Sign::Value sign;
            char const* integral;
            char const* fractional;
            bool defined;
            char const* str;
        } const testData[] =
        {
            // Variations of sign
            { Sign::Unspecified, NULL, NULL, false, "?0" },
            { Sign::Positive, NULL, NULL, false, "+0" },
            { Sign::Negative, NULL, NULL, false, "-0" },

            // Variations of integral values
            { Sign::Unspecified, "123", NULL, false, "?123" },
            { Sign::Positive, "123", NULL, true, "+123" },

            // Variations of fractional values
            { Sign::Unspecified, NULL, "456", false, "?0.456" },
            { Sign::Negative, NULL, "456", true, "-0.456" },

            // Variations of combined values
            { Sign::Unspecified, "123", "456", false, "?123.456" },
            { Sign::Positive, "123", "456", true, "+123.456" },
            { Sign::Negative, "123", "456", true, "-123.456" }
        };
        static SizeT const testCount = BFDP_COUNT_OF_ARRAY( testData );

        for( SizeT i = 0; i < testCount; ++i )
        {
            SCOPED_TRACE
                (
                ::testing::Message( "[" ) << i << "]"
                    << " s=" << Sign( testData[i].sign ).GetExplicitStr()
                    << " i=" << StrOrNull( testData[i].integral )
                    << " f=" << StrOrNull( testData[i].fractional )
                );

            FlexNumber::Component c;
            c.sign = testData[i].sign;
            if( testData[i].integral )
            {
                c.integral.Set( testData[i].integral, 10 );
            }
            if( testData[i].fractional )
            {
                c.fractional.Set( testData[i].fractional, 10 );
            }
            ASSERT_EQ( testData[i].defined, c.IsDefined() );
            ASSERT_STREQ( testData[i].str, c.GetStr( true ).c_str() );
        }

    }

    TEST_F( DataFlexNumberTest, DefaultState )
    {
        FlexNumber defaultNumber;
        ASSERT_FALSE( defaultNumber.IsDefined() );
        ASSERT_STREQ( "", defaultNumber.GetStr().c_str() );
        ASSERT_STREQ( "", defaultNumber.GetStr( true ).c_str() );
    }

} // namespace BfsdlTests

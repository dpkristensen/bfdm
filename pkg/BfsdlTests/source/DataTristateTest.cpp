/**
    BFDP Data Tristate Test

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

// External includes
#include <cctype>
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/Data/Tristate.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using Bfdp::Data::Tristate;

    class DataTristateTest
        : public ::testing::Test
    {
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( DataTristateTest, CreateDefault )
    {
        Tristate def;

        ASSERT_TRUE( Tristate::Unset == def ); // Operator == comparison to enum
        ASSERT_FALSE( def.IsSet() );
        ASSERT_FALSE( def.IsFalse() );
        ASSERT_FALSE( def.IsTrue() );
    }

    TEST_F( DataTristateTest, CreateWithValue )
    {
        Tristate obj = Tristate( true );

        ASSERT_TRUE( Tristate::True == obj );
        ASSERT_TRUE( Tristate::False != obj );
        ASSERT_EQ( Tristate::True, obj );
        ASSERT_TRUE( obj.IsSet() );
        ASSERT_FALSE( obj.IsFalse() );
        ASSERT_TRUE( obj.IsTrue() );
    }

    TEST_F( DataTristateTest, OperatorAssignBool )
    {
        Tristate obj;

        obj = false;
        ASSERT_TRUE( Tristate::False == obj );
        ASSERT_TRUE( Tristate::True != obj );
        ASSERT_EQ( Tristate::False, obj );
        ASSERT_TRUE( obj.IsSet() );
        ASSERT_TRUE( obj.IsFalse() );
        ASSERT_FALSE( obj.IsTrue() );
    }

    TEST_F( DataTristateTest, Reset )
    {
        Tristate obj = Tristate( true );

        obj.Reset();
        ASSERT_TRUE( Tristate::True != obj );
        ASSERT_TRUE( Tristate::False != obj );
        ASSERT_EQ( Tristate::Unset, obj );
        ASSERT_FALSE( obj.IsSet() );
        ASSERT_FALSE( obj.IsFalse() );
        ASSERT_FALSE( obj.IsTrue() );
    }

} // namespace BfsdlTests

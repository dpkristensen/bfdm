/**
    BFDP UniquePtr Test

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

#include "Bfdp/UniquePtr.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using Bfdp::UniquePtr;

    class UniquePtrTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        struct ValueType
        {
            ValueType
                (
                int const v = 0
                )
                : a( v )
            {
            }

            int a;
        };
    };

    TEST_F( UniquePtrTest, AssignPtr )
    {
        UniquePtr< ValueType > obj1( new ValueType( 42 ) );

        ASSERT_TRUE( NULL != obj1.Get() );

        UniquePtr< ValueType > obj2;
        obj2 = obj1;

        ASSERT_TRUE( NULL == obj1.Get() );
        ASSERT_TRUE( NULL != obj2.Get() );
        ASSERT_EQ( 42, obj2->a );
    }

    TEST_F( UniquePtrTest, AssignRaw )
    {
        UniquePtr< ValueType > obj1;
        obj1 = new ValueType( 111 );
        ASSERT_TRUE( NULL != obj1.Get() );
        ASSERT_EQ( 111, obj1->a );
    }

    TEST_F( UniquePtrTest, CreateConstructor )
    {
        ValueType* v = new ValueType();
        UniquePtr< ValueType > obj( v );

        ASSERT_EQ( v, obj.Get() );

        // Test pointer access
        v->a = 1;
        ASSERT_EQ( 1, obj->a );
        v->a = 2;
        ASSERT_EQ( 2, (*obj).a );
        v->a = 3;
        ASSERT_EQ( 3, obj[0].a );

        obj.Release();
        ASSERT_TRUE( NULL == obj.Get() );
    }

    TEST_F( UniquePtrTest, CreateCopy )
    {
        UniquePtr< ValueType > obj1( new ValueType() );

        ASSERT_TRUE( NULL != obj1.Get() );
        obj1->a = 42;

        UniquePtr< ValueType > obj2( obj1 );

        ASSERT_TRUE( NULL == obj1.Get() );
        ASSERT_TRUE( NULL != obj2.Get() );
        ASSERT_EQ( 42, obj2->a );
        ASSERT_EQ( 42, (*obj2).a );
        ASSERT_EQ( 42, obj2[0].a );
    }

    TEST_F( UniquePtrTest, CreateDefault )
    {
        UniquePtr< ValueType > obj;

        ASSERT_TRUE( NULL == obj.Get() );
        obj.Release();
        ASSERT_TRUE( NULL == obj.Get() );
    }


} // namespace BfsdlTests

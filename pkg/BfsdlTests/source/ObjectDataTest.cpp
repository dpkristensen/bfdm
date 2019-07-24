/**
    BFDP Object Data Tests

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

#include "BfsdlParser/Objects/Field.hpp"
#include "BfsdlParser/Objects/Tree.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::Field;
    using BfsdlParser::Objects::IObject;
    using BfsdlParser::Objects::ObjectType;
    using BfsdlParser::Objects::Tree;

    class ObjectDataTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( ObjectDataTest, Field )
    {
        Field::UFieldPtr fp = Field::Create( "test" );

        ASSERT_TRUE( fp != NULL );
        ASSERT_EQ( ObjectType::Field, fp->GetType() );
        ASSERT_STREQ( "test", fp->GetName().c_str() );
        ASSERT_STREQ( "test", fp->GetId().GetStr().c_str() );

        IObject::UPtr op = std::move( fp );
        Field* f = Field::Get( op );
        ASSERT_TRUE( f != NULL );
        ASSERT_STREQ( "test", f->GetName().c_str() );
    }

    TEST_F( ObjectDataTest, Tree )
    {
        Tree tree;
        ASSERT_EQ( ObjectType::Tree, tree.GetType() );
        ASSERT_STREQ( "", tree.GetName().c_str() );

        IObject* op = tree.Add( Field::Create( "One" ) );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "One", op->GetName().c_str() );

        op = tree.Add( Field::Create( "Two" ) );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "Two", op->GetName().c_str() );

        op = tree.Find( "doesNotExist" );
        ASSERT_TRUE( op == NULL );

        op = tree.Find( "Two" );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "Two", op->GetName().c_str() );

        op = tree.Find( "One" );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "One", op->GetName().c_str() );

        op = tree.Find( "Two" );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "Two", op->GetName().c_str() );
    }

} // namespace BfsdlTests

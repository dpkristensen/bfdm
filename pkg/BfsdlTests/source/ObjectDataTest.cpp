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

#include "BfsdlParser/Objects/NumericField.hpp"
#include "BfsdlParser/Objects/StringField.hpp"
#include "BfsdlParser/Objects/Tree.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::Field;
    using BfsdlParser::Objects::FieldPtr;
    using BfsdlParser::Objects::FieldType;
    using BfsdlParser::Objects::IObject;
    using BfsdlParser::Objects::IObjectPtr;
    using BfsdlParser::Objects::NumericField;
    using BfsdlParser::Objects::NumericFieldPtr;
    using BfsdlParser::Objects::ObjectType;
    using BfsdlParser::Objects::StringField;
    using BfsdlParser::Objects::StringFieldPtr;
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

    TEST_F( ObjectDataTest, NumericField )
    {
        IObjectPtr op = std::make_shared< NumericField >( "test" );

        ASSERT_TRUE( op != NULL );
        ASSERT_EQ( ObjectType::Field, op->GetType() );
        ASSERT_STREQ( "test", op->GetName().c_str() );
        ASSERT_STREQ( "test", op->GetId().GetStr().c_str() );

        FieldPtr fp = Field::StaticCast( op );
        ASSERT_TRUE( fp != NULL );
        ASSERT_STREQ( "test", fp->GetName().c_str() );
        ASSERT_STREQ( "???", fp->GetTypeStr().c_str() );
        ASSERT_EQ( FieldType::Numeric, fp->GetFieldType() );

        NumericFieldPtr nfp = NumericField::StaticCast( op );
        ASSERT_TRUE( nfp != NULL );
        ASSERT_STREQ( "test", nfp->GetName().c_str() );
        ASSERT_STREQ( "???", nfp->GetTypeStr().c_str() );
        ASSERT_EQ( FieldType::Numeric, nfp->GetFieldType() );
    }

    TEST_F( ObjectDataTest, StringField )
    {
        IObjectPtr op = std::make_shared< StringField >( "test" );

        ASSERT_TRUE( op != NULL );
        ASSERT_EQ( ObjectType::Field, op->GetType() );
        ASSERT_STREQ( "test", op->GetName().c_str() );
        ASSERT_STREQ( "test", op->GetId().GetStr().c_str() );

        FieldPtr fp = Field::StaticCast( op );
        ASSERT_TRUE( fp != NULL );
        ASSERT_STREQ( "test", fp->GetName().c_str() );
        ASSERT_STREQ( "???", fp->GetTypeStr().c_str() );
        ASSERT_EQ( FieldType::String, fp->GetFieldType() );

        StringFieldPtr sfp = StringField::StaticCast( op );
        ASSERT_TRUE( sfp != NULL );
        ASSERT_STREQ( "test", sfp->GetName().c_str() );
        ASSERT_STREQ( "???", sfp->GetTypeStr().c_str() );
        ASSERT_EQ( FieldType::String, sfp->GetFieldType() );
    }

    TEST_F( ObjectDataTest, Tree )
    {
        Tree tree;
        ASSERT_EQ( ObjectType::Tree, tree.GetType() );
        ASSERT_STREQ( "", tree.GetName().c_str() );

        IObjectPtr op = tree.Add( std::make_shared< NumericField >( "One" ) );
        ASSERT_TRUE( op != NULL );
        ASSERT_STREQ( "One", op->GetName().c_str() );

        op = tree.Add( std::make_shared< NumericField >( "Two" ) );
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

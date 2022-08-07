/**
    BFDP Object Database Tests

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

// Base Includes
#include "gtest/gtest.h"

// External Includes
#include <list>
#include <string>

// Internal Includes
#include "BfsdlParser/Objects/Database.hpp"
#include "BfsdlParser/Objects/NumericField.hpp"
#include "BfsdlParser/Objects/StringProperty.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::Database;
    using BfsdlParser::Objects::Field;
    using BfsdlParser::Objects::FieldPtr;
    using BfsdlParser::Objects::IObject;
    using BfsdlParser::Objects::IObjectPtr;
    using BfsdlParser::Objects::NumericField;
    using BfsdlParser::Objects::NumericFieldProperties;
    using BfsdlParser::Objects::NumericFieldPtr;
    using BfsdlParser::Objects::StringProperty;
    using BfsdlParser::Objects::StringPropertyPtr;

    class ObjectDatabaseTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    namespace BfsdlTestsInternal
    {
        typedef std::list< std::string > TestItemList;

        void TestCb
            (
            IObjectPtr& aObject,
            void* const aArg
            )
        {
            TestItemList* list = reinterpret_cast< TestItemList* >( aArg );
            FieldPtr fp = Field::StaticCast( aObject );
            StringPropertyPtr pp = StringProperty::StaticCast( aObject );
            std::stringstream ss;
            if( fp != NULL )
            {
                ss << fp->GetName() << ":" << fp->GetTypeStr();
            }
            else if( pp != NULL )
            {
                ss << "." << pp->GetName() << "=" << pp->GetValue();
            }
            else
            {
                ss << aObject->GetName();
            }
            list->push_back( ss.str() );
        }
    }
    using namespace BfsdlTestsInternal;

    TEST_F( ObjectDatabaseTest, Add )
    {
        char const* ExpectedData[] =
        {
            ".p1=abc",
            "f1:u8",
            "f2:s8.8"
        };

        Database db;
        Database::Handle hOut = Database::InvalidHandle;

        IObjectPtr fp = std::make_shared< NumericField >( "f1", NumericFieldProperties( false, 8, 0 ) );
        ASSERT_TRUE( db.Add( fp, db.GetRoot(), &hOut ) );
        ASSERT_EQ( Database::InvalidHandle, hOut );

        fp = std::make_shared< NumericField >( "f2", NumericFieldProperties( true, 8, 8 ) );
        ASSERT_TRUE( db.Add( fp, db.GetRoot() ) );

        IObjectPtr pp = std::make_shared< StringProperty >( "p1" );
        ASSERT_TRUE( pp != NULL );
        StringProperty::StaticCast( pp )->SetValueUtf8( "abc" );
        ASSERT_TRUE( db.Add( pp, db.GetRoot(), &hOut ) );
        ASSERT_EQ( Database::InvalidHandle, hOut );

        TestItemList out;
        db.Iterate( TestCb, &out );
        out.sort();

        ASSERT_TRUE( StrListsMatch( ExpectedData, out ) );
    }

} // namespace BfsdlTests

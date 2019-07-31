/**
    BFDP Object StringFieldBuilder Tests

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

#include "BfsdlParser/Objects/Common.hpp"
#include "BfsdlParser/Objects/StringFieldBuilder.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using BfsdlParser::Objects::AttributeParseResult;
    using BfsdlParser::Objects::BitBase;
    using BfsdlParser::Objects::NumericFieldProperties;
    using BfsdlParser::Objects::NumericLiteral;
    using BfsdlParser::Objects::StringField;
    using BfsdlParser::Objects::StringFieldBuilder;
    using BfsdlParser::Objects::StringFieldPtr;

    using ::testing::AssertionFailure;
    using ::testing::AssertionResult;
    using ::testing::AssertionSuccess;
    using ::testing::Message;

    class ObjectStringFieldBuilderTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        typedef AttributeParseResult APR;

        AssertionResult VerifyIdent
            (
            StringFieldBuilder& aBuilder,
            bool const aExpectedResult,
            char const* aIdent
            )
        {
            AssertionResult verifyResult = AssertionSuccess();

            bool parseResult = aBuilder.ParseIdentifier( aIdent );
            if( aExpectedResult )
            {
                verifyResult = AssertionResult( parseResult )
                    << "Ident '" << StrOrNull( aIdent ) << "' failed";
            }

            return verifyResult;
        }

        AssertionResult VerifyFinalize
            (
            StringFieldBuilder& aBuilder,
            bool const aExpectedResult,
            char const* aOutStr
            )
        {
            if( aExpectedResult )
            {
                if( !aBuilder.Finalize() )
                {
                    return AssertionFailure() << "Finalize() failed!";
                }
            }

            StringFieldPtr f = aBuilder.GetField( "test" );
            if( aExpectedResult )
            {
                BFDP_RETURNIF_V( ( NULL == f ), AssertionFailure() << "GetField() == NULL" );
                std::string str = f->GetName();
                BFDP_RETURNIF_V( str != "test", AssertionFailure() << "GetName() == " << str );
                str = f->GetTypeStr();
                BFDP_RETURNIF_V
                    (
                    str != aOutStr,
                    AssertionFailure() << "GetTypeStr() failed:" << std::endl
                        << "    Returned: " << str << std::endl
                        << "    Expected: " << aOutStr;
                    );
            }
            else
            {
                BFDP_RETURNIF_V( ( NULL != f ), AssertionFailure() << "GetField() != NULL" );
            }

            return AssertionSuccess();
        }
    };

    TEST_F( ObjectStringFieldBuilderTest, ParseIdents )
    {
        static struct TestDataType
        {
            char const* inIdent;
            char const* outStr;
        } const sTestData[] =
        {
            // Identifier   Output
            { "string",     "string:b:t0;ascii" },
            { "cstring",    "string:b:t0;ascii" },
            { "pstring",    "string:p8:t0:tu;ascii" },
            { "strin",      NULL },
            { "String",     NULL },
            { "strings",    NULL },
            { "sstring",    NULL },
            { "ccstring",   NULL },
            { "cpstring",   NULL },
            { "ppstring",   NULL },
            { "pcstring",   NULL },
            { "bstring",    NULL },
        };
        static size_t const sNumTests = BFDP_COUNT_OF_ARRAY( sTestData );

        StringFieldBuilder builder;
        for( size_t i = 0; i < sNumTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "ident=" ) << sTestData[i].inIdent );

            // Reset between iterations
            builder.Reset();

            bool expectedResult = ( sTestData[i].outStr != NULL );
            ASSERT_TRUE( NULL == builder.GetField( "test" ) );

            ASSERT_TRUE( VerifyIdent( builder, expectedResult, sTestData[i].inIdent ) );
            ASSERT_TRUE( VerifyFinalize( builder, expectedResult, sTestData[i].outStr ) );
        }
    }

    TEST_F( ObjectStringFieldBuilderTest, ParseStringAttributes )
    {
        static struct TestDataType
        {
            char const* inIdent;
            char const* inAttrName;
            char const* inAttrValue;
            APR::Type outAttrResult;
            char const* outStr;
        } const sTestData[] =
        {
            // Identifier   Attribute   Value       Result                  Output
            { "string",     "code",     "",         APR::InvalidArgument,   NULL },
            { "string",     "code",     "FOO",      APR::InvalidArgument,   NULL },
            { "string",     "code",     "ASCII",    APR::Success,           "string:b:t0;ascii" },
            { "cstring",    "code",     "MS-1252",  APR::Success,           "string:b:t0;ms1252" },
            { "pstring",    "code",     "UTF8",     APR::Success,           "string:p8:t0:tu;utf8" },

            // Identifier   Attribute   Value       Result                  Output
            { "string",     "len",      "",         APR::Unsupported,       NULL },
            { "string",     "plen",     "",         APR::Unsupported,       NULL },
            { "string",     "term",     "",         APR::Unsupported,       NULL },

            // Identifier   Attribute   Value       Result                  Output
            { "string",     "unterm",   "",         APR::Success,           "string:b:t0:tu;ascii" },
            { "string",     "unterm",   "1",        APR::InvalidArgument,   NULL },
            { "string",     "unterm",   "true",     APR::InvalidArgument,   NULL },
            { "cstring",    "unterm",   "",         APR::Success,           "string:b:t0:tu;ascii" },
            { "pstring",    "unterm",   "",         APR::Redefinition,      NULL },
        };
        static size_t const sNumTests = BFDP_COUNT_OF_ARRAY( sTestData );

        SetMockErrorHandlers();
        MockErrorHandler::Workspace errWksp;

        StringFieldBuilder builder;
        for( size_t i = 0; i < sNumTests; ++i )
        {
            SCOPED_TRACE( ::testing::Message( "value=" ) << sTestData[i].inAttrValue );
            SCOPED_TRACE( ::testing::Message( "attr=" ) << sTestData[i].inAttrName );
            SCOPED_TRACE( ::testing::Message( "ident=" ) << sTestData[i].inIdent );

            // Reset between iterations
            builder.Reset();

            bool expectedResult = ( sTestData[i].outStr != NULL );
            ASSERT_TRUE( NULL == builder.GetField( "test" ) );

            ASSERT_TRUE( VerifyIdent( builder, expectedResult, sTestData[i].inIdent ) );

            APR::Type attrResult = builder.ParseStringAttribute( sTestData[i].inAttrName, sTestData[i].inAttrValue );
            ASSERT_EQ( sTestData[i].outAttrResult, attrResult );
            ASSERT_TRUE( VerifyFinalize( builder, expectedResult, sTestData[i].outStr ) );

            // Check for redefinition AFTER finalizing, since it causes Finalize() to fail
            if( attrResult == APR::Success )
            {
                ASSERT_EQ( APR::Redefinition, builder.ParseStringAttribute( sTestData[i].inAttrName, sTestData[i].inAttrValue ) );
                ASSERT_NE( APR::Success, builder.ParseStringAttribute( sTestData[i].inAttrName, "SOMETHING INVALID" ) );
                errWksp.ExpectInternalError();
                ASSERT_FALSE( builder.Finalize() );
                ASSERT_NO_FATAL_FAILURE( errWksp.VerifyInternalError() );
            }
        }
    }

} // namespace BfsdlTests

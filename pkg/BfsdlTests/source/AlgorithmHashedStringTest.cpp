/**
    BFDP Algorithm HashedString Tests

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

#include <map>

#include "Bfdp/Algorithm/HashedString.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using Bfdp::Algorithm::HashedString;

    class AlgorithmHashedStringTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    typedef std::map< HashedString, int > TestMap;

    TEST_F( AlgorithmHashedStringTest, CreateEqual )
    {
        HashedString hs1 = HashedString( "abc" );
        HashedString hs2 = HashedString( "abc" );

        ASSERT_TRUE( hs1 == hs2 );
        ASSERT_FALSE( hs1 != hs2 );
        ASSERT_EQ( hs1.GetHash(), hs2.GetHash() );

        ASSERT_STREQ( hs1.GetStr().c_str(), "abc" );
        ASSERT_STREQ( hs2.GetStr().c_str(), "abc" );
    }

    TEST_F( AlgorithmHashedStringTest, CreateUnequal )
    {
        HashedString hs1 = HashedString( "Foo" );
        HashedString hs2 = HashedString( "Baz" );

        ASSERT_FALSE( hs1 == hs2 );
        ASSERT_TRUE( hs1 != hs2 );
        ASSERT_NE( hs1.GetHash(), hs2.GetHash() );

        ASSERT_STREQ( hs1.GetStr().c_str(), "Foo" );
        ASSERT_STREQ( hs2.GetStr().c_str(), "Baz" );
    }

    TEST_F( AlgorithmHashedStringTest, VerifyStrictWeakOrder )
    {
        HashedString hsFoo1 = HashedString( "Foo" );
        HashedString hsFoo2 = HashedString( "Foo" );
        HashedString hsBaz = HashedString( "Baz" );

        // Preconditions for this to be a valid test for the implemented algorithm
        ASSERT_EQ( hsFoo1.GetHash(), hsFoo2.GetHash() );
        ASSERT_NE( hsFoo1.GetHash(), hsBaz.GetHash() );

        // Since the relationship is canonically undefined, determine what to expect first.
        bool isFooLessThanBaz = hsFoo1.IsStrictWeakLessThan( hsBaz );

        // Compare for same hash
        ASSERT_EQ( isFooLessThanBaz, hsFoo2.IsStrictWeakLessThan( hsBaz ) );
        ASSERT_NE( isFooLessThanBaz, hsBaz.IsStrictWeakLessThan( hsFoo2 ) );

        // Symmetry
        ASSERT_NE( isFooLessThanBaz, hsBaz.IsStrictWeakLessThan( hsFoo1 ) );

        // Reflexivity / Equality
        ASSERT_FALSE( hsFoo1.IsStrictWeakLessThan( hsFoo2 ) );
        ASSERT_FALSE( hsFoo2.IsStrictWeakLessThan( hsFoo1 ) );
    }

} // namespace BfsdlTests

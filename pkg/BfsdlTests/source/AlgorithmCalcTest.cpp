/**
    BFDP Algorithm Calculation Tests

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

#include "Bfdp/Algorithm/Calc.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using Bfdp::Algorithm::HashType;
    using Bfdp::Algorithm::FastHash;

    class AlgorithmCalcTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( AlgorithmCalcTest, FastHash )
    {
        HashType const testHashes[] =
        {
            FastHash( NULL, 0 ),
            FastHash( "hello" ),
            FastHash( "Hello" ), // Test for case sensitivity
            FastHash( "olleh" ), // Test for ordering
            FastHash( "heljp" ), // Test for simple checksums (same as "hello")
        };
        size_t const numTests = BFDP_COUNT_OF_ARRAY( testHashes );

        // Verify NULL pointer is the same as 0-length string
        GTEST_ASSERT_EQ( testHashes[0], FastHash( std::string() ) );

        // Iterate through all permutations and verify each hash is unique
        //
        // While there are no guarantees that these are actually different, any algorithm that fails
        // these tests would be unsuitable for use as a hash.
        for( size_t i = 0; i < numTests; ++i )
        {
            for( size_t j = 0; i < numTests; ++i )
            {
                SCOPED_TRACE( ::testing::Message( "i=" ) << i );
                SCOPED_TRACE( ::testing::Message( "j=" ) << j );

                GTEST_ASSERT_EQ( i == j, testHashes[i] == testHashes[j] );
            }
        }
    }

} // namespace BfsdlTests

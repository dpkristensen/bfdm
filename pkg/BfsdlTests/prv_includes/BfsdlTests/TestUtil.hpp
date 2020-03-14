/**
    BFDP Test Utility Declarations

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

#ifndef BfsdlTests_TestUtil
#define BfsdlTests_TestUtil

// External includes
#include <gtest/gtest.h>
#include <list>
#include <string>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/String.hpp"

// Macro to help comparing things that are implicitly convertible into std::string
// because GoogleTest's ASSERT_EQ and ASSERT_STREQ macros can't handle std::string vs char const*.
#define BFDP_ASSERT_STREQ( _expected, _actual ) \
    ASSERT_TRUE( StrEq( _expected, _actual ) )

namespace BfsdlTests
{

    typedef std::list< std::string > TestStringList;

    ::testing::AssertionResult ArraysMatch
        (
        Bfdp::Byte const* const aExpected,
        Bfdp::Byte const* const aActual,
        size_t const aCount
        );

    void ClearErrorHandlers();

    void SetDefaultErrorHandlers();

    template< class T >
    T const* PtrValueOrNull( T const* aPtr, T const* aNullValue )
    {
        return ( aPtr == NULL )
            ? aNullValue
            : aPtr;
    }

    ::testing::AssertionResult StrEq
        (
        std::string const& aExpected,
        std::string const& aActual
        );

    ::testing::AssertionResult StrListsMatch
        (
        char const* const* aExpected,
        TestStringList& aActual,
        size_t const aCount
        );

    template< size_t NUM_ELEMENTS >
    ::testing::AssertionResult StrListsMatch
        (
        char const* (&aExpected)[NUM_ELEMENTS],
        TestStringList& aActual
        )
    {
        return StrListsMatch( aExpected, aActual, NUM_ELEMENTS );
    }

    static inline char const* StrOrNull( char const* aString )
    {
        return PtrValueOrNull( aString, "(null)" );
    }

} // namespace BfsdlTests

#endif // BfsdlTests_TestUtil

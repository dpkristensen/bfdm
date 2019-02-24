/**
    BFDP Mock TokenObserver Declaration

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

#ifndef BfsdlTests_MockTokenObserver
#define BfsdlTests_MockTokenObserver

// Base include
#include "BfsdlParser/Token/ITokenObserver.hpp"

// External includes
#include <gtest/gtest.h>
#include <list>
#include <string>

// Internal includes
#include "Bfdp/Common.hpp"

namespace BfsdlTests
{

    //! Test double for an ITokenObserver
    //!
    //! This class records values in sequence so they can be checked post-call.
    class MockTokenObserver
        : public BfsdlParser::Token::ITokenObserver
    {
    public:
        MockTokenObserver();

        virtual ~MockTokenObserver();

        virtual bool OnControlCharacter
            (
            std::string const& aControlCharacter
            );

        virtual bool OnNumericLiteral
            (
            BfsdlParser::Objects::NumericLiteral const& aValue
            );

        ::testing::AssertionResult VerifyNext
            (
            std::string const& aNextValue
            );

        ::testing::AssertionResult VerifyNone();

    private:
        typedef std::list< std::string > ValueList;

        ValueList mValues;
    };

} // namespace BfsdlTests

#endif // BfsdlTests_MockTokenObserver

/**
    BFDP Mock Observer Base Definition

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

// Base Includes
#include "BfsdlTests/MockObserverBase.hpp"

// External Includes
#include <sstream>

namespace BfsdlTests
{

    ::testing::AssertionResult MockObserverBase::VerifyNext
        (
        std::string const& aNextValue
        )
    {
        if( mValues.empty() )
        {
            return ::testing::AssertionFailure() << "Missing expected value: " << aNextValue;
        }
        else if( mValues.front() != aNextValue )
        {
            return ::testing::AssertionFailure() << "Symbol mismatch:" << std::endl
                << "    Actual: " << mValues.front() << std::endl
                << "  Expected: " << aNextValue;
        }

        mValues.pop_front();
        return ::testing::AssertionSuccess();
    }

    ::testing::AssertionResult MockObserverBase::VerifyNone()
    {
        if( !mValues.empty() )
        {
            return ::testing::AssertionFailure() << "Unexpected " << mValues.front();
        }
        return ::testing::AssertionSuccess();
    }

    MockObserverBase::~MockObserverBase()
    {
        EXPECT_TRUE( VerifyNone() );
    }

    void MockObserverBase::RecordEvent
        (
        std::string const& aEvent
        )
    {
        mValues.push_back( aEvent );
    }
} // namespace BfsdlTests

/**
    BFDP Mock ErrorHandler Definition

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

// External includes
#include <cstddef>
#include <sstream>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    /* static */ MockErrorHandler::Workspace * MockErrorHandler::mWorkspace = NULL;

    /* static */ void MockErrorHandler::InternalErrorHandler
        (
        char const* const aModuleName,
        unsigned int const aLine,
        char const* const aErrorText
        )
    {
        std::ostringstream errorMsg;
        errorMsg << "Internal Error in " << aModuleName << "@" << aLine << ": " << aErrorText;

        if( mWorkspace == NULL )
        {
            FAIL() << "No workspace registered with MockErrorHandler" << std::endl << errorMsg.str();
        }
        else
        {
            FireError( mWorkspace->mInternalErrorState, errorMsg.str() );
        }
    }

    /* static */ void MockErrorHandler::MisuseErrorHandler
        (
        char const* const aModuleName,
        unsigned int const aLine,
        char const* const aErrorText
        )
    {
        std::ostringstream errorMsg;
        errorMsg << "Misuse Error in " << aModuleName << "@" << aLine << ": " << aErrorText;

        if( mWorkspace == NULL )
        {
            FAIL() << "No workspace registered with MockErrorHandler" << std::endl << errorMsg.str();
        }
        else
        {
            FireError( mWorkspace->mMisuseErrorState, errorMsg.str() );
        }
    }

    /* static */ void MockErrorHandler::RunTimeErrorHandler
        (
        char const* const aModuleName,
        unsigned int const aLine,
        char const* const aErrorText
        )
    {
        std::ostringstream errorMsg;
        errorMsg << "Run Time Error in " << aModuleName << "@" << aLine << ": " << aErrorText;

        if( mWorkspace == NULL )
        {
            FAIL() << "No workspace registered with MockErrorHandler" << std::endl << errorMsg.str();
        }
        else
        {
            FireError( mWorkspace->mRunTimeErrorState, errorMsg.str() );
        }
    }

    MockErrorHandler::ErrorState::ErrorState
        (
        char const* const aName
        )
        : expect( Expectation::None )
        , fired( false )
        , name( aName )
    {
    }

    MockErrorHandler::Workspace::Workspace()
        : mInternalErrorState( "Internal Error" )
        , mMisuseErrorState( "Misuse Error" )
        , mRunTimeErrorState( "Run Time Error" )
    {
        MockErrorHandler::RegisterWorkspace( *this );
    }

    MockErrorHandler::Workspace::~Workspace()
    {
        VerifyExpectation( mInternalErrorState, true );
        VerifyExpectation( mMisuseErrorState, true );
        VerifyExpectation( mRunTimeErrorState, true );
        MockErrorHandler::UnregisterWorkspace();
        SetDefaultErrorHandlers();
    }

    void MockErrorHandler::Workspace::ExpectInternalError
        (
        bool const aExpected
        )
    {
        SetExpectation( mInternalErrorState, aExpected );
    }

    void MockErrorHandler::Workspace::ExpectMisuseError
        (
        bool const aExpected
        )
    {
        SetExpectation( mMisuseErrorState, aExpected );
    }

    void MockErrorHandler::Workspace::ExpectRunTimeError
        (
        bool const aExpected
        )
    {
        SetExpectation( mRunTimeErrorState, aExpected );
    }

    void MockErrorHandler::Workspace::SetExpectation
        (
        ErrorState& aState,
        bool const aExpected
        )
    {
        Expectation::Type newExpectation = aExpected
        ? Expectation::Fired
        : Expectation::NotFired;

        if( aState.expect == Expectation::None )
        {
            aState.expect = newExpectation;
        }
        else
        {
            FAIL() << "Previous Error expectation not fulfilled";
        }
    }

    void MockErrorHandler::Workspace::VerifyExpectation
        (
        ErrorState& aState,
        bool const aParanoid
        )
    {
        switch( aState.expect )
        {
            case Expectation::None:
                ASSERT_TRUE( aParanoid ) << "Test error: cannot verify without setting expectation.";
                break;

            case Expectation::Fired:
                ASSERT_TRUE( aState.fired ) << aState.name;
                break;

            case Expectation::NotFired:
                ASSERT_FALSE( aState.fired ) << aState.name;
                break;

            default:
                FAIL() << "Internal error: unhandled expectation";
                break;
        }

        aState.expect = Expectation::None;
        aState.fired = false;
    }

    /* static */ void MockErrorHandler::FireError
        (
        ErrorState& aState,
        std::string const& aErrorMsg
        )
    {
        if( aState.fired )
        {
            FAIL() << "Duplicate " << aErrorMsg;
        }

        switch( aState.expect )
        {
        case Expectation::Fired:
            aState.fired = true;
            break;

        case Expectation::NotFired:
        case Expectation::None:
            FAIL() << "Unexpected " << aErrorMsg;
            break;

        default:
            FAIL() << "Unknown state for error:" << std::endl
                << "    " << aErrorMsg;
            break;
        };
    }

    /* static */ void MockErrorHandler::RegisterWorkspace
        (
        Workspace& aWorkspace
        )
    {
        if( mWorkspace == NULL )
        {
            mWorkspace = &aWorkspace;
        }
        else
        {
            FAIL() << "Cannot have more than one MockErrorHandler Workspace at a time";
        }
    }

    /* static */ void MockErrorHandler::UnregisterWorkspace()
    {
        if( mWorkspace != NULL )
        {
            mWorkspace = NULL;
        }
    }

    void SetMockErrorHandlers()
    {
        ErrorReporter::SetInternalErrorHandler( MockErrorHandler::InternalErrorHandler );
        ErrorReporter::SetMisuseErrorHandler( MockErrorHandler::MisuseErrorHandler );
        ErrorReporter::SetRunTimeErrorHandler( MockErrorHandler::RunTimeErrorHandler );
    }

} // namespace BfsdlTests

/**
    BFDP Mock ErrorHandler Definition

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

// External includes
#include <cstddef>
#include <sstream>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    /* static */ MockErrorHandler::Workspace * MockErrorHandler::mWorkspace = NULL;

    /* static */ void MockErrorHandler::InternalErrorHandler
        (
        char const* const aModuleName,
        UInt const aLine,
        char const* const aErrorText
        )
    {
        std::stringstream errorMsg;
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
        UInt const aLine,
        char const* const aErrorText
        )
    {
        std::stringstream errorMsg;
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
        UInt const aLine,
        char const* const aErrorText
        )
    {
        std::stringstream errorMsg;
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

    MockErrorHandler::Workspace::Workspace()
        : mInternalErrorState( ErrorState::Unexpected )
        , mMisuseErrorState( ErrorState::Unexpected )
        , mRunTimeErrorState( ErrorState::Unexpected )
    {
        MockErrorHandler::RegisterWorkspace( *this );
    }

    MockErrorHandler::Workspace::~Workspace()
    {
        VerifyExpectations( mInternalErrorState, "Misuse" );
        VerifyExpectations( mMisuseErrorState, "Misuse" );
        VerifyExpectations( mRunTimeErrorState, "Run Time" );
        MockErrorHandler::UnregisterWorkspace();
    }

    void MockErrorHandler::Workspace::ExpectInternalError()
    {
        if( mInternalErrorState != ErrorState::Expected )
        {
            mInternalErrorState = ErrorState::Expected;
        }
        else
        {
            FAIL() << "Previous Internal Error expectation not fulfilled";
        }
    }

    void MockErrorHandler::Workspace::ExpectMisuseError()
    {
        if( mMisuseErrorState != ErrorState::Expected )
        {
            mMisuseErrorState = ErrorState::Expected;
        }
        else
        {
            FAIL() << "Previous Misuse Error expectation not fulfilled";
        }
    }

    void MockErrorHandler::Workspace::ExpectRunTimeError()
    {
        if( mRunTimeErrorState != ErrorState::Expected )
        {
            mRunTimeErrorState = ErrorState::Expected;
        }
        else
        {
            FAIL() << "Previous Run Time Error expectation not fulfilled";
        }
    }

    /* static */ void MockErrorHandler::FireError
        (
        ErrorState::Type& aState,
        std::string const aErrorMsg
        )
    {
        switch( aState )
        {
        case ErrorState::Expected:
            aState = ErrorState::Fired;
            break;

        case ErrorState::Fired:
            FAIL() << "Duplicate " << aErrorMsg;
            break;

        case ErrorState::Unexpected:
            FAIL() << "Unexpected " << aErrorMsg;
            break;

        default:
            FAIL() << "Unknown ErrorState " << aState << " for error:" << std::endl
                << aErrorMsg;
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

    /* static */ void MockErrorHandler::VerifyExpectations
        (
        ErrorState::Type const aState,
        char const* const aErrorType
        )
    {
        if( aState == ErrorState::Expected )
        {
            FAIL() << "Expected " << aErrorType << " Error did not occur";
        }
    }

    void SetMockErrorHandlers()
    {
        ErrorReporter::SetInternalErrorHandler( MockErrorHandler::InternalErrorHandler );
        ErrorReporter::SetMisuseErrorHandler( MockErrorHandler::MisuseErrorHandler );
        ErrorReporter::SetRunTimeErrorHandler( MockErrorHandler::RunTimeErrorHandler );
    }

} // namespace BfsdlTests

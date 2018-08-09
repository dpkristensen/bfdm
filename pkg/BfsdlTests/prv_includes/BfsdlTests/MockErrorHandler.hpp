/**
    BFDP Mock ErrorHandler Declaration

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

#ifndef BfsdlTests_MockErrorHandler
#define BfsdlTests_MockErrorHandler

// External includes
#include <gtest/gtest.h>

// Internal includes
#include "Bfdp/Common.hpp"

namespace BfsdlTests
{

    class MockErrorHandler
    {
    public:
        static void InternalErrorHandler
            (
            char const* const aModuleName,
            Bfdp::UInt const aLine,
            char const* const aErrorText
            );

        static void MisuseErrorHandler
            (
            char const* const aModuleName,
            Bfdp::UInt const aLine,
            char const* const aErrorText
            );

        static void RunTimeErrorHandler
            (
            char const* const aModuleName,
            Bfdp::UInt const aLine,
            char const* const aErrorText
            );

        struct ErrorState
        {
            enum Type
            {
                Expected,
                Fired,
                Unexpected
            };
        };

        struct Workspace
        {
            //! Constructor
            //!
            //! Registers itself with the MockErrorHandler
            Workspace();

            //! Destructor
            //!
            //! Verifies no unexpected asserts occurred
            ~Workspace();

            //! Expect a single Internal Error on the next call.
            void ExpectInternalError();

            //! Expect a single Misuse Error on the next call.
            void ExpectMisuseError();

            //! Expect a single Misuse Error on the next call.
            void ExpectRunTimeError();

        private:
            friend class MockErrorHandler;

            ErrorState::Type mInternalErrorState;
            ErrorState::Type mMisuseErrorState;
            ErrorState::Type mRunTimeErrorState;
        };

    private:
        friend struct Workspace;

        MockErrorHandler();

        static void FireError
            (
            ErrorState::Type& aState,
            std::string const aErrorMsg
            );

        static void RegisterWorkspace
            (
            Workspace& aWorkspace
            );

        static void UnregisterWorkspace();

        static void VerifyExpectations
            (
            ErrorState::Type const aState,
            char const* const aErrorType
            );

        static Workspace* mWorkspace;
    };

    void SetMockErrorHandlers();

} // namespace BfsdlTests

#endif // BfsdlTests_MockErrorHandler

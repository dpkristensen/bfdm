/**
    BFDP Error Reporter Function Definitions

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

// Base includes
#include "Bfdp/ErrorReporter/Functions.hpp"

// External includes
#include <cstdlib>

namespace Bfdp
{

    namespace ErrorReporter
    {

        static ErrorHandler gInternalErrorHandler = NULL;
        static ErrorHandler gMisuseErrorHandler = NULL;
        static ErrorHandler gRunTimeErrorHandler = NULL;

        void ReportInternalError
            (
            char const* const aModuleName,
            unsigned int const aLine,
            char const* const aErrorText
            )
        {
            if( NULL != gInternalErrorHandler )
            {
                gInternalErrorHandler( aModuleName, aLine, aErrorText );
            }
        }

        void ReportMisuseError
            (
            char const* const aModuleName,
            unsigned int const aLine,
            char const* const aErrorText
            )
        {
            if( NULL != gMisuseErrorHandler )
            {
                gMisuseErrorHandler( aModuleName, aLine, aErrorText );
            }
        }

        void ReportRunTimeError
            (
            char const* const aModuleName,
            unsigned int const aLine,
            char const* const aErrorText
            )
        {
            if( NULL != gRunTimeErrorHandler )
            {
                gRunTimeErrorHandler( aModuleName, aLine, aErrorText );
            }
        }

        void SetInternalErrorHandler
            (
            ErrorHandler const aFunction
            )
        {
            gInternalErrorHandler = aFunction;
        }

        void SetMisuseErrorHandler
            (
            ErrorHandler const aFunction
            )
        {
            gMisuseErrorHandler = aFunction;
        }

         void SetRunTimeErrorHandler
            (
            ErrorHandler const aFunction
            )
        {
            gRunTimeErrorHandler = aFunction;
        }

    } // namespace ErrorReporter

} // namespace Bfdp

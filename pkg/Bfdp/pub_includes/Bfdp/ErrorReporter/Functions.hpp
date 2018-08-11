/**
    BFDP Error Reporter Function Declarations

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

#ifndef Bfdp_ErrorReporter_Functions
#define Bfdp_ErrorReporter_Functions

// Internal includes
#include "Bfdp/Common.hpp"

#define BFDP_INTERNAL_ERROR_M( _text, _module ) ::Bfdp::ErrorReporter::ReportInternalError( _module, __LINE__, _text )
#define BFDP_MISUSE_ERROR_M( _text, _module ) ::Bfdp::ErrorReporter::ReportMisuseError( _module, __LINE__, _text )
#define BFDP_RUNTIME_ERROR_M( _text, _module ) ::Bfdp::ErrorReporter::ReportRunTimeError( _module, __LINE__, _text )

#define BFDP_INTERNAL_ERROR( _text ) BFDP_INTERNAL_ERROR_M( _text, BFDP_MODULE )
#define BFDP_MISUSE_ERROR( _text ) BFDP_MISUSE_ERROR_M( _text, BFDP_MODULE )
#define BFDP_RUNTIME_ERROR( _text ) BFDP_RUNTIME_ERROR_M( _text, BFDP_MODULE )

//! If condition, report assertion failure (i.e., misuse error) and return
#define BFDP_RETURNIF_A( _cond, _errmsg ) \
    if( _cond ) \
    { \
        BFDP_MISUSE_ERROR( _errmsg ); \
        return; \
    }

//! If condition, report assertion failure (i.e., misuse error) and return a value
#define BFDP_RETURNIF_VA( _cond, _value, _errmsg ) \
    if( _cond ) \
    { \
        BFDP_MISUSE_ERROR( _errmsg ); \
        return _value; \
    }

//! If condition, report runtime error and return
#define BFDP_RETURNIF_E( _cond, _errmsg ) \
    if( _cond ) \
    { \
        BFDP_RUNTIME_ERROR( _errmsg ); \
        return; \
    }

//! If condition, report runtime error and return a value
#define BFDP_RETURNIF_VE( _cond, _value, _errmsg ) \
    if( _cond ) \
    { \
        BFDP_RUNTIME_ERROR( _errmsg ); \
        return _value; \
    }

namespace Bfdp
{

    namespace ErrorReporter
    {

        //! Error Handler
        //!
        //! The project should register handlers with this component to be notified of errors that
        //! occur in the library.  The handler should not call any library API from the handlers to
        //! prevent a potential deadlock; any extensive processing that does not result in
        //! termination should be deferred or done on a separate thread for performance reasons.
        typedef void (*ErrorHandler)
            (
            char const* const aModuleName,
            UInt const aLine,
            char const* const aErrorText
            );

        //! Report Internal Error
        //!
        //! Report to the handler (if set)
        void ReportInternalError
            (
            char const* const aModuleName,
            UInt const aLine,
            char const* const aErrorText
            );

        //! Report Misuse Error
        //!
        //! Report to the handler (if set)
        void ReportMisuseError
            (
            char const* const aModuleName,
            UInt const aLine,
            char const* const aErrorText
            );

        //! Report Run-Time Error
        //!
        //! Report to the handler (if set)
        void ReportRunTimeError
            (
            char const* const aModuleName,
            UInt const aLine,
            char const* const aErrorText
            );

        //! Register the specified function to handle internal errors.
        //!
        //! @note These should never occur, and indicate an internal contract violation.
        void SetInternalErrorHandler
            (
            ErrorHandler const aFunction
            );

        //! Register the specified function to handle API misuse errors.
        //!
        //! @note These are generally public API misuse, but may also be from internal bugs across library component boundaries.
        void SetMisuseErrorHandler
            (
            ErrorHandler const aFunction
            );

        //! Register the specified function to handle run-time errors.
        //!
        //! @note These are potentially unavoidable run-time failures such as memory allocation
        //!     failures, or other errors reported by the system.
        void SetRunTimeErrorHandler
            (
            ErrorHandler const aFunction
            );

    } // namespace ErrorReporter

} // namespace Bfdp

#endif // Bfdp_ErrorReporter_Functions

/**
    BFDP Test Utility Definitions

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
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    ::testing::AssertionResult ArraysMatch
        (
        UInt8 const * const aExpected,
        UInt8 const * const aActual,
        SizeT const aCount
        )
    {
        for( SizeT i = 0; i < aCount; ++i )
        {
            if( aExpected[i] != aActual[i] )
            {
                return ::testing::AssertionFailure() << "Array mismatch" << std::endl
                    << "  Expected[" << i << "]: " << static_cast< unsigned int >( aExpected[i] & 0xFF ) << std::endl
                    << "  Actual[" << i << "]: " << static_cast< unsigned int >( aActual[i] & 0xFF );
            }
        }

        return ::testing::AssertionSuccess();
    }

    void ClearErrorHandlers()
    {
        ErrorReporter::SetInternalErrorHandler( NULL );
        ErrorReporter::SetMisuseErrorHandler( NULL );
        ErrorReporter::SetRunTimeErrorHandler( NULL );
    }

    static void InternalErrorHandler
        (
        char const * const aModuleName,
        UInt const aLine,
        char const * const aErrorText
        )
    {
        FAIL() << "Internal Error in " << aModuleName << "@" << aLine << ": " << aErrorText;
    }

    static void MisuseErrorHandler
        (
        char const * const aModuleName,
        UInt const aLine,
        char const * const aErrorText
        )
    {
        FAIL() << "Misuse Error in " << aModuleName << "@" << aLine << ": " << aErrorText;
    }

    static void RunTimeErrorHandler
        (
        char const * const aModuleName,
        UInt const aLine,
        char const * const aErrorText
        )
    {
        FAIL() << "Run Time Error in " << aModuleName << "@" << aLine << ": " << aErrorText;
    }

    void SetDefaultErrorHandlers()
    {
        ErrorReporter::SetInternalErrorHandler( MisuseErrorHandler );
        ErrorReporter::SetMisuseErrorHandler( MisuseErrorHandler );
        ErrorReporter::SetRunTimeErrorHandler( RunTimeErrorHandler );
    }

} // namespace BfsdlTests
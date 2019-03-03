/**
    BFDP Mock SymbolObserver Declaration

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

#ifndef BfsdlTests_MockSymbolObserver
#define BfsdlTests_MockSymbolObserver

// Base include
#include "Bfdp/Lexer/ISymbolObserver.hpp"
#include "BfsdlTests/MockObserverBase.hpp"

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"

namespace BfsdlTests
{

    //! Test double for an ISymbolObserver
    //!
    //! This class records values in sequence so they can be checked post-call.
    class MockSymbolObserver
        : public MockObserverBase
        , public Bfdp::Lexer::ISymbolObserver
    {
    public:
        BFDP_OVERRIDE( bool OnMappedSymbols
            (
            int const aCategory,
            std::string const& aSymbols,
            size_t const aNumSymbols
            ) );

        BFDP_OVERRIDE( bool OnUnmappedSymbols
            (
            std::string const& aSymbols,
            size_t const aNumSymbols
            ) );
    };

} // namespace BfsdlTests

#endif // BfsdlTests_MockSymbolObserver

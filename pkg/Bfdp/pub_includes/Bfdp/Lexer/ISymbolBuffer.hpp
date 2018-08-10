/**
    BFDP Lexer SymbolBuffer Interface

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

#ifndef Bfdp_ISymbolBuffer
#define Bfdp_ISymbolBuffer

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Unicode/Common.hpp"

namespace Bfdp
{

    namespace Lexer
    {

        //! Abstract interface for classes which store symbols
        class ISymbolBuffer
        {
        public:
            virtual ~ISymbolBuffer()
            {
            }

            //! @return true if aSymbol was added to the buffer successfully, false otherwise.
            virtual bool Add
                (
                Unicode::CodePoint const aSymbol
                ) = 0;

            //! Reset the symbol buffer to a clear state
            virtual void Clear() = 0;

            //! @return the number of symbols in the buffer
            virtual SizeT GetSize() const = 0;

            //! @return the symbol at the given index
            virtual Unicode::CodePoint GetSymbolAt
                (
                SizeT const aIndex
                ) const = 0;

            //! @return true if the buffer is empty
            virtual bool IsEmpty() const = 0;
        };

    }

} // namespace Bfdp

#endif // Bfdp_ISymbolBuffer

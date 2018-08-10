/**
    BFDP Lexer SymbolCategory Interface

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

#ifndef Bfdp_Lexer_ISymbolCategory
#define Bfdp_Lexer_ISymbolCategory

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Unicode/Common.hpp"

namespace Bfdp
{

    namespace Lexer
    {

        //! Abstract interface for categorization of symbols
        //!
        //! A symbol category has enough information to determine what its category is and whether any
        //! given symbol is in that category.
        class ISymbolCategory
        {
        public:
            virtual ~ISymbolCategory()
            {
            }

            //! @return true if aSymbol is in the category
            virtual bool Contains
                (
                Unicode::CodePoint aSymbol
                ) const = 0;

            //! @return the category associated with this class (must be >= 0)
            virtual SInt GetCategory() const = 0;

            //! @return true if the symbols in the category should be concatenation when reported,
            //!     false otherwise.
            virtual bool ShouldConcatenate() const = 0;
        };

    } // namespace Lexer

} // namespace Bfdp

#endif // Bfdp_Lexer_ISymbolCategory

/**
    BFDP Lexer StringSymbolCategory Declaration

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

#ifndef Bfdp_Lexer_StringSymbolCategory
#define Bfdp_Lexer_StringSymbolCategory

// Base includes
#include "Bfdp/Lexer/SymbolCategoryBase.hpp"

// External includes
#include <string>

namespace Bfdp
{

    namespace Lexer
    {

        //! String Symbol Category
        //!
        //! Implements a ISymbolCategory to provide a lookup based on whether the symbol exists in a
        //! string containing ASCII encoded characters.
        class StringSymbolCategory
            : public SymbolCategoryBase
        {
        public:
            //! Create a StringSymbolCategory
            StringSymbolCategory
                (
                int const aCategory, //!< Category to report when one of these symbols is encountered, must be >= 0
                std::string const& aSymbols, //!< A collection of symbols to be loaded
                bool const aShouldConcatenate //!< Whether symbols should be reported in a concatenated form
                );

            virtual ~StringSymbolCategory();

            //! @copydoc ISymbolCategory::Contains()
            BFDP_OVERRIDE( bool Contains
                (
                Unicode::CodePoint const aSymbol
                ) const );

        private:
            std::string mSymbols;
        };

    } // namespace Lexer

} // namespace Bfdp

#endif // Bfdp_Lexer_StringSymbolCategory

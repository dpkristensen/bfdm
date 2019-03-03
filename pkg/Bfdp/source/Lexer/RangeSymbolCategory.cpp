/**
    BFDP Lexer RangeSymbolCategory Definition

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
#include "Bfdp/Lexer/RangeSymbolCategory.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "Lexer::RangeSymbolCategory"

namespace Bfdp
{

    namespace Lexer
    {

        RangeSymbolCategory::RangeSymbolCategory
            (
            int const aCategory,
            Unicode::CodePoint const aStart,
            Unicode::CodePoint const aEnd,
            bool const aShouldConcatenate
            )
            : SymbolCategoryBase( aCategory, aShouldConcatenate, BFDP_MODULE )
            , mStart( aStart )
            , mEnd( aEnd )
        {
            if( ( aStart == Unicode::InvalidCodePoint ) ||
                ( aEnd   == Unicode::InvalidCodePoint ) ||
                ( aStart > aEnd                      ) )
            {
                BFDP_MISUSE_ERROR( "Bad input to constructor" );
            }
        }

        RangeSymbolCategory::RangeSymbolCategory
            (
            int const aCategory,
            Unicode::CodePoint const aCodePoint,
            bool const aShouldConcatenate
            )
            : SymbolCategoryBase( aCategory, aShouldConcatenate, BFDP_MODULE )
            , mStart( aCodePoint )
            , mEnd( aCodePoint )
        {
            if( aCodePoint == Unicode::InvalidCodePoint )
            {
                BFDP_MISUSE_ERROR( "Bad input to constructor" );
            }
        }

        /* virtual */ RangeSymbolCategory::~RangeSymbolCategory()
        {
        }

        /* virtual */ bool RangeSymbolCategory::Contains
            (
            Unicode::CodePoint const aSymbol
            ) const
        {
            return IsWithinRange( mStart, aSymbol, mEnd );
        }

    } // namespace Lexer

} // namespace Bfdp

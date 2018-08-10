/**
    BFDP Lexer ArraySymbolCategory Definition

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
#include "Bfdp/Lexer/ArraySymbolCategory.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "Lexer::ArraySymbolCategory"

namespace Bfdp
{

    namespace Lexer
    {

        ArraySymbolCategory::ArraySymbolCategory
            (
            SInt const aCategory,
            Unicode::CodePoint const* const aPtr,
            SizeT const aCount,
            bool const aShouldConcatenate
            )
            : SymbolCategoryBase( aCategory, aShouldConcatenate, BFDP_MODULE )
            , mArrayPtr( aPtr )
            , mCount( aCount )
        {
            if( NULL == mArrayPtr )
            {
                BFDP_MISUSE_ERROR( "Bad input to constructor" );
            }
        }

        /* virtual */ ArraySymbolCategory::~ArraySymbolCategory()
        {
        }

        /* virtual */ bool ArraySymbolCategory::Contains
            (
            Unicode::CodePoint const aSymbol
            ) const
        {
            if( NULL == mArrayPtr )
            {
                // Error reported in constructor already
                return false;
            }

            for( unsigned int i = 0; i < mCount; ++i )
            {
                if( mArrayPtr[i] == aSymbol )
                {
                    return true;
                }
            }

            return false;
        }

    } // namespace Lexer

} // namespace Bfdp

/**
    BFDP Lexer SymbolCategory Base Definition

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
#include "Bfdp/Lexer/SymbolCategoryBase.hpp"

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"

namespace Bfdp
{

    namespace Lexer
    {

        /* virtual */ SymbolCategoryBase::~SymbolCategoryBase()
        {
        }

        /* virtual */ int SymbolCategoryBase::GetCategory() const
        {
            return mCategory;
        }

        /* virtual */ bool SymbolCategoryBase::ShouldConcatenate() const
        {
            return mShouldConcatenate;
        }

        SymbolCategoryBase::SymbolCategoryBase
            (
            int const aCategory,
            bool const aShouldConcatenate,
            char const* const aModuleName
            )
            : mCategory( aCategory )
            , mShouldConcatenate( aShouldConcatenate )
        {
            if( aCategory < 0 )
            {
                BFDP_MISUSE_ERROR_M( "Bad input to constructor", aModuleName );
            }
        }

    } // namespace Lexer

} // namespace Bfdp

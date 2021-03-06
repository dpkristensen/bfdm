/**
    BFDP Hashed String Definition

    Copyright 2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

// Base Includes
#include "Bfdp/Algorithm/HashedString.hpp"

// Internal Includes
#include "Bfdp/Algorithm/Calc.hpp"

namespace Bfdp
{

    namespace Algorithm
    {

        bool HashedString::StrictWeakCompare::operator()
            (
            HashedString const& aLhs,
            HashedString const& aRhs
            ) const
        {
            return aLhs.IsStrictWeakLessThan( aRhs );
        }

        HashedString::HashedString
            (
            std::string const& aValue
            )
            : mString( aValue )
            , mHash( FastHash( aValue ) )
        {
        }

        HashedString::HashedString
            (
            std::string const& aValue,
            HashType const aHash
            )
            : mString( aValue )
            , mHash( aHash )
        {
        }

        HashType const HashedString::GetHash() const
        {
            return mHash;
        }

        std::string const& HashedString::GetStr() const
        {
            return mString;
        }

        bool HashedString::IsStrictWeakLessThan
            (
            HashedString const& aOther
            ) const
        {
            return ( mHash == aOther.mHash )
                ? ( 0 > std::strcmp( mString.c_str(), aOther.mString.c_str() ) )
                : ( mHash < aOther.mHash );
        }

        bool HashedString::operator ==
            (
            HashedString const& aOther
            ) const
        {
            return !(operator !=( aOther ));
        }

        bool HashedString::operator !=
            (
            HashedString const& aOther
            ) const
        {
            return ( mHash != aOther.mHash ) ||
                ( 0 != std::strcmp( mString.c_str(), aOther.mString.c_str() ) );
        }

    } // namespace Algorithm

} // namespace Bfdp

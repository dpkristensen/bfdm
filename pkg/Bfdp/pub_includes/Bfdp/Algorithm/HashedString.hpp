/**
    BFDP Hashed String Declaration

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

#ifndef Bfdp_Algorithm_HashedString
#define Bfdp_Algorithm_HashedString

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Algorithm/Calc.hpp"

namespace Bfdp
{

    namespace Algorithm
    {

        //! Hashed String
        //!
        //! This class encapsulates an immutable string with a hash.
        class HashedString BFDP_FINAL
        {
        public:
            //! Functor meeting Compare requirements for strict weak ordering
            struct StrictWeakCompare
            {
                bool operator()
                    (
                    HashedString const& aLhs,
                    HashedString const& aRhs
                    ) const;
            };

            HashedString
                (
                std::string const& aValue
                );

            HashType const GetHash() const;

            std::string const& GetStr() const;

            //! Comparison operator for less than operator with a strict weak ordering
            bool IsStrictWeakLessThan
                (
                HashedString const& aOther
                ) const;

            //! Comparison operator for equality
            bool operator ==
                (
                HashedString const& aOther
                ) const;

            //! Comparison operator for inequality
            bool operator !=
                (
                HashedString const& aOther
                ) const;

        private:
            std::string const mString;
            HashType const mHash;
        };

    } // namespace Algorithm

} // namespace Bfdp

#endif // Bfdp_Algorithm_HashedString

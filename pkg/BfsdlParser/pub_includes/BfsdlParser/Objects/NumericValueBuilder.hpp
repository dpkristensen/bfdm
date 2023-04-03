/**
    BFSDL Parser Fixed Point Number Builder

    Copyright 2023, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef BfsdlParser_Objects_NumericValueBuilder
#define BfsdlParser_Objects_NumericValueBuilder

// Base Includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Objects/Common.hpp"
#include "BfsdlParser/Objects/NumericField.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Numeric Value Builder
        //!
        //! Allows building a numeric value incrementally
        class NumericValueBuilder BFDP_FINAL
            : private Bfdp::NonAssignable
            , private Bfdp::NonCopyable
        {
        public:
            NumericValueBuilder();

            //! @pre HasProperties() returns true
            //! @return Number of bits until value is complete.
            size_t GetBitsTillComplete() const;

            //! @pre Properties were set to indicate a signed value
            //! @pre IsComplete() returns true
            //! @return The value as a signed 64-bit fixed point integer
            int64_t GetRawS64() const;

            //! @pre Properties were set to indicate an unsigned value
            //! @pre IsComplete() returns true
            //! @return The value as an unsigned 64-bit fixed point integer
            uint64_t GetRawU64() const;

            //! @return true if the properties were set, false otherwise.
            bool HasProperties() const;

            //! @return true if the data was supplied completely, false otherwise.
            bool IsComplete() const;

            //! @pre HasProperties() returns true
            //! @return Whether the field is signed or unsigned
            bool NumericValueBuilder::IsSigned() const;

            //! Add the requested bits to the value
            //!
            //! @pre The relevant bits occupy the LSb portion of the value with bit 0 corresponding
            //!     to the next bit required for the sequence.
            //! @pre aNumBits must be > 0 and <= GetBitsTillComplete()
            //! @post Use IsComplete() to determine if all bits are present.
            //! @return Whether the bits were accepted into the buffer.
            bool ParseBits(uint64_t const aValue, size_t const aNumBits);

            void Reset();

            //! Set up the builder to parse values for the given field
            //!
            //! Setting the same field properties is idempotent.
            //!
            //! @note This implicitly clears any previously parsed data.
            //! @return Whether the field properties are supported by this parser
            bool SetFieldProperties
                (
                NumericFieldProperties const& aProperties
                );

        private:
            bool mIsSigned;
            size_t mTotalBits;
            bool mPropsAreSet;
            uint64_t mValue;
            size_t mValueBits;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_NumericValueBuilder

/**
    BFSDL Parser Numeric Value Builder Definition

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

// Base Includes
#include "BfsdlParser/Objects/NumericValueBuilder.hpp"

// Internal Includes
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/Mask.hpp"

#define BFDP_MODULE "Objects::NumericValueBuilder"

namespace BfsdlParser
{

    using namespace Bfdp;

    namespace Objects
    {

        namespace NumericValueBuilderInternal
        {
            static size_t const MaxValueBits = 64;
        };
        using namespace NumericValueBuilderInternal;

        NumericValueBuilder::NumericValueBuilder()
        {
            Reset();
        }

        size_t NumericValueBuilder::GetBitsTillComplete() const
        {
            return mTotalBits - mValueBits;
        }

        int64_t NumericValueBuilder::GetRawS64() const
        {
            return *reinterpret_cast< int64_t const* >( &mValue );
        }

        uint64_t NumericValueBuilder::GetRawU64() const
        {
            return mValue;
        }

        bool NumericValueBuilder::HasProperties() const
        {
            return mPropsAreSet;
        }

        bool NumericValueBuilder::IsComplete() const
        {
            return mPropsAreSet && ( mValueBits == mTotalBits );
        }

        bool NumericValueBuilder::IsSigned() const
        {
            return mIsSigned;
        }

        bool NumericValueBuilder::ParseBits(uint64_t const aValue, size_t const aNumBits)
        {
            // If props are not set, there will be 0 bits till complete, and this will
            // fail to accept any new bits regardless of checking mPropsAreSet.
            if( ( aNumBits == 0 ) || ( GetBitsTillComplete() < aNumBits ) )
            {
                // Invalid # of input bits or props not set
                return false;
            }
            if( ( mValueBits == 0U ) && mIsSigned )
            {
                // On first data, pre-expand negative sign bit for signed values
                uint64_t signMask = BitManip::CreateMask< uint64_t >( 1U, aNumBits - 1 );
                if( ( aValue & signMask ) != 0U )
                {
                    mValue = BitManip::CreateMask< uint64_t >( MaxValueBits, 0U );
                }
            }
            mValue <<= aNumBits;
            mValue = BitManip::ReplaceBits( mValue, aValue, aNumBits, 0U );
            mValueBits += aNumBits;
            return true;
        }

        void NumericValueBuilder::Reset()
        {
            mIsSigned = false;
            mTotalBits = 0U;
            mPropsAreSet = false;
            mValue = 0U;
            mValueBits = 0U;
        }

        bool NumericValueBuilder::SetFieldProperties
            (
            NumericFieldProperties const& aProperties
            )
        {
            BFDP_CTIME_ASSERT( ( sizeof( mValue ) * BitManip::BitsPerByte ) == MaxValueBits, "Fix MaxValueBits" );
            Reset();
            size_t minIntegralBits = aProperties.mSigned ? 1U : 0U; // Sign bit
            size_t minTotalBits = minIntegralBits + 1U; // Sign + at least 1 data bit
            size_t totalBits = aProperties.mIntegralBits + aProperties.mFractionalBits;
            if( ( aProperties.mIntegralBits < minIntegralBits ) ||
                ( !IsWithinRange( minTotalBits, totalBits, MaxValueBits ) ) )
            {
                return false;
            }
            mIsSigned = aProperties.mSigned;
            mTotalBits = totalBits;
            mPropsAreSet = true;
            return true;
        }

    } // namespace Objects

} // namespace BfsdlParser

/**
    BFDP Bitmanip Generic BitStream Definitions

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
#include "Bfdp/BitManip/GenericBitStream.hpp"

// External Includes
#include <algorithm>

// Internal Includes
#include "Bfdp/BitManip/Mask.hpp"
#include "Bfdp/BitManip/Private.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        GenericBitStream::GenericBitStream
            (
            BitBuffer& aBuffer
            )
            : mBuffer( aBuffer )
            , mCurBit( 0 )
            , mCurByte( 0 )
        {
        }

        size_t GenericBitStream::GetBitsTillEnd() const
        {
            return mBuffer.GetDataBits() - GetPosBits();
        }

        size_t GenericBitStream::GetPosBits() const
        {
            return BytesToBits( mCurByte ) + mCurBit;
        }

        bool GenericBitStream::ReadBits
            (
            Byte* const aOutData,
            size_t const aNumBits
            ) const
        {
            size_t outByte = 0;
            size_t outBit = 0;

            return CopyBits
                (
                // Output
                aOutData,
                outByte,
                outBit,
                // Input
                mBuffer.GetDataPtr(),
                mCurByte,
                mCurBit,
                // Common
                aNumBits
                );
        }

        bool GenericBitStream::SeekBits
            (
            size_t const aBitPos
            )
        {
            if( aBitPos > mBuffer.GetDataBits() )
            {
                return false;
            }

            mCurByte = aBitPos / BitsPerByte;
            mCurBit = aBitPos % BitsPerByte;
            return true;
        }

        bool GenericBitStream::WriteBits
            (
            Byte const* const aInData,
            size_t const aNumBits
            )
        {
            size_t inByte = 0;
            size_t inBit = 0;

            return CopyBits
                (
                // Output
                mBuffer.GetDataPtr(),
                mCurByte,
                mCurBit,
                // Input
                aInData,
                inByte,
                inBit,
                // Common
                aNumBits
                );
        }

        bool GenericBitStream::CopyBits
            (
            Byte* const aOutData,
            size_t& aOutByteCtr,
            size_t& aOutBitCtr,
            Byte const* const aInData,
            size_t& aInByteCtr,
            size_t& aInBitCtr,
            size_t const aNumBits
            ) const
        {
            // Detect overrun in such a way as to avoid overflows
            if( GetBitsTillEnd() < aNumBits )
            {
                return false;
            }

            size_t bitsRemain = aNumBits;
            while( bitsRemain )
            {
                // Determine how many bits can be copied at once.
                size_t numBitsToCopy = BitsPerByte - std::max( aInBitCtr, mCurBit );
                numBitsToCopy = std::min( numBitsToCopy, bitsRemain );

                // Extract and replace the relevant bits
                Byte value = ExtractBits( aInData[aInByteCtr], numBitsToCopy, aInBitCtr );
                aOutData[aOutByteCtr] = ReplaceBits( aOutData[aOutByteCtr], value, numBitsToCopy, aOutBitCtr );

                // Update counters
                IncrementPos( aOutByteCtr, aOutBitCtr, numBitsToCopy );
                IncrementPos( aInByteCtr, aInBitCtr, numBitsToCopy );
                bitsRemain -= numBitsToCopy;
            }

            return true;
        }

    } // namespace BitManip

} // namespace Bfdp

/**
    BFDP BitManip BitBuffer Definition

    Copyright 2018-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

//! Base Includes
#include "Bfdp/BitManip/BitBuffer.hpp"

//! External Includes
#include <string>

//! Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/BitManip/Conversion.hpp"

#define BFDP_MODULE "BitManip::BitBuffer"

namespace Bfdp
{

    namespace BitManip
    {

        BitBuffer::BitBuffer
            (
            size_t const aInitialCapacity
            )
            : mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            BFDP_UNUSED_RETURN( CreateBuffer( aInitialCapacity ) );
        }

        BitBuffer::BitBuffer
            (
            Byte const* const aBytes,
            size_t const aNumBits
            )
            : mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            BFDP_RETURNIF( 0 == aBytes );
            BFDP_RETURNIF_A( NULL == aBytes, "Null buffer" );

            size_t numBytes = CreateBuffer( aNumBits );
            if( numBytes != 0 )
            {
                mBuffer.CopyFrom( aBytes, numBytes );
                mDataBits = aNumBits;
            }
        }

        BitBuffer::BitBuffer
            (
            BitBuffer const& aOther
            )
            : mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            Copy( aOther );
        }

        BitBuffer::~BitBuffer()
        {
            DeleteBuffer();
        }

        BitBuffer& BitBuffer::operator=
            (
            BitBuffer const& aOther
            )
        {
            Copy( aOther );
            return *this;
        }

        size_t BitBuffer::GetCapacityBits() const
        {
            return mCapacityBits;
        }

        size_t BitBuffer::GetCapacityBytes() const
        {
            return BitsToBytes( mCapacityBits );
        }

        size_t BitBuffer::GetDataBits() const
        {
            return mDataBits;
        }

        size_t BitBuffer::GetDataBytes() const
        {
            return BitsToBytes( mDataBits );
        }

        Byte* BitBuffer::GetDataPtr()
        {
            return mBuffer.GetPtr();
        }

        bool BitBuffer::IsEmpty() const
        {
            return mDataBits == 0;
        }

        void BitBuffer::MemSet
            (
            Byte const aByte
            )
        {
            mBuffer.MemSet( aByte );
        }

        bool BitBuffer::ResizeNoPreserve
            (
            size_t const aNumBits
            )
        {
            if( aNumBits <= mCapacityBits )
            {
                mDataBits = aNumBits;
                return true;
            }

            DeleteBuffer();
            BFDP_RETURNIF_VE( CreateBuffer( aNumBits ) == 0, false, "Resize failure" );

            mDataBits = aNumBits;
            return true;
        }

        bool BitBuffer::ResizePreserve
            (
            size_t const aNumBits
            )
        {
            if( aNumBits <= mCapacityBits )
            {
                mDataBits = aNumBits;
                return true;
            }

            Data::ByteBuffer newBuffer;
            if( !AllocateBits( aNumBits, newBuffer ) )
            {
                return false;
            }

            //! Copy existing contents to new buffer
            if( !IsEmpty() )
            {
                newBuffer.CopyFrom( mBuffer.GetConstPtr(), BitsToBytes( mDataBits ) );
            }

            //! Switch buffers
            mBuffer.Swap( newBuffer );

            // Update counters
            mCapacityBits = BytesToBits( mBuffer.GetSize() );
            mDataBits = aNumBits;
            return true;
        }

        bool BitBuffer::ResizePreserve
            (
            size_t const aNumBits,
            Byte const aNewByteValue
            )
        {
            size_t oldCapacityBytes = GetCapacityBytes();

            BFDP_RETURNIF_V( !ResizePreserve( aNumBits ), false );

            size_t newCapacityBytes = GetCapacityBytes();

            if( newCapacityBytes > oldCapacityBytes )
            {
                std::memset( &mBuffer[oldCapacityBytes], aNewByteValue, newCapacityBytes - oldCapacityBytes );
            }

            return true;
        }

        bool BitBuffer::AllocateBits
            (
            size_t const aNumBits,
            Data::ByteBuffer& aNewBuffer
            )
        {
            size_t numBytes = BitsToBytes( aNumBits );

            if( !aNewBuffer.Allocate( numBytes ) )
            {
                BFDP_RUNTIME_ERROR( "Allocation failure" );
                return false;
            }

            return true;
        }

        void BitBuffer::Copy
            (
            BitBuffer const& aOther
            )
        {
            size_t numBytes = CreateBuffer( aOther.mCapacityBits );
            if( numBytes != 0 )
            {
                mBuffer.CopyFrom( aOther.mBuffer.GetConstPtr(), numBytes );
                mCapacityBits = aOther.mCapacityBits;
                mDataBits = aOther.mDataBits;
            }
        }

        size_t BitBuffer::CreateBuffer
            (
            size_t const aNumBits
            )
        {
            BFDP_RETURNIF_V( aNumBits == 0, 0 );

            if( !AllocateBits( aNumBits, mBuffer ) )
            {
                return 0;
            }

            mCapacityBits = BytesToBits( mBuffer.GetSize() );
            return mBuffer.GetSize();
        }

        void BitBuffer::DeleteBuffer()
        {
            mBuffer.Delete();
            mCapacityBits = 0;
            mDataBits = 0;
        }

    } // namespace BitManip

} // namespace Bfdp

/**
    BFDP BitManip BitBuffer Definition

    Copyright 2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
            SizeT const aInitialCapacity
            )
            : mBuffer( NULL )
            , mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            BFDP_UNUSED_RETURN( CreateBuffer( aInitialCapacity ) );
        }

        BitBuffer::BitBuffer
            (
            Byte const* const aBytes,
            SizeT const aNumBits
            )
            : mBuffer( NULL )
            , mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            BFDP_RETURNIF( 0 == aBytes );
            BFDP_RETURNIF_A( NULL == aBytes, "Null buffer" );

            SizeT numBytes = CreateBuffer( aNumBits );
            if( numBytes != 0 )
            {
                std::memcpy( mBuffer, aBytes, numBytes );
                mDataBits = aNumBits;
            }
        }

        BitBuffer::BitBuffer
            (
            BitBuffer const& aOther
            )
            : mBuffer( NULL )
            , mCapacityBits( 0 )
            , mDataBits( 0 )
        {
            Copy( aOther );
        }

        BitBuffer::~BitBuffer()
        {
            delete [] mBuffer;
        }

        BitBuffer& BitBuffer::operator=
            (
            BitBuffer const& aOther
            )
        {
            Copy( aOther );
            return *this;
        }

        SizeT BitBuffer::GetCapacityBits() const
        {
            return mCapacityBits;
        }

        SizeT BitBuffer::GetCapacityBytes() const
        {
            return BitsToBytes( mCapacityBits );
        }

        SizeT BitBuffer::GetDataBits() const
        {
            return mDataBits;
        }

        SizeT BitBuffer::GetDataBytes() const
        {
            return BitsToBytes( mDataBits );
        }

        Byte const* BitBuffer::GetDataPtr() const
        {
            return mBuffer;
        }

        void BitBuffer::Copy
            (
            BitBuffer const& aOther
            )
        {
            SizeT numBytes = CreateBuffer( aOther.mCapacityBits );
            if( numBytes != 0 )
            {
                std::memcpy( mBuffer, aOther.mBuffer, numBytes );
                mCapacityBits = aOther.mCapacityBits;
                mDataBits = aOther.mDataBits;
            }
        }

        SizeT BitBuffer::CreateBuffer
            (
            SizeT const aNumBits
            )
        {
            BFDP_RETURNIF_V( aNumBits == 0, 0 );

            SizeT numBytes = BitsToBytes( aNumBits );

            mBuffer = new( std::nothrow ) Byte[numBytes];
            BFDP_RETURNIF_VE( mBuffer == NULL, 0, "Allocation failure" );

            mCapacityBits = BytesToBits( numBytes );
            return numBytes;
        }
    } // namespace BitManip

} // namespace Bfdp

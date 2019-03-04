/**
    BFDP Data Byte Buffer Definitions

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

// Base includes
#include "Bfdp/Data/ByteBuffer.hpp"

// External includes
#include <algorithm>
#include <cstring>

namespace Bfdp
{

    namespace Data
    {

        ByteBuffer::ByteBuffer()
            : mPtr( NULL )
            , mSize( 0U )
        {
        };

        ByteBuffer::~ByteBuffer()
        {
            Delete();
        }

        bool ByteBuffer::Allocate
            (
            size_t const aSize
            )
        {
            Byte* newBuffer = new(std::nothrow)Byte[aSize];
            BFDP_RETURNIF_V( NULL == newBuffer, false );

            Delete();
            mPtr = newBuffer;
            mSize = aSize;

            return true;
        }

        void ByteBuffer::Clear()
        {
            MemSet( 0U );
        }

        size_t ByteBuffer::CopyFrom
            (
            void const* const aPtr,
            size_t const aSize
            )
        {
            size_t copySize = std::min( mSize, aSize );
            if( copySize )
            {
                std::memcpy( mPtr, aPtr, copySize );
            }
            return copySize;
        }

        void ByteBuffer::Delete()
        {
            delete [] mPtr;
            mPtr = NULL;
            mSize = 0U;
        }

        Byte const* ByteBuffer::GetConstPtr() const
        {
            return mPtr;
        }

        Byte* ByteBuffer::GetPtr()
        {
            return mPtr;
        }

        size_t ByteBuffer::GetSize() const
        {
            return mSize;
        }

        std::string ByteBuffer::GetString
            (
            size_t const aBytes
            )
        {
            BFDP_RETURNIF_V( mSize == 0, std::string() );

            size_t numBytes = aBytes;
            if( numBytes == 0 )
            {
                numBytes = mSize;
            }
            else
            {
                numBytes = std::min( aBytes, mSize );
            }
            return std::string( GetConstPtrT< char >(), numBytes );
        }

        void ByteBuffer::MemSet
            (
            Byte const aValue
            )
        {
            if( mSize )
            {
                std::memset( mPtr, aValue, mSize );
            }
        }

        void ByteBuffer::Swap
            (
            ByteBuffer& aOther
            )
        {
            std::swap( mPtr, aOther.mPtr );
            std::swap( mSize, aOther.mSize );
        }

        Byte& ByteBuffer::operator []
            (
            size_t const aIndex
            )
        {
            return mPtr[aIndex];
        }

        Byte const& ByteBuffer::operator []
            (
            size_t const aIndex
            ) const
        {
            return mPtr[aIndex];
        }

    } // namespace Data

} // namespace Bfdp

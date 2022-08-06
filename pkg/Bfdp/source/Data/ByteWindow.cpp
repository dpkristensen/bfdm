/**
    BFDP Data Byte Window Definitions

    Copyright 2022, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/Data/ByteWindow.hpp"

// External includes
#include <algorithm>
#include <cstring>

// Internal Includes
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "Data::ByteWindow"

namespace Bfdp
{

    namespace Data
    {

        ByteWindow::ByteWindow()
            : mBeginCounter( 0 )
            , mEndCounter( 0 )
            , mHead( 0 )
        {
        }

        bool ByteWindow::Init
            (
            size_t const aSize
            )
        {
            if( !mBuf.Allocate( aSize ) )
            {
                return false;
            }

            Reset();
            return true;
        }

        size_t ByteWindow::GetBeginCounter() const
        {
            return mBeginCounter;
        }

        Byte ByteWindow::GetByte
            (
            size_t const aIndex
            ) const
        {
            if( aIndex >= GetSize() )
            {
                return 0U;
            }

            return mBuf[IncrementIndex(mHead, aIndex)];
        }

        size_t ByteWindow::GetEndCounter() const
        {
            return mEndCounter;
        }

        ByteBuffer const& ByteWindow::GetRawBuffer() const
        {
            return mBuf;
        }

        size_t ByteWindow::GetSize() const
        {
            return mEndCounter - mBeginCounter;
        }

        void ByteWindow::PushData
            (
            Byte const* const aData,
            size_t const aSize
            )
        {
            if( aSize == 0 )
            {
                // No change without any data
                return;
            }

            size_t bytesLeft = aSize;
            size_t inDataIdx = 0;
            if( bytesLeft > mBuf.GetSize() )
            {
                // Too much data for underlying buffer, skip to the end and cycle out existing window
                inDataIdx = bytesLeft - mBuf.GetSize();
                mEndCounter += inDataIdx;
                mBeginCounter = mEndCounter; // All existing data is skipped, so bring begin up to the end
                mHead = 0; // Head index is irrelevant now, so just reset.
                bytesLeft = mBuf.GetSize();
            }

            // Copy remaining data to fill up any remaining window space
            size_t tail = IncrementIndex( mHead, GetSize() );
            if( bytesLeft )
            {
                size_t bytesToCopy = std::min( bytesLeft, mBuf.GetSize() - GetSize() );
                CopyCirc( tail, &aData[inDataIdx], bytesToCopy );
                // Head did not move, so only move the tail
                tail = IncrementIndex( tail, bytesToCopy );
                mEndCounter += bytesToCopy;
                inDataIdx += bytesToCopy;
                bytesLeft -= bytesToCopy;
            }

            // Copy remaining data, moving both head and tail
            if( bytesLeft )
            {
                CopyCirc( tail, &aData[inDataIdx], bytesLeft );
                mBeginCounter += bytesLeft;
                mEndCounter += bytesLeft;
                mHead = IncrementIndex( mHead, bytesLeft );
            }
        }

        void ByteWindow::Reset()
        {
            mBeginCounter = 0U;
            mEndCounter = 0U;
            mHead = 0U;
        }

        void ByteWindow::CopyCirc
            (
            size_t const aBufIndex,
            Byte const* const aData,
            size_t const aSize
            )
        {
            // Sanity guards: do nothing if out of bounds or empty
            if( ( aBufIndex >= mBuf.GetSize() ) || ( aSize == 0 ) )
            {
                return;
            }

            size_t copySize = std::min( aSize, mBuf.GetSize() - aBufIndex );
            std::memcpy( &mBuf[aBufIndex], aData, copySize );

            size_t dataIdx = copySize;
            copySize = aSize - copySize;

            if( copySize > 0 )
            {
                std::memcpy( &mBuf[0], &aData[dataIdx], copySize );
            }
        }

        size_t ByteWindow::IncrementIndex
            (
            size_t const aIndex,
            size_t const aCount
            ) const
        {
            if( ( aCount == 0 ) || ( mBuf.GetSize() == 0 ) )
            {
                return aIndex;
            }
            size_t idx = aIndex + aCount;
            while( idx >= mBuf.GetSize() )
            {
                idx -= mBuf.GetSize();
            }

            return idx;
        }

    } // namespace Data

} // namespace Bfdp

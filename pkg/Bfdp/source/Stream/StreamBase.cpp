/**
    BFDP Stream Base Class Definitions

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

// Base includes
#include "Bfdp/Stream/StreamBase.hpp"

// External Includes
#include <cstring>
#include <sstream>

// Internal Includes
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"

#define BFDP_MODULE "Bfdp::Stream"

namespace Bfdp
{

    namespace Stream
    {

        /* virtual */ StreamBase::~StreamBase()
        {
        }

        size_t StreamBase::GetTotalProcessedBits() const
        {
            return mTotalProcessedBits;
        }

        size_t StreamBase::GetTotalProcessedBytes() const
        {
            return mTotalProcessedBytes;
        }

        std::string StreamBase::GetTotalProcessedStr() const
        {
            std::stringstream ss;
            ss << mTotalProcessedBytes << "." << mTotalProcessedBits << " Bb";
            std::string ret = ss.str();
            return ret;
        }

        bool StreamBase::HasError()
        {
            return mHasError;
        }

        bool StreamBase::ReadSequenceStart()
        {
            mHasError = false;
            // To (hopefully) keep underlying I/O block aligned, allocate two
            // chunks worth of data so that a new read can be appended to
            // unprocessed data.
            if( ( 0 == mBuffer.GetCapacityBytes() ) &&
                ( !mBuffer.ResizeNoPreserve( BitManip::BytesToBits( mChunkSize * 2 ) ) ) )
            {
                BFDP_RUNTIME_ERROR( "Failed to allocate read buffer" );
                mHasError = true;
                return false;
            }

            return true;
        }

        bool StreamBase::ReadSequenceContinue()
        {
            size_t startPositionBits = mBufferPositionBits;
            if( !mBuffer.SetDataBytes( mBufferDataSizeBytes ) )
            {
                mHasError = true;
                BFDP_INTERNAL_ERROR( "Buffer data out of view" );
                return false;
            }
            BitManip::GenericBitStream bitstream = BitManip::GenericBitStream( mBuffer );
            if( !bitstream.SeekBits( startPositionBits ) )
            {
                mHasError = true;
                BFDP_INTERNAL_ERROR( "Buffer seek position out of sync" );
                return false;
            }
            Control::Type control = Control::Continue;
            while( bitstream.GetBitsTillEnd() )
            {
                // TODO: Make GenericBitStream work for read-only operations while const
                control = mObserver.OnStreamData( bitstream );
                mLastControlCode = control;

                // If the buffer moved backward, # of bits processed is indeterminate.
                if( bitstream.GetPosBits() < mBufferPositionBits )
                {
                    mHasError = true;
                    BFDP_INTERNAL_ERROR( "Buffer position corrupted" );
                    return false;
                } else if( ( bitstream.GetPosBits() == mBufferPositionBits ) &&
                    ( control == Control::Continue ) )
                {
                    // Detect implicit NoData by checking to see if the position
                    // didn't move since last time; this could prevent an infinite
                    // loop.
                    control = Control::NoData;
                }
                // Save new buffer position and figure out how many bits were processed.
                mBufferPositionBits = bitstream.GetPosBits();

                if( control != Control::Continue )
                {
                    // Stop calling the callback, but don't return yet as the internal
                    // state (counters, etc...) needs to be updated.
                    break;
                }
            }

            // Calculate number of processed bits since the beginning of the
            // callback loop.  The position always moved forwards; backwards
            // movement is detected and handled in the loop above.
            size_t numProcessedBits = mBufferPositionBits - startPositionBits;

            // Update totals as (total bytes) + (remainder in bits)
            mTotalProcessedBits += numProcessedBits;
            mTotalProcessedBytes += mTotalProcessedBits / BitManip::BitsPerByte;
            mTotalProcessedBits %= BitManip::BitsPerByte;

            // Move byte-oriented data position up to the beginning of the
            // byte where mBufferPositionBits points to.
            size_t numProcessedBytes = ( mBufferPositionBits / BitManip::BitsPerByte ) - mBufferDataOffset;
            mBufferDataOffset += numProcessedBytes;
            mBufferDataSizeBytes -= numProcessedBytes;

            // Interrupt stream operation now that internal state is updated
            if( control == Control::Error )
            {
                mHasError = true;
                return false;
            }
            else if( control == Control::Stop )
            {
                return false;
            }

            // Before the next read, free up space in the buffer by moving
            // existing data to the beginning.  This assumes in-place memcpy
            // works from lowest address to highest for efficiency over
            // supporting insane STL's.
            if( mBufferDataOffset != 0 )
            {
                std::memcpy( mBuffer.GetDataPtr(), mBuffer.GetDataPtr() + numProcessedBytes, mBufferDataSizeBytes );
                mBufferDataOffset = 0;
            }

            if( !mIn ) {
                // If read has not started or end of stream was detected in
                // the last read; we have no new data to process. So we will
                // stop the sequence.
                //
                // When reading past the end of file, eof()+fail() will be set;
                // only bad() is necessary to see if there was an actual error.
                mHasError = mHasError || mIn.bad();
                return false;
            }

            // Now figure out where the free space is to read more data.
            size_t bufferFreeSpaceOffset = mBufferDataOffset + mBufferDataSizeBytes;
            size_t bufferFreeSpaceCount = mBuffer.GetCapacityBytes() - bufferFreeSpaceOffset;

            if( bufferFreeSpaceCount < mChunkSize )
            {
                // Cannot fit a whole chunk here; the callback is probably
                // trying to process too much data at once.
                BFDP_RUNTIME_ERROR( "Stream buffer overflow" );
                mHasError = true;
                return false;
            }

            size_t readSize = mChunkSize;
            if( !ReadImpl( mIn, readSize, mBuffer.GetDataPtr() + bufferFreeSpaceOffset ) )
            {
                mHasError = true;
                return false;
            }
            if( mBufferDataSizeBytes + readSize > mBuffer.GetCapacityBytes() )
            {
                BFDP_INTERNAL_ERROR( "Stream read buffer overflow" );
                mHasError = true;
                return false;
            }
            mBufferDataSizeBytes += readSize;

            // If the read went past the end of the input stream, readSize
            // will probably be zero; but the eofbit wll be set in istream and
            // it will be checked once this function is called again to have a
            // chance at processing the remaining data in the buffer.

            return true;
        }

        void StreamBase::ReadSequenceEnd()
        {
            if( !mHasError &&
                ( ( mLastControlCode == Control::Continue ) && ( mBufferDataSizeBytes > 0 ) ) )
            {
                BFDP_RUNTIME_ERROR( "Unread stream data" );
                mHasError = true;
            }
        }

        bool StreamBase::ReadStream()
        {
            if( !ReadSequenceStart() )
            {
                return false;
            }

            while( ReadSequenceContinue() )
            {
                // Keep reading until stop
            }

            ReadSequenceEnd();
            return true;
        }

        bool StreamBase::IsValid() const
        {
            return IsValidImpl();
        }

        StreamBase::StreamBase
            (
            std::string const& aName,
            std::istream& aIn,
            IStreamObserver& aObserver
            )
            : mBuffer()
            , mBufferDataOffset( 0U )
            , mBufferDataSizeBytes( 0U )
            , mBufferPositionBits( 0U )
            , mChunkSize( 4096U ) // Default
            , mHasError( false )
            , mIn( aIn )
            , mLastControlCode( Control::Continue )
            , mName( aName )
            , mObserver( aObserver )
            , mTotalProcessedBytes( 0U )
            , mTotalProcessedBits( 0U )
        {
        }

        /* virtual */ bool StreamBase::ReadImpl
            (
            std::istream& aInStream,
            size_t& aInOutSizeBytes,
            Byte* const aOutBuffer
            )
        {
            BFDP_CTIME_ASSERT( sizeof(Byte) == sizeof(char), "Unsupported char size" );
            std::streamsize streamReadSize = static_cast< std::streamsize >( aInOutSizeBytes );
            aInStream.read( reinterpret_cast< char* >( aOutBuffer ), streamReadSize );
            streamReadSize = aInStream.gcount();
            if( streamReadSize < 0 )
            {
                return false;
            }
            aInOutSizeBytes = static_cast< size_t >( streamReadSize );
            return true;
        }

        /* virtual */ bool StreamBase::IsValidImpl() const
        {
            // The base class should not be used directly
            return false;
        }

    } // namespace Stream

} // namespace Bfdp

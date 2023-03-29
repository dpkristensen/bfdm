/**
    BFDP Stream Base Class Declarations

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

#ifndef Bfdp_Stream_StreamBase
#define Bfdp_Stream_StreamBase

// External Includes
#include <iostream>
#include <string>
#include <memory>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"
#include "Bfdp/String.hpp"
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Stream/IStreamObserver.hpp"

namespace Bfdp
{

    namespace Stream
    {

        //! Base class for Streams
        //!
        //! This class allows reading data from a data stream (e.g., file,
        //! stdin, etc...) into a buffer, and then presenting a view of that
        //! data to the caller as a bitwise data stream.
        //!
        //! The caller provides a function to receive data which will be
        //! called every time data is available; periodically the buffer
        //! will be re-filled from the input stream.
        //!
        //! This base class lets concrete classes specialize behavior using
        //! the Template Method pattern.
        class StreamBase
            : private NonAssignable
            , private NonCopyable
        {
        public:
            virtual ~StreamBase();

            //! @return the "bits" portion of the total processed data counter
            size_t GetTotalProcessedBits() const;

            //! @return the "bytes" portion of the total processed data counter
            size_t GetTotalProcessedBytes() const;

            //! @return A string representation of the total processed data counter
            std::string GetTotalProcessedStr() const;

            //! @return Whether an error was reported in the read sequence
            bool HasError();

            //! Start a multi-operation read sequence
            //!
            //! @pre Caller must check IsValid() first.
            //! @return Whether the sequence can continue.
            bool ReadSequenceStart();

            //! Continue a multi-operation read sequence
            //!
            //! @return Whether the sequence can continue.
            bool ReadSequenceContinue();

            //! End a multi-operation read sequence
            //!
            //! This should be called always if ReadSequenceStart() returned true.
            void ReadSequenceEnd();

            //! Single implementation of the entire read sequence
            //!
            //! @pre Caller must check IsValid() first.
            //! @return True if the sequence was started successfully
            bool ReadStream();

            //! @return Whether the stream object is ready to be used
            bool IsValid() const;

        protected:
            StreamBase
                (
                std::string const& aName,
                std::istream& aIn,
                IStreamObserver& aObserver
                );

            //! Implementation-specific read function
            //!
            //! @pre aInOutSizeBytes will indicate the desired read size.
            //! @pre aOutBuffer will point to a buffer of at least aInOutSizeBytes
            //! @post On success, aInOutSizeBytes must be updated to indicate
            //!     how much data was read into aOutBuffer.
            //! @return Whether any data was read
            virtual bool ReadImpl
                (
                std::istream& aInStream,
                size_t& aInOutSizeBytes,
                Byte* const aOutBuffer
                );

            //! @return Whether the stream object is ready to be used
            virtual bool IsValidImpl() const;

        private:
            //! Buffer used to store data read from the stream
            BitManip::BitBuffer mBuffer;

            //! Offset of unprocessed data bytes in the buffer from the beginning
            size_t mBufferDataOffset;

            //! Number of unprocessed data bytes in the buffer from offset
            size_t mBufferDataSizeBytes;

            //! Start position of the unprocessed data in the buffer from the beginning
            size_t mBufferPositionBits;

            //! Maximum number of bytes to read at a time from the stream
            size_t mChunkSize;

            //! Whether an error occurred during a read sequence
            bool mHasError;

            //! Input stream
            std::istream& mIn;

            Control::Type mLastControlCode;

            //! Name reference for mIn
            std::string mName;

            //! Interface for processing data
            IStreamObserver& mObserver;

            //! Byte-wise portion of total amount of data ever processed (may wrap!)
            size_t mTotalProcessedBytes;

            //! Bit-wise portion of total amount of data ever processed
            size_t mTotalProcessedBits;

        };

        typedef std::shared_ptr< StreamBase > StreamPtr;

    } // namespace Stream

} // namespace Bfdp

#endif // Bfdp_Stream_StreamBase

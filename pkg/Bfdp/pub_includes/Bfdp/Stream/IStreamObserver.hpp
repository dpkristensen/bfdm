/**
    BFDP Stream Observer Interface

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

#ifndef Bfdp_Stream_IStreamObserver
#define Bfdp_Stream_IStreamObserver

// Internal includes
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Stream/Common.hpp"

namespace Bfdp
{

    namespace Stream
    {

        //! Abstract interface for handling data from the stream
        //!
        //! aInBitStream is read-only and the callback is expected to read
        //! from it by the stream operations.  The callback may choose to read
        //! read all data available at once or only a portion at a time.
        //!
        //! Not reading from aInBitStream may implicitly change a return value
        //! of Control::Continue to Control::NoData; but it is always better
        //! to explicitly return NoData when the bitstream does not have
        //! enough data to process.
        //!
        //! After data is exhausted from the bitstream, more data will be read
        //! from the input stream before the callback is called again.
        //!
        //! No information may be drawn from the current position of the
        //! position or the contents of the bitstream between callback
        //! invocations.  The bitstream is a snapshot into the Stream's moving
        //! window, so the bitstream object or the data pointed to by the
        //! bitstream may change.
        //!
        //! The bitstream will contain up to a maximum of 2 times the read
        //! chunk size; data for large read operations will have to be cached
        //! by the callback.  Repeated attempts to ignore data from the stream
        //! could eventually trigger an error and stop the stream.
        class IStreamObserver
        {
        public:
            //! Stream data callback function
            //!
            //! @return How to proceed with the stream operation.
            virtual Control::Type OnStreamData
                (
                BitManip::GenericBitStream& aInBitStream
                ) = 0;

        protected:
            virtual ~IStreamObserver()
            {
            }
        };

    } // namespace Stream

} // namespace Bfdp

#endif // Bfdp_Stream_IStreamObserver

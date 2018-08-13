/**
    BFDP BitManip BitBuffer Declaration

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

#ifndef Bfdp_BitManip_BitBuffer
#define Bfdp_BitManip_BitBuffer

//! Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/String.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! BitBuffer
        //!
        //! Encapsulates a bit-oriented buffer of data
        class BitBuffer BFDP_FINAL
        {
        public:
            //! Constructor
            BitBuffer
                (
                SizeT const aInitialCapacity = 0
                );

            //! Construct a buffer from existing data
            BitBuffer
                (
                Byte const* const aBytes,
                SizeT const aNumBits
                );

            //! Copy Constructor
            BitBuffer
                (
                BitBuffer const& aOther
                );

            //! Destructor
            ~BitBuffer();

            //! Copy Assignment operator
            BitBuffer& operator=
                (
                BitBuffer const& aOther
                );

            SizeT GetCapacityBits() const;

            SizeT GetCapacityBytes() const;

            SizeT GetDataBits() const;

            SizeT GetDataBytes() const;

            Byte* GetDataPtr();

        private:
            //! Copy aOther into this object
            void Copy
                (
                BitBuffer const& aOther
                );

            //! Create a buffer to hold aNumBits
            //!
            //! @return Capacity of the buffer in bytes (0 on memory failure)
            SizeT CreateBuffer
                (
                SizeT const aNumBits
                );

            Byte* mBuffer;
            SizeT mCapacityBits;
            SizeT mDataBits;
        };

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_BitBuffer

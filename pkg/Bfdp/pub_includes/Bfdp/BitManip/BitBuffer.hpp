/**
    BFDP BitManip BitBuffer Declaration

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

#ifndef Bfdp_BitManip_BitBuffer
#define Bfdp_BitManip_BitBuffer

//! Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/String.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! BitBuffer
        //!
        //! Encapsulates a bit-oriented buffer of data
        //!
        //! The capacity always reflects the physical memory dimensions of the
        //! buffer, where the "data size" reflects changes made by resizing or
        //! or setting the data size directly.  In simpler terms, the following
        //! will always be true:
        //!
        //! * Capacity Bits = Capacity Bytes * 8
        //! * Data Bits <= Capacity Bits
        //! * Data Bytes <= Capacity Bytes
        //! * Data Bits <= (Data Bytes * 8) (within -8)
        //! * Data Bytes >= (Data Bits / 8) (within +1)
        //!
        //! The intended use of this buffer is that the same Bit or Byte family
        //! of functions is used when changing the data size of the buffer and
        //! subsequently checking it.
        //!
        //! Because the size is stored as bits, the upper limit on number of bits
        //! that can be represented is (SIZE_MAX/8); using a size or capacity
        //! greater than or equal to (SIZE_MAX/8) will result in Undefined
        //! Behavior (probably an overflow in the bits size)!
        //!
        //! This class does NOT provide any concept of "current position" in
        //! the buffer; see GenericBitStream for use cases when the buffer
        //! needs to be read or written.
        class BitBuffer BFDP_FINAL
        {
        public:
            //! Constructor
            BitBuffer
                (
                size_t const aInitialCapacity = 0
                );

            //! Construct a buffer from existing data
            //!
            //! @note This performs a COPY; does not use the provided buffer
            //!     directly.
            BitBuffer
                (
                Byte const* const aBytes,
                size_t const aNumBits
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

            //! Get capacity in bits
            //!
            //! If the buffer is initialized or resized to a capacity including
            //! a partial byte, it will have rounded UP to a whole number of
            //! bytes since partial bytes cannot exist in memory.
            //!
            //! @returns The number of bits in the backing memory buffer.
            size_t GetCapacityBits() const;

            //! Get capacity in bytes
            //!
            //! @note See GetCapacityBits() about alignment
            //! @returns The number of bytes in the backing memory buffer.
            size_t GetCapacityBytes() const;

            //! Get number of data bits
            //!
            //! This will always be less than or equal to GetCapacityBits().
            //! Unlike GetCapacityBits(), this value can represent a partial
            //! number of bytes.
            //!
            //! @return The number of bits used in the last successful bit-
            //!     oriented size change operation.
            size_t GetDataBits() const;

            //! Get number of data bytes
            //!
            //! This will always be less than or equal to GetCapacityBytes().
            //!
            //! @return The number of bytes used in the last successful byte-
            //!     oriented size change operation.
            size_t GetDataBytes() const;

            //! Get pointer to the data buffer
            //!
            //! @note This address may change on resize or copy operations.
            //! @return Pointer to the beginning of the data buffer.
            Byte* GetDataPtr();

            //! @note Memory may still be allocated
            //! @return Whether the number of data bits is 0
            bool IsEmpty() const;

            //! Set all bytes of the buffer to the given value
            //!
            //! @note This is safe to call with no allocated buffer.
            void MemSet
                (
                Byte const aByte
                );

            //! Resize the buffer to aNumBits (no preservation)
            //!
            //! @note This does not attempt to preserve the data, but nor does it guarantee any
            //!     post-condition about the contents of the buffer.
            //! @return true if successful, false otherwise.
            bool ResizeNoPreserve
                (
                size_t const aNumBits
                );

            //! Resize the buffer to aNumBits (preserve data)
            //!
            //! @return true if successful, false otherwise.
            bool ResizePreserve
                (
                size_t const aNumBits
                );

            //! Resize the buffer to aNumBits (preserve data)
            //!
            //! Also uses aNewByteValue to initialize any newly allocated memory.
            //!
            //! @return true if successful, false otherwise.
            bool ResizePreserve
                (
                size_t const aNumBits,
                Byte const aNewByteValue
                );

            //! Set the data size to the specified number of bits
            //!
            //! This sets the number of data bits to the given size regardless
            //! of the old size, so long as aNumBits <= GetCapacityBits().
            //!
            //! Unlike the Resize* family of functions, this is guaranteed to
            //! both preserve the original data AND never perform any memory
            //! allocate/free/copy operations.
            //!
            //! @post On success, GetDataBits() will return aNumBits
            //! @return true if successful, false otherwise.
            bool BitBuffer::SetDataBits
                (
                size_t const aNumBits
                );

            //! Set the data size to the specified number of bytes
            //!
            //! This sets the number of data bytes to the given size regardless
            //! of the old size, so long as aNumBytes <= GetCapacityBytes().
            //!
            //! Unlike the Resize* family of functions, this is guaranteed to
            //! both preserve the original data AND never perform any memory
            //! allocate/free/copy operations.
            //!
            //! @post On success, GetDataBytes() will return aNumBytes
            //! @return true if successful, false otherwise.
            bool BitBuffer::SetDataBytes
                (
                size_t const aNumBytes
                );

        private:
            //! Allocate a new buffer
            bool AllocateBits
                (
                size_t const aNumBits, //!< [in] Number of bits needed
                Data::ByteBuffer& aNewBuffer //!< [inout] Buffer to use
                );

            //! Copy aOther into this object
            void Copy
                (
                BitBuffer const& aOther
                );

            //! Create a buffer to hold aNumBits
            //!
            //! @return Capacity of the buffer in bytes (0 on memory failure)
            size_t CreateBuffer
                (
                size_t const aNumBits
                );

            void DeleteBuffer();

            Data::ByteBuffer mBuffer;
            size_t mCapacityBits;
            size_t mDataBits;
        };

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_BitBuffer

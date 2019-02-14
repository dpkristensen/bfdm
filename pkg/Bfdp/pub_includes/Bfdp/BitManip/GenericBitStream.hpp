/**
    BFDP Bitmanip Generic BitStream Declarations

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

#ifndef Bfdp_BitManip_GenericBitStream
#define Bfdp_BitManip_GenericBitStream

// External Includes
#include <limits>

// Internal Includes
#include "Bfdp/BitManip/BitBuffer.hpp"
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace BitManip
    {

        //! Generic BitStream
        //!
        //! This class implements a BitStream for which the byte/bit ordering of the underlying
        //! buffer is unimportant.  Because of this, it is important to rely only on the
        //! interpretation of values read from the buffer as they were originally written.
        //!
        //! If this is used to access a buffer as a collection of elements, all elements must be
        //! written and read back with the same offset and size to prevent data corruption.  Use
        //! cases that could take advantage of this are:
        //! * A vector of non-standard numeric elements.  Ex:
        //!   * Digits of "big int" field
        //!   * A string encoded in a very limited character set
        //! * Compact storage for a data structure of bitfields
        class GenericBitStream BFDP_FINAL
        {
        public:
            GenericBitStream
                (
                BitBuffer& aBuffer
                );

            SizeT GetBitsTillEnd() const;

            SizeT GetPosBits() const;

            //! Read aNumBits into aOutData
            //!
            //! @note The bytes in aOutData are read to offset 0 in ascending order, with the
            //!     least significant portion of each byte read first.
            //! @return true on success, or false otherwise.
            bool ReadBits
                (
                Byte* const aOutData,
                SizeT const aNumBits
                );

            //! Read all the bits of aValue
            //!
            //! @return true on success, or false otherwise.
            template< class T >
            bool ReadValue
                (
                T& aOutValue
                );

            //! Seek to a given position
            //!
            //! @return true on success, or false if out of bounds.
            bool SeekBits
                (
                SizeT const aBitPos
                );

            //! Write aNumBits into aInData
            //!
            //! @note The bytes in aInData are written from offset 0 in ascending order, with the
            //!     least significant portion of each byte written first.
            //! @return true on success, or false otherwise.
            bool WriteBits
                (
                Byte const* const aInData,
                SizeT const aNumBits
                );

            //! Write all the bits of aValue
            //!
            //! @return true on success, or false otherwise.
            template< class T >
            bool WriteValue
                (
                T& aInValue
                );

        private:
            bool CopyBits
                (
                Byte* const aOutData,
                SizeT& aOutByteCtr,
                SizeT& aOutBitCtr,
                Byte const* const aInData,
                SizeT& aInByteCtr,
                SizeT& aInBitCtr,
                SizeT const aNumBits
                );

            BitBuffer& mBuffer;
            SizeT mCurBit;
            SizeT mCurByte;
        };

        template< class T >
        bool GenericBitStream ::ReadValue
            (
            typename T& aValue
            )
        {
            BFDP_CTIME_ASSERT( std::numeric_limits< typename T >::is_integer, "Unsupported Type" );
            return ReadBits( &aValue, BytesToBits( sizeof( aValue ) ) );
        }

        template< class T >
        bool GenericBitStream::WriteValue
            (
            typename T& aValue
            )
        {
            BFDP_CTIME_ASSERT( std::numeric_limits< typename T >::is_integer, "Unsupported Type" );
            return WriteBits( &aValue, BytesToBits( sizeof( aValue ) ) );
        }

    } // namespace BitManip

} // namespace Bfdp

#endif // Bfdp_BitManip_GenericBitStream

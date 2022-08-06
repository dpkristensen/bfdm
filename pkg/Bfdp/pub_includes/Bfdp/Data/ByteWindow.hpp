/**
    BFDP Data Byte Window Declarations

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

#ifndef Bfdp_Data_ByteWindow
#define Bfdp_Data_ByteWindow

// Internal includes
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

namespace Bfdp
{

    namespace Data
    {

        //! Encapsulates a windowing function for a Byte array
        class ByteWindow BFDP_FINAL
            : private NonAssignable
            , private NonCopyable
        {
        public:
            ByteWindow();

            //! (Re)Initialize the window to aSize bytes
            //!
            //! @note Does not preserve existing content.
            //! @return Whether allocation is successful.
            bool Init
                (
                size_t const aSize
                );

            //! @note Will wrap on integer overflow!
            //! @return Counter that is incremented for every byte the head of the list moves
            size_t GetBeginCounter() const;

            //! @return Value of byte at offset aIndex from the head, or 0 if out of bounds.
            Byte GetByte
                (
                size_t const aIndex
                ) const;

            //! @note Will wrap on integer overflow!
            //! @return Counter that is incremented for every byte the tail of the list moves
            size_t GetEndCounter() const;

            //! @return Nonmodifiable reference to the underlying buffer (unsorted).
            ByteBuffer const& GetRawBuffer() const;

            //! @return Number of bytes in the window
            size_t GetSize() const;

            //! Reset the window to an initialized state
            //!
            //! @note Does not clear the buffer's memory
            void Reset();

            //! Pushes new data into the window
            //!
            //! @pre If aSize > 0, aData must not be NULL.
            //! @post Any data that can't fit in the buffer will be discarded
            //! @post Begin and End counters are updated
            void PushData
                (
                Byte const* const aData,
                size_t const aSize
                );

        private:
            //! Copy aSize bytes from aData into the circular buffer
            //!
            //! @post Does NOT advance head or counters
            void CopyCirc
                (
                size_t const aBufIndex,
                Byte const* const aData,
                size_t const aSize
                );

            //! @return aIndex advanced aCount spaces, wrapping at the end of the buffer
            size_t IncrementIndex
                (
                size_t const aIndex,
                size_t const aCount
                ) const;

            ByteBuffer mBuf;
            size_t mBeginCounter;
            size_t mEndCounter;
            size_t mHead;
        };

    } // namespace Data

} // namespace Bfdp

#endif // Bfdp_Data_ByteWindow

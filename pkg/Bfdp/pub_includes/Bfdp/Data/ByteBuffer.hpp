/**
    BFDP Data Byte Buffer Declarations

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

#ifndef Bfdp_Data_ByteBuffer
#define Bfdp_Data_ByteBuffer

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"
#include "Bfdp/String.hpp"

namespace Bfdp
{

    namespace Data
    {

        //! Encapsulates a dynamic buffer of bytes
        //!
        //! This class is only intended to help with memory buffer options (resource cleanup,
        //! type casting, etc...).
        class ByteBuffer BFDP_FINAL
            : private NonAssignable
            , private NonCopyable
        {
        public:
            ByteBuffer();

            ~ByteBuffer();

            //! (Re)allocate a buffer of aSize
            //!
            //! @note Does not preserve existing content.
            //! @return Whether allocation is successful.
            bool Allocate
                (
                size_t const aSize
                );

            void Clear();

            //! Copy from an external buffer
            //!
            //! @return Number of bytes copied
            size_t CopyFrom
                (
                void const* const aPtr,
                size_t const aSize
                );

            void Delete();

            Byte const* GetConstPtr() const;

            template< class T >
            T const* GetConstPtrT() const
            {
                return reinterpret_cast< T const* >( mPtr );
            }

            Byte* GetPtr();

            template< class T >
            T* GetPtrT()
            {
                return reinterpret_cast< T* >( mPtr );
            }

            size_t GetSize() const;

            //! @return The buffer as a string, up to aBytes long (or the whole thing if 0).
            std::string GetString
                (
                size_t const aBytes = 0
                );

            //! Set all values of the buffer to the given value
            void MemSet
                (
                Byte const aValue
                );

            //! Swap the contents of the two buffers
            void Swap
                (
                ByteBuffer& aOther
                );

            Byte& operator []
                (
                size_t const aIndex
                );

            Byte const& operator []
                (
                size_t const aIndex
                ) const;

        private:
            Byte* mPtr;
            size_t mSize;
        };

    } // namespace Data

} // namespace Bfdp

#endif // Bfdp_Data_ByteBuffer

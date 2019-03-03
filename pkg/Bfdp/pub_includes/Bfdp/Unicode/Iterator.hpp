/**
    BFDP Unicode Iterator Declarations

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

#ifndef Bfdp_Unicode_Iterator
#define Bfdp_Unicode_Iterator

// Internal includes
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "Bfdp/String.hpp"
#include "Bfdp/Unicode/Common.hpp"
#include "Bfdp/Unicode/IConverter.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        //! Base class for conversion iterators
        //!
        //! This class provides read-only forward moving access to code points from a buffer of
        //! bytes using the given converter.  Once the iterator reaches the end of the buffer,
        //! it is no longer valid.
        class IteratorBase
        {
        public:
            //! Current value
            //!
            //! @pre The iterator must be valid
            //! @return the code point at the current buffer position.
            CodePoint operator *() const;

            //! Buffer address
            //!
            //! @pre The iterator must be valid
            //! @return A const pointer to the current buffer position (where the current value
            //!     is read from).
            Byte const* operator &() const;

            //! Pre-increment and Dereference
            //!
            //! @return The code point dereferenced after advancing to the next pointer
            CodePoint operator ++() const;

            //! Dereference and Post-increment
            //!
            //! @note The int parameter is required by the C++ standard to differentiate the
            //!     operators, but unused.
            //! @return The code point dereferenced before advancing to the next pointer
            CodePoint operator ++(int) const;

            //! Validity test
            //!
            //! @return true if the iterator is valid, false otherwise.
            operator bool() const;

            //! Get the current index
            //!
            //! This returns the index of the current code point, starting at 0 and
            //! incrementing for each code point discovered.
            //!
            //! @note This is NOT the array index of the byte in the buffer.
            //! @return The index of the current code point
            size_t GetIndex() const;

            //! @return true if the iterator had a conversion error, false otherwise.
            bool HasError() const;

        protected:
            //! Default constructor (invalid)
            IteratorBase();

            //! Destructor (only delete from the concrete class)
            ~IteratorBase();

            void Init
                (
                IConverter* const aConverter,
                void const* const aBuffer,
                size_t const aSize
                );

            enum FlagsType
            {
                F_ERROR_BIT,
                F_NEED_CONVERT_BIT,

                F_COUNT,

                F_ERROR = 1 << F_ERROR_BIT,
                F_NEED_CONVERT = 1 << F_NEED_CONVERT_BIT,

                F_INIT_VAL = F_NEED_CONVERT
            };
            BFDP_CTIME_ASSERT( F_COUNT <= ( BitManip::BitsPerByte * sizeof( int ) ), "Too many flags" );

        private:
            void Advance() const;

            void Convert() const;

            mutable CodePoint mCodePoint;
            mutable size_t mCodePointSize;
            IConverter* mConverter;
            mutable size_t mIndex;
            mutable int mFlags;
            mutable Byte const* mPtr;
            mutable size_t mRemain;
        };

        //! Iterator for a std::string using the specified converter
        template< class ConverterType >
        class StrIterator
            : public IteratorBase
        {
        public:
            StrIterator
                (
                std::string const& aString
                )
                : IteratorBase()
                , mConverter()
                , mString( aString )
            {
                Init( &mConverter, mString.c_str(), mString.size() );
            }

        private:
            typename ConverterType mConverter;
            std::string const mString;
        };

    } // namespace Unicode

} // namespace Bfdp

#endif // Bfdp_Unicode_Iterator

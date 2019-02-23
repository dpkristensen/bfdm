/**
    BFDP Unicode Iterator Definitions

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

// Base includes
#include "Bfdp/Unicode/Iterator.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        CodePoint IteratorBase::operator *() const
        {
            Convert();
            return mCodePoint;
        }

        Byte const* IteratorBase::operator &() const
        {
            return mPtr;
        }

        CodePoint IteratorBase::operator ++() const
        {
            Advance();
            Convert();
            return mCodePoint;
        }

        CodePoint IteratorBase::operator ++(int) const
        {
            Convert();
            CodePoint c = mCodePoint;
            Advance();
            return c;
        }

        IteratorBase::operator bool() const
        {
            return mRemain != 0;
        }

        SizeT IteratorBase::GetIndex() const
        {
            return mIndex;
        }

        bool IteratorBase::HasError() const
        {
            return 0 != ( mFlags & F_ERROR );
        }

        IteratorBase::IteratorBase()
            : mCodePoint( InvalidCodePoint )
            , mCodePointSize( 0 )
            , mConverter( NULL )
            , mFlags( F_INIT_VAL )
            , mIndex( 0 )
            , mPtr( NULL )
            , mRemain( 0 )
        {
        }

        IteratorBase::~IteratorBase()
        {
        }

        void IteratorBase::Init
            (
            IConverter* const aConverter,
            void const* const aBuffer,
            SizeT const aSize
            )
        {
            mConverter = aConverter;
            mPtr = reinterpret_cast< Byte const* >( aBuffer );
            mRemain = aSize;
        }

        void IteratorBase::Advance() const
        {
            mPtr += mCodePointSize;
            mRemain -= mCodePointSize;
            ++mIndex;

            mCodePoint = InvalidCodePoint;
            mCodePointSize = 0;
            mFlags |= F_NEED_CONVERT;
        }

        void IteratorBase::Convert() const
        {
            if( ( 0 == ( mFlags & F_NEED_CONVERT ) ) ||
                ( 0 == mRemain ) )
            {
                return;
            }

            mCodePointSize = mConverter->ConvertBytes( mPtr, mRemain, mCodePoint );
            if( 0 == mCodePointSize )
            {
                mFlags |= F_ERROR;
            }
            mFlags &= ~F_NEED_CONVERT;
        }

    } // namespace Unicode

} // namespace Bfdp

/**
    BFSDL Parser Unique Pointer Declaration

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

#ifndef Bfdp_Algorithm_UniquePtr
#define Bfdp_Algorithm_UniquePtr

// Base Includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

namespace Bfdp
{

    //! Unique Pointer
    //!
    //! Wrapper class for single-instance classes to enable scope-based lifetime management of
    //! heap-allocated objects.
    template< class T >
    class UniquePtr
    {
    public:
        //! Default constructor
        UniquePtr()
            : mInstancePtr( NULL )
        {
        }

        //! Construct a pointer from an existing raw pointer
        //!
        //! @note The UniquePtr object takes ownership of the object after this call.
        UniquePtr
            (
            T* aObject
            )
            : mInstancePtr( NULL )
        {
            *this = aObject;
        }

        //! Copy Constructor
        //!
        //! @note The original object no longer owns the memory
        UniquePtr
            (
            UniquePtr& aOther
            )
            : mInstancePtr( NULL )
        {
            *this = aOther;
        }

        virtual ~UniquePtr()
        {
            Release();
        }

        T* Get()
        {
            return mInstancePtr;
        }

        T const* Get() const
        {
            return mInstancePtr;
        }

        void Release() const
        {
            delete mInstancePtr;
            mInstancePtr = NULL;
        }

        UniquePtr< T >& operator =
            (
            UniquePtr& aOther
            )
        {
            Release();
            mInstancePtr = aOther.mInstancePtr;
            aOther.mInstancePtr = NULL;
            return *this;
        }

        UniquePtr< T >& operator =
            (
            T* aObject
            )
        {
            Release();
            mInstancePtr = aObject;
            return *this;
        }

        T* operator ->()
        {
            return mInstancePtr;
        }

        T const* operator ->() const
        {
            return mInstancePtr;
        }

        operator T*()
        {
            return mInstancePtr;
        }

        operator T const*() const
        {
            return mInstancePtr;
        }

    private:
        mutable T* mInstancePtr;
    };

} // namespace Bfdp

#endif // Bfdp_Algorithm_UniquePtr

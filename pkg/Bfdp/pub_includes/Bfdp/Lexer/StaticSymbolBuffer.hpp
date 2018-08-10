/**
    BFDP Lexer StaticSymbolBuffer

    Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef Bfdp_StaticSymbolBuffer
#define Bfdp_StaticSymbolBuffer

// Base types
#include "Bfdp/Lexer/ISymbolBuffer.hpp"
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// External includes
#include <cstring>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Lexer/ISymbolBuffer.hpp"

namespace Bfdp
{

    namespace Lexer
    {

        //! A fixed-size buffer to contain symbol data
        //!
        //! @tparam MAX_SYMBOL_LENGTH Length of the buffer (in Chars) to allocate
        template
            <
            SizeT MAX_SYMBOL_LENGTH
            >
        class StaticSymbolBuffer
            : private NonAssignable
            , private NonCopyable
            , public ISymbolBuffer
        {
        public:
            //! Constructor
            StaticSymbolBuffer()
                : mIndex( 0 )
            {
                std::memset( mBuffer, 0, sizeof( mBuffer ) );
            }

            //! @copydoc ISymbolBuffer::AddSymbol()
            virtual bool Add
                (
                Unicode::CodePoint const aSymbol
                )
            {
                if( mIndex < MAX_SYMBOL_LENGTH )
                {
                    mBuffer[mIndex] = aSymbol;
                    ++mIndex;
                    return true;
                }

                return false;
            }

            //! @copydoc ISymbolBuffer::Clear()
            virtual void Clear()
            {
                if( !IsEmpty() )
                {
                    std::memset( mBuffer, 0, mIndex * sizeof( Unicode::CodePoint ) );
                }
                mIndex = 0;
            }

            //! @copydoc ISymbolBuffer::GetSize()
            virtual SizeT GetSize() const
            {
                return mIndex;
            }

            //! @copydoc ISymbolBuffer::GetSymbolAt()
            virtual Unicode::CodePoint GetSymbolAt
                (
                SizeT const aIndex
                ) const
            {
                if( aIndex >= GetSize() )
                {
                    // Macro cannot be used in header file since the module cannot be scoped to a
                    // single compilation unit.
                    ErrorReporter::ReportMisuseError( "Lexer::StaticSymbolBuffer", __LINE__, "Index out of range" );
                    return Unicode::InvalidCodePoint;
                }
                return mBuffer[aIndex];
            }

            //! @copydoc ISymbolBuffer::IsEmpty()
            virtual bool IsEmpty() const
            {
                return ( mIndex == 0 );
            }

        private:
            Unicode::CodePoint mBuffer[ MAX_SYMBOL_LENGTH + 1 ];
            SizeT mIndex;
        };

    } // namespace Lexer

} // namespace Bfdp

#endif // Bfdp_StaticSymbolBuffer

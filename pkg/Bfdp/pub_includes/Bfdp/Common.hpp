/**
    BFDP Common declarations

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

#ifndef Bfdp_Common
#define Bfdp_Common

// External includes
#include <cstddef>

namespace Bfdp
{

    //! Represents a size for memory operations
    typedef std::size_t SizeT;

    typedef int SInt;
    typedef char SInt8;
    typedef short SInt16;
    typedef int SInt32;
    typedef long long SInt64;

    typedef unsigned int UInt;
    typedef unsigned char UInt8;
    typedef unsigned short UInt16;
    typedef unsigned int UInt32;
    typedef unsigned long long UInt64;

    static SInt8 const SInt8Max = static_cast< SInt8 >( 0x7FU );
    static SInt8 const SInt8Min = static_cast< SInt8 >( 0x80U );
    static UInt8 const UInt8Max = static_cast< UInt8 >( 0xFFU );
    static UInt8 const UInt8Min = static_cast< UInt8 >( 0x00U );
    static SInt16 const SInt16Max = static_cast< SInt16 >( 0x7FFFU );
    static SInt16 const SInt16Min = static_cast< SInt16 >( 0x8000U );
    static UInt16 const UInt16Max = static_cast< UInt16 >( 0xFFFFU );
    static UInt16 const UInt16Min = static_cast< UInt16 >( 0x0000U );
    static SInt32 const SInt32Max = static_cast< SInt32 >( 0x7FFFFFFFU );
    static SInt32 const SInt32Min = static_cast< SInt32 >( 0x80000000U );
    static UInt32 const UInt32Max = static_cast< UInt32 >( 0xFFFFFFFFU );
    static UInt32 const UInt32Min = static_cast< UInt32 >( 0x00000000U );
    static SInt64 const SInt64Max = static_cast< SInt64 >( 0x7FFFFFFFFFFFFFFFULL );
    static SInt64 const SInt64Min = static_cast< SInt64 >( 0x8000000000000000ULL );
    static UInt64 const UInt64Max = static_cast< UInt64 >( 0xFFFFFFFFFFFFFFFFULL );
    static UInt64 const UInt64Min = static_cast< UInt64 >( 0x0000000000000000ULL );

    template< class T >
    inline bool IsWithinRange
        (
        T const aMin,
        T const aValue1,
        T const aMax
        )
    {
        return ( aMin <= aValue1 ) && ( aValue1 <= aMax );
    }

} // namespace Bfdp

#endif // Bfdp_Common

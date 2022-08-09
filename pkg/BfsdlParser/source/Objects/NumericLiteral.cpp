/**
    BFSDL Parser Numeric Literal Definitions

    Copyright 2016-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

// Base Includes
#include "BfsdlParser/Objects/NumericLiteral.hpp"

// Internal Includes
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/BitManip/GenericBitStream.hpp"
#include "Bfdp/Compiler.hpp"
#include "Bfdp/Macros.hpp"

#define BFDP_MODULE "Objects::NumericLiteral"

namespace BfsdlParser
{

    namespace Objects
    {

        using namespace Bfdp;

        NumericLiteral::NumericLiteral()
            : mRadix( Data::InvalidRadix )
        {
        }

        Data::FlexNumber::Component const& NumericLiteral::GetBase() const
        {
            return mNumber.base;
        }

        Data::FlexNumber::Component const& NumericLiteral::GetExponent() const
        {
            return mNumber.exponent;
        }

        Data::FlexNumber::Component const& NumericLiteral::GetSignificand() const
        {
            return mNumber.significand;
        }

        std::string NumericLiteral::GetStr
            (
            bool const aVerbose
            ) const
        {
            // Casting away const is safe, GetStr() doesn't modify the underlying objects
            return const_cast< Data::FlexNumber* >( &mNumber )->GetStr( aVerbose );
        }

        bool NumericLiteral::HasRadix() const
        {
            return mRadix != Data::InvalidRadix;
        }

        bool NumericLiteral::IsDefined() const
        {
            return mNumber.IsDefined();
        }

        void NumericLiteral::Reset()
        {
            mRadix = Data::InvalidRadix;
            mNumber.Reset();
        }

        bool NumericLiteral::SetDefaultBase()
        {
            char const* base2 = ( mRadix == 2 ) ? "10" : "2";
            // IEEE floats always use base 2
            mNumber.base.sign = Data::Sign::Positive;
            return mNumber.base.integral.Set( base2, mRadix );
        }

        bool NumericLiteral::SetExponentDigits
            (
            std::string const& aDigits
            )
        {
            BFDP_RETURNIF_V( !mNumber.base.IsDefined(), false );

            if( !mNumber.exponent.integral.Set( aDigits, mRadix ) )
            {
                return false;
            }

            return true;
        }

        void NumericLiteral::SetExponentSign
            (
            Data::Sign const aSign
            )
        {
            mNumber.exponent.sign = aSign;
        }

        bool NumericLiteral::SetSignificandFractionalDigits
            (
            std::string const& aDigits
            )
        {
            return mNumber.significand.fractional.Set( aDigits, mRadix );
        }

        bool NumericLiteral::SetSignificandIntegralDigits
            (
            std::string const& aDigits
            )
        {
            return mNumber.significand.integral.Set( aDigits, mRadix );
        }

        void NumericLiteral::SetSignificandSign
            (
            Data::Sign const aSign
            )
        {
            mNumber.significand.sign = aSign;
        }

        void NumericLiteral::SetRadix
            (
            Data::RadixType const aRadix
            )
        {
            mRadix = aRadix;
        }

        bool NumericLiteral::GetUintImpl
            (
            void* const aOut,
            size_t const aMaxBits
            ) const
        {
            BFDP_RETURNIF_V( !mNumber.IsIntegral(), false );

            uint64_t val;
            BFDP_RETURNIF_V( !mNumber.significand.GetUint64( val, aMaxBits ), false );

            size_t numBytes = BitManip::BitsToBytes( aMaxBits );
            #if( BFDP_HOST_ENDIAN_LE() )
                std::memcpy( aOut, &val, numBytes );
            #else
                std::memcpy( aOut, BFDP_OFFSET_BYTE( &val, sizeof( val ) - numBytes ), numBytes );
            #endif

            return true;
        }

    } // namespace Objects

} // namespace BfsdlParser

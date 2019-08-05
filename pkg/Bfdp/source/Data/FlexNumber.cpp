/**
    BFDP Data FlexNumber Definitions

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

// Base Includes
#include "Bfdp/Data/FlexNumber.hpp"

// External Includes
#include <sstream>

#define BFDP_MODULE "Data::FlexNumber"

namespace Bfdp
{

    namespace Data
    {

        bool FlexNumber::Component::IsDefined() const
        {
            return sign.IsSpecified() && ( integral.IsDefined() || fractional.IsDefined() );
        }

        std::string FlexNumber::Component::GetStr
            (
            bool const aVerbose
            ) const
        {
            std::ostringstream out;

            out << ( aVerbose ? sign.GetExplicitStr() : sign.GetStr() );

            out << ( integral.IsDefined() ? integral.GetStr() : "0" );

            if( fractional.IsDefined() )
            {
                out << "." << fractional.GetStr();
            }

            return out.str();
        }

        bool FlexNumber::Component::IsIntegral() const
        {
            return sign.IsSpecified() && integral.IsDefined() && !fractional.IsDefined();
        }

        void FlexNumber::Component::Reset()
        {
            sign = Sign::Unspecified;
            integral.Reset();
            fractional.Reset();
        }

        std::string FlexNumber::GetStr
            (
            bool const aVerbose
            ) const
        {
            std::ostringstream out;

            bool hasSignificand = significand.IsDefined();
            bool hasOrder = base.IsDefined() && exponent.IsDefined();

            if( hasSignificand )
            {
                out << significand.GetStr( aVerbose );
            }

            if( hasSignificand && hasOrder )
            {
                out << " x ";
            }

            if( hasOrder )
            {
                out << base.GetStr( aVerbose ) << " ^ " << exponent.GetStr( aVerbose );
            }

            return out.str();
        }

        bool FlexNumber::HasExponent() const
        {
            return base.IsDefined() && exponent.IsDefined();
        }

        bool FlexNumber::HasSignificand() const
        {
            return significand.IsDefined();
        }

        bool FlexNumber::IsDefined() const
        {
            return HasSignificand() || HasExponent();
        }

        bool FlexNumber::IsIntegral() const
        {
            return significand.IsIntegral() && !HasExponent();
        }

        void FlexNumber::Reset()
        {
            significand.Reset();
            base.Reset();
            exponent.Reset();
        }

    } // namespace Data

} // namespace Bfdp

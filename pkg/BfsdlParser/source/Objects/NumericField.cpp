/**
    BFSDL Parser Numeric Field Definition

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
#include "BfsdlParser/Objects/NumericField.hpp"

// External Includes
#include <sstream>

namespace BfsdlParser
{

    namespace Objects
    {

        /* static */ NumericFieldPtr NumericField::StaticCast
            (
            IObjectPtr const aObject
            )
        {
            FieldPtr basePtr = Field::StaticCast( aObject );
            if( ( basePtr == NULL ) ||
                ( basePtr->GetFieldType() != FieldType::Numeric ) )
            {
                return NULL;
            }

            return std::static_pointer_cast< NumericField >( aObject );
        }

        NumericField::NumericField
            (
            std::string const& aName,
            NumericFieldProperties const& aProps
            )
            : Field( aName, FieldType::Numeric )
            , mProps( aProps )
        {
        }

        NumericField::~NumericField()
        {
        }

        NumericFieldProperties const& NumericField::GetNumericFieldProperties() const
        {
            return mProps;
        }

        std::string const& NumericField::GetTypeStr() const
        {
            if( mTypeStr.empty() )
            {
                std::stringstream ss;
                ss << ( mProps.mSigned ? "s" : "u" );
                ss << mProps.mIntegralBits;
                if( mProps.mFractionalBits != 0 )
                {
                    ss << "." << mProps.mFractionalBits;
                }
                mTypeStr = ss.str();
            }

            return mTypeStr;
        }

    } // namespace Objects

} // namespace BfsdlParser

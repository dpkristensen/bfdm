/**
    BFSDL Parser String Field Definition

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
#include "BfsdlParser/Objects/StringField.hpp"

// External Includes
#include <sstream>

namespace BfsdlParser
{

    namespace Objects
    {

        /* static */ StringFieldPtr StringField::StaticCast
            (
            IObjectPtr& aObject
            )
        {
            FieldPtr basePtr = Field::StaticCast( aObject );
            if( ( basePtr == NULL ) ||
                ( basePtr->GetFieldType() != FieldType::String ) )
            {
                return NULL;
            }

            return std::static_pointer_cast< StringField >( aObject );
        }

        StringField::StringField
            (
            std::string const& aName,
            Bfdp::Unicode::CodePoint const aTermChar,
            bool const aAllowUnterminated,
            Bfdp::Unicode::CodingId const aCode
            )
            : Field( aName, FieldType::String )
            , mAllowUnterminated( aAllowUnterminated )
            , mTermChar( aTermChar )
            , mCode( aCode )
        {
        }

        StringField::~StringField()
        {
        }

        /* virtual */ std::string StringField::GetConcreteTypeStr() const
        {
            return "b";
        }

        std::string const& StringField::GetTypeStr() const
        {
            if( mTypeStr.empty() )
            {
                std::stringstream ss;
                ss << "string:" << GetConcreteTypeStr();
                if( mTermChar != Bfdp::Unicode::InvalidCodePoint )
                {
                    ss << ":t" << mTermChar;
                }
                if( mAllowUnterminated )
                {
                    ss << ":tu";
                }
                // Encoding info should be last
                ss << ";" << Bfdp::Unicode::GetCodingTypeStr( mCode );
                mTypeStr = ss.str();
            }

            return mTypeStr;
        }

    } // namespace Objects

} // namespace BfsdlParser

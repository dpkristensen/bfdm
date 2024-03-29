/**
    BFSDL Parser Property Definition

    Copyright 2022, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "BfsdlParser/Objects/Property.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        /* static */ PropertyPtr Property::StaticCast
            (
            IObjectPtr const aObject
            )
        {
            return ( ObjectType::Property == aObject->GetType() )
                ? std::static_pointer_cast< Property >( aObject )
                : NULL;
        }

        Property::~Property()
        {
        }

        Property::Property
            (
            std::string const& aName
            )
            : ObjectBase( aName, ObjectType::Property )
        {
        }

        Bfdp::Data::ByteBuffer const& Property::GetData() const
        {
            return mData;
        }

        std::string Property::GetString() const
        {
            return mData.GetString();
        }

        bool Property::SetData
            (
            Bfdp::Byte const* const aData,
            size_t const aSize
            )
        {
            if( !mData.Allocate( aSize ) )
            {
                return false;
            }
            mData.CopyFrom( aData, aSize );
            return true;
        }

        bool Property::SetString
            (
            std::string const& aValue
            )
        {
            return SetData( Bfdp::Char( aValue.c_str() ), aValue.length() );
        }

    } // namespace Objects

} // namespace BfsdlParser

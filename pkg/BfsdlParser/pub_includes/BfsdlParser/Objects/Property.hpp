/**
    BFSDL Parser Property Declaration

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

#ifndef BfsdlParser_Objects_Property
#define BfsdlParser_Objects_Property

// Base Includes
#include "BfsdlParser/Objects/ObjectBase.hpp"

// External Includes
#include <memory>
#include <string>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Data/ByteBuffer.hpp"
#include "BfsdlParser/Objects/Common.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        class Property;

        typedef std::shared_ptr< Property > PropertyPtr;

        //! Property
        //!
        //! Stores metadata about the parent node which is applicable to the node and all descendents.
        //!
        //! The interpretation of a property is tied to its name, therefore properties must have
        //! canonical names and interpretations.  Also, Properties should be named starting with
        //! a "." to avoid collision with valid field names.
        //!
        //! The primary difference between a property and a field is that properties are defined
        //! by the BFSDL specification and do not correlate to objects in the data stream like
        //! fields.
        class Property
            : public ObjectBase
        {
        public:
            //! @return Pointer to Property object if aObject is a Property, otherwise NULL.
            static PropertyPtr StaticCast
                (
                IObjectPtr const aObject
                );

            Property
                (
                std::string const& aName
                );

            virtual ~Property();

            //! Set the value as a raw byte buffer
            //!
            //! @return Whether the value was set successfully
            bool SetData
                (
                Bfdp::Byte const* const aData,
                size_t const aSize
                );

            //! Helper function to set a numeric value
            //!
            //! @return Whether the value was set successfully
            template< typename T >
            bool SetNumericValue
                (
                typename T const& aValue
                )
            {
                typename T value = aValue;
                return SetData( reinterpret_cast< Bfdp::Byte const* >( &value ), sizeof( value ) );
            }

            //! Helper function to get a numeric value
            //!
            //! @return Value, or 0 on error
            template< typename T >
            bool GetNumericValue
                (
                typename T& aValue
                ) const
            {
                if( mData.GetSize() != sizeof( aValue ) )
                {
                    return false;
                }

                std::memcpy( &aValue, mData.GetConstPtr(), sizeof( aValue ) );
                return true;
            }

            //! @return Access to the data buffer containing the value
            Bfdp::Data::ByteBuffer const& GetData() const;

        protected:
            Bfdp::Data::ByteBuffer mData;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_Property

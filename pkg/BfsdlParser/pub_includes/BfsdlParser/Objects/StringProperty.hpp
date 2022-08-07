/**
    BFSDL Parser String Property Declaration

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

#ifndef BfsdlParser_Objects_StringProperty
#define BfsdlParser_Objects_StringProperty

// Base Includes
#include "BfsdlParser/Objects/Property.hpp"

// External Includes
#include <memory>
#include <string>

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        class StringProperty;

        typedef std::shared_ptr< StringProperty > StringPropertyPtr;

        //! String Property
        //!
        //! Specialization of Property to store UTF-8 string values.
        class StringProperty
            : public Property
        {
        public:
            //! @pre Caller must know if this property should be interpreted as a string based on the name.
            //! @return Pointer to StringProperty object if aObject is a Property, otherwise NULL.
            static StringPropertyPtr StaticCast
                (
                IObjectPtr const aObject
                );

            StringProperty
                (
                std::string const& aName
                );

            virtual ~StringProperty();

            //! @return Value of the string (canonical)
            std::string GetValue() const;

            //! Set the value as a UTF-8 string
            //!
            //! @return Whether the value was set successfully
            bool SetValueUtf8
                (
                std::string const& aValue
                );
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_Property

/**
    BFSDL Parser Object Tree Container

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

#ifndef BfsdlParser_Objects_Tree
#define BfsdlParser_Objects_Tree

// Base includes
#include "BfsdlParser/Objects/ObjectBase.hpp"

// External includes
#include <list>
#include <map>
#include <memory>
#include <string>

// Internal includes
#include "Bfdp/Algorithm/Calc.hpp"
#include "Bfdp/Algorithm/HashedString.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Objects/IObject.hpp"
#include "BfsdlParser/Objects/Field.hpp"
#include "BfsdlParser/Objects/Property.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        class Tree;

        typedef std::shared_ptr< Tree > TreePtr;

        //! Object Tree Container
        //!
        //! Encapsulates a collection of objects
        class Tree
            : public ObjectBase
        {
        public:
            Tree();

            virtual ~Tree();

            //! @return Pointer to the object if added to the tree, NULL otherwise.
            IObjectPtr Add
                (
                IObjectPtr const aNode
                );

            //! @note This does NOT do a recursive lookup.
            //! @return Pointer to the property object if found in the tree, NULL otherwise.
            PropertyPtr FindProperty
                (
                std::string const& aName
                );

            //! @return The result of FindProperty, cast as a pointer to a specific type of property
            template< class T >
            std::shared_ptr< T > FindPropertyT
                (
                std::string const& aName
                )
            {
                return typename T::StaticCast( FindProperty( aName ) );
            }

            //! Convenience function to get the value of a numeric property
            //!
            //! @post aValue is undefined on failure.
            //! @return Whether the value was obtained.
            template< typename T >
            bool Tree::GetNumericProperty
                (
                std::string const& aName,
                typename T& aValue
                )
            {
                PropertyPtr p = FindProperty( aName );
                return ( p ) && ( p->GetNumericValue< T >( aValue ) );
            }

            //! Convenience function to get the value of a numeric property with a default
            //!
            //! @return The value if found, or aDefault on error.
            template< typename T >
            typename T Tree::GetNumericPropertyWithDefault
                (
                std::string const& aName,
                typename T const aDefault
                )
            {
                typename T value;
                return GetNumericProperty< T >( aName, value )
                    ? value
                    : aDefault;
            }

            //! Convenience function to get the value of a string property
            //!
            //! If you need to distinguish between the absence of a property vs empty
            //! string, use FindProperty() directly.
            //!
            //! @return The string value of a property, or empty string if not found.
            std::string GetStringProperty
                (
                std::string const& aName
                );

            //! Iterate over fields and call the FieldCb for each
            void IterateFields
                (
                FieldCb const aFunc,
                void* const aArg
                );

            //! Iterate over properties and call the PropertyCb for each
            void IterateProperties
                (
                PropertyCb const aFunc,
                void* const aArg
                );

        private:
            typedef std::multimap
                <
                Bfdp::Algorithm::HashedString,
                PropertyPtr,
                Bfdp::Algorithm::HashedString::StrictWeakCompare
                > PropertyMap;

            typedef std::list< FieldPtr > FieldList;

            //! Fields are sequential data elements; so this must be ordered and can be duplicated.
            FieldList mFieldList;

            //! Properties are metadata about the scope of this tree; un-ordered and unique.
            PropertyMap mPropertyMap;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_Tree

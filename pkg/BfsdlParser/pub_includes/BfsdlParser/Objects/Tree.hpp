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
#include <map>
#include <string>

// Internal includes
#include "Bfdp/Algorithm/Calc.hpp"
#include "Bfdp/Algorithm/HashedString.hpp"
#include "Bfdp/Macros.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Object Tree Container
        //!
        //! Encapsulates a collection of objects
        class Tree
            : public ObjectBase
        {
        public:
            Tree();

            virtual ~Tree();

            //! @note On success, lifecycle management of the node is transferred to the tree;
            //!     the original pointer will be NULL.
            //! @return Raw pointer to the object if added to the tree, NULL otherwise.
            IObject* Add
                (
                IObject::UPtr aNode
                );

            //! @note This does NOT do a recursive lookup.
            //! @return Pointer to the object if found in the tree, NULL otherwise.
            IObject* Find
                (
                std::string const& aName
                );

        private:
            typedef std::multimap
                <
                Bfdp::Algorithm::HashedString,
                IObject::UPtr,
                Bfdp::Algorithm::HashedString::StrictWeakCompare
                > NodeMap;

            NodeMap mMap;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_Tree

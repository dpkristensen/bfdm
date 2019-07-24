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

// Base Includes
#include "BfsdlParser/Objects/Tree.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        Tree::~Tree()
        {
        }

        IObject* Tree::Add
            (
            IObject::UPtr aNode
            )
        {
            BFDP_RETURNIF_V( aNode == NULL, NULL );
            BFDP_RETURNIF_V( Find( aNode->GetName() ) != NULL, NULL );

            NodeMap::iterator iter = mMap.insert
                (
                std::make_pair( aNode->GetId(), std::move( aNode ) )
                );
            BFDP_RETURNIF_V( iter == mMap.end(), NULL );

            return iter->second.get();
        }

        IObject* Tree::Find
            (
            std::string const& aName
            )
        {
            using Bfdp::Algorithm::HashedString;

            HashedString hs = HashedString( aName );
            NodeMap::iterator iter = mMap.find( hs );
            BFDP_RETURNIF_V( iter == mMap.end(), NULL );

            return iter->second.get();
        }

        Tree::Tree()
            : ObjectBase( std::string(), ObjectType::Tree )
        {
        }

    } // namespace Objects

} // namespace BfsdlParser

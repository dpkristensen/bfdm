/**
    BFSDL Parser Object Base Declaration

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

#ifndef BfsdlParser_Objects_ObjectBase
#define BfsdlParser_Objects_ObjectBase

// Base Includes
#include "BfsdlParser/Objects/IObject.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Algorithm/HashedString.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Base class for objects
        //!
        //! encapsulates functionality common to all Objects
        class ObjectBase
            : public IObject
        {
        public:
            virtual ~ObjectBase();

            BFDP_OVERRIDE( std::string const& GetName() const );

            BFDP_OVERRIDE( ObjectType::Id GetType() const );

        protected:
            ObjectBase
                (
                std::string const& aName,
                ObjectType::Id const aType
                );

            Bfdp::Algorithm::HashedString const mName;

            ObjectType::Id const mType;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_ObjectBase

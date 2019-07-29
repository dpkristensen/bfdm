/**
    BFSDL Parser Object Database

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

#ifndef BfsdlParser_Objects_Database
#define BfsdlParser_Objects_Database

// Internal Includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Macros.hpp"
#include "BfsdlParser/Objects/IObject.hpp"
#include "BfsdlParser/Objects/Tree.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        //! Object Database
        //!
        //! Keeps track of objects parsed from a BFSDL stream.
        class Database BFDP_FINAL
        {
        public:
            //! Opaque handle type to protect the database
            typedef void* Handle;

            static Handle BFDP_CONSTEXPR InvalidHandle = NULL;

            Database();

            ~Database();

            //! Add object
            //!
            //! Add aObject to the database as a child of aParent (or the root node if invalid).
            //!
            //! If aOutHandle is not NULL, then the output is set on success as follows:
            //! * If aObject is of type Tree, then aOutHandle is the new handle of the tree.
            //! * NULL otherwise
            bool Add
                (
                IObject::UPtr& aObject,
                Handle const aParent,
                Handle* const aOutHandle = NULL
                );

            Handle GetRoot();

            void Iterate
                (
                ObjectCb const aFunc,
                void* const aArg
                );

        private:
            Tree mRoot;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Object_Database

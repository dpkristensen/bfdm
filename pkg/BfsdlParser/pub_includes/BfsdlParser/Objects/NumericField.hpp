/**
    BFSDL Parser Numeric Field Declaration

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

#ifndef BfsdlParser_Objects_NumericField
#define BfsdlParser_Objects_NumericField

// Base Includes
#include "BfsdlParser/Objects/Field.hpp"

// Internal Includes
#include "BfsdlParser/Objects/Common.hpp"

namespace BfsdlParser
{

    namespace Objects
    {

        class NumericField;

        typedef std::shared_ptr< NumericField > NumericFieldPtr;

        //! Numeric Field
        //!
        //! Specialization of Field for numeric data types.
        class NumericField
            : public Field
        {
        public:
            static NumericFieldPtr StaticCast
                (
                IObjectPtr const aObject
                );

            NumericField
                (
                std::string const& aName,
                NumericFieldProperties const& aProps
                );

            virtual ~NumericField();

            NumericFieldProperties const& GetNumericFieldProperties() const;

            BFDP_OVERRIDE( std::string const& GetTypeStr() const );

        private:
            NumericFieldProperties mProps;
        };

    } // namespace Objects

} // namespace BfsdlParser

#endif // BfsdlParser_Objects_NumericField

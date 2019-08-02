/**
    BFDP Unicode Coding Map Declarations

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

#ifndef Bfdp_Unicode_CodingMap
#define Bfdp_Unicode_CodingMap

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/IConverter.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        //! Unique ID for supported codecs
        //!
        //! @see GetCodingId()
        typedef void* CodingId;

        static CodingId BFDP_CONSTEXPR InvalidCodingId = NULL;

        struct CodingFamily
        {
            enum Id
            {
                ASCII,
                HP,
                IBM,
                IEC,
                ISO,
                MS,
                UTF8,
                // TODO: UTF16, UTF32, UTF64

                Count
            };
        };

        bool IsValidCoding
            (
            std::string const& aCoding
            );

        //! @return A codec instantiated from a CodingId, or NULL on failure.
        IConverterPtr GetCodec
            (
            CodingId const aCodingId
            );

        //! @return A memory-efficient identifier for a supported codec,
        //!     or InvalidCodingId if not supported.
        CodingId GetCodingId
            (
            std::string const& aCoding
            );

    } // namespace Unicode

} // namespace Bfdp

#endif // Bfdp_Unicode_CodingMap

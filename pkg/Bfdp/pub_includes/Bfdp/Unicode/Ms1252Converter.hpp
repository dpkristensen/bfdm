/**
    BFDP Unicode to Microsoft Code Page 1252 Converter

    Copyright 2016-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef Bfdp_Unicode_Ms1252Converter
#define Bfdp_Unicode_Ms1252Converter

// Base includes
#include "Bfdp/Unicode/IConverter.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        //! Converter for Microsoft Code Page 1252
        class Ms1252Converter
            : public IConverter
        {
        public:
            //! @copydoc GetUnicodeFunc
            static bool GetUnicode
                (
                CodePoint const& aSymbolIn,
                CodePoint& aSymbolOut
                );

            Ms1252Converter();

            //! @copydoc IConverter::ConvertBytes
            BFDP_OVERRIDE( size_t ConvertBytes
                (
                Byte const* const aBytesIn,
                size_t const aByteCount,
                CodePoint& aSymbolOut
                ) );

            //! @copydoc IConverter::ConvertSymbol
            BFDP_OVERRIDE( size_t ConvertSymbol
                (
                CodePoint const& aSymbolIn,
                Byte* const aBytesOut,
                size_t const aByteCount
                ) );

            //! @copydoc IConverter::GetMaxBytes
            BFDP_OVERRIDE( size_t GetMaxBytes() const );

            //! @copydoc IConverter::GetTypeStr
            BFDP_OVERRIDE( std::string GetTypeStr() const );
        };

    } // namespace Unicode

} // namespace Bfdp

#endif // Bfdp_Unicode_Ms1252Converter

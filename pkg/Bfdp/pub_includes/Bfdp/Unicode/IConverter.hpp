/**
    BFDP Unicode Converter Interface

    Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef Bfdp_Unicode_IConverter
#define Bfdp_Unicode_IConverter

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/String.hpp"
#include "Bfdp/Unicode/Common.hpp"

namespace Bfdp
{

    namespace Unicode
    {

        //! Abstract interface for converting bytes
        class IConverter
        {
        public:
            //! Convert Bytes into a Unicode symbol
            //!
            //! @note aSymbolOut is only modified when a non-zero value is returned
            //! @return Number of bytes converted, or 0 if no conversion took place.
            virtual size_t ConvertBytes
                (
                Byte const* const aBytesIn, //!< [in] Pointer to bytes to convert
                size_t const aByteCount,     //!< [in] Number of bytes available
                CodePoint& aSymbolOut       //!< [out] Symbol to save converted byte
                ) = 0;

            //! Convert a Unicode symbol into Bytes
            //!
            //! @note aBytesOut is only modified when a non-zero value is returned
            //! @return Number of bytes converted, or 0 if no conversion took place.
            virtual size_t ConvertSymbol
                (
                CodePoint const& aSymbolIn, //!< [in] Symbol to convert
                Byte* const aBytesOut,      //!< [out] Where to write bytes
                size_t const aByteCount      //!< [in] Number of Bytes available to write to; must be at least GetMaxBytes()
                ) = 0;

            //! Get Max Bytes
            //!
            //! @note This will always be less than or equal to MaxBytesForConversion
            //! @return the maximum number of bytes required to convert all possible symbols
            virtual size_t GetMaxBytes() const = 0;

        protected:
            virtual ~IConverter()
            {
            }
        };

    } // namespace Unicode

} // namespace Bfdp

#endif // Bfdp_Unicode_IConverter

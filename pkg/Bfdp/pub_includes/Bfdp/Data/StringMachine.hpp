/**
    BFDP Data StringMachine Declarations

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

#ifndef Bfdp_Data_StringMachine
#define Bfdp_Data_StringMachine

// External Includes
#include <sstream>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/IConverter.hpp"

namespace Bfdp
{

    namespace Data
    {

        //! String Machine
        //!
        //! This class encapsulates a string, which can be modified dynamically and perform output
        //! conversion.
        class StringMachine BFDP_FINAL
        {
        public:
            StringMachine();

            //! Append a string decoded with the given converter
            //!
            //! @post IsDefined() will return true if successful
            //! @return true if successful, false otherwise.
            bool AppendString
                (
                Unicode::IConverter& aConverter, //!< [in] Converter to use
                std::string const& aIn //!< [in] String to add
                );

            //! Append a Unicode symbol with the given code point
            //!
            //! @post IsDefined() will return true if successful
            //! @return true if successful, false otherwise.
            bool AppendUnicode
                (
                Unicode::CodePoint const aCodePoint
                );

            //! Append a UTF8 encoded string to the buffer
            //!
            //! @post IsDefined() will return true
            void AppendUtf8
                (
                std::string const& aValue
                );

            //! @return A string encoded by the given converter
            bool GetString
                (
                Unicode::IConverter& aConverter, //!< [in] Converter to use
                std::string& aOut //!< [out] Where to save the output
                ) const;

            //! @return A UTF8-encoded string in a 2-byte hexadecimal form with an optional
            //!     per-byte prefix and separator.
            std::string GetUtf8HexString
                (
                std::string const& aSeparator = std::string(),
                std::string const& aPrefix = std::string()
                ) const;

            //! @return A UTF8-encoded string
            std::string const& GetUtf8String() const;

            bool IsDefined() const;

            bool IsEmpty() const;

            void Reset();

            //! Mark the data as Defined
            //!
            //! @post IsDefined() will return true
            void SetDefined();

        private:
            char GetHex
                (
                char const aNibble
                ) const;

            void Pack() const;

            //! Whether a value has been added
            bool mDefined;

            // Internal cache for read optimization; UTF8 format
            mutable std::string mCache;

            // Buffer used to build the string; UTF8 format.
            mutable std::ostringstream mBuffer;
        };

    } // namespace Data

} // namespace Bfdp

#endif // Bfdp_Data_StringMachine

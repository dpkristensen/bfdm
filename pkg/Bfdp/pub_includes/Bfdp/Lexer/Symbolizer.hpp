/**
    BFDP Lexer Symbolizer Declaration

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

#ifndef Bfdp_Lexer_Symbolizer
#define Bfdp_Lexer_Symbolizer

// Base types
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// External includes
#include <list>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Lexer/ISymbolObserver.hpp"
#include "Bfdp/Lexer/ISymbolBuffer.hpp"
#include "Bfdp/Lexer/SymbolCategoryMap.hpp"
#include "Bfdp/Unicode/IConverter.hpp"

namespace Bfdp
{

    namespace Lexer
    {

        //! Symbolizer
        //!
        //! This class is the foundation of all parsers used to read grammar.  Parse() takes data
        //! from the text file, converts multi-byte sequences, and categorizes the symbols into
        //! buckets based on the categories provided.
        //!
        //! Symbols from the same category are combined together into a string and reported as the
        //! final string to the observer.  The intended flow for using the Symbolizer is as follows:
        //!
        //! 1. Create an object which implements ILexerObserver
        //! 2. Allocate objects as members or on the stack:
        //!     a. An ISymbolBuffer derivative, which provides storage space for parsing symbols
        //!        (ex: StaticSymbolBuffer, which provides a statically-allocated array for
        //!        storage).
        //!        The maximum length of any symbol and uncategorized text is determined by this
        //!        class.
        //!     b. An IByteConverter derivative, which provides conversion to Unicode.
        //!     c. All category objects which define unique sets of symbols which are desired to be
        //!        parsed.  An enumeration can be used to provide values for the category numbers.
        //!     d. The Symbolizer instance
        //! 3. Add the categories owned by the class to the Symbolizer.
        //! 4. Start at offset 0 in a stream (which could be a file).
        //! 5. Call Parse with data chunks read from the stream at its current position.  The
        //!    ILexerObserver methods will be called during this time from the same thread.
        //!     a. If Parse() returns true, increment the stream position by the number of bytes
        //!        read and repeat step 5 until no more data remains.
        //!     b. If Parse() returns false, an error occurred and the number of bytes read will
        //!        correspond to the stream offset where the problem occurred.  Parsing cannot
        //!        continue.
        //! 6. When all data has been read from the stream successfully, call EndParse() to finalize
        //!    processing of the cached data.
        //!
        //! After a complete parsing operation, a new operation can begin on the same instance without
        //! having to set up the categories and observer again (provided the objects still exist).
        //!
        //! If the last operation was unsuccessful, Reset() must be used before beginning again.
        //!
        //! For parsing a context-independent grammar, only one parser is needed.  But for a
        //! context-dependent grammar such as BFSDL, multiple parser objects will be needed; in the
        //! latter case, it is recommended to use a different Symbolizer instance for each context's
        //! parser.
        class Symbolizer
            : private NonAssignable
            , private NonCopyable
        {
        public:
            //! Constructor
            Symbolizer
                (
                ISymbolObserver& aObserver, //!< Observer to report findings to
                ISymbolBuffer& aSymbolBuffer, //!< Buffer which holds symbol data
                Unicode::IConverter& aByteConverter //!< Converter for bytes
                );

            //! Add Category
            //!
            //! Add a mapping of aSymbols into aCategory.
            //!
            //! @note The Symbolizer does not take ownership of the pointer; the intent of this method is to
            //!     provide pointers to stack-allocated objects.
            //! @note It is a good idea to add the most common entries first for performance reasons.
            //! @return true if the symbols were added successfully, false otherwise.
            bool AddCategory
                (
                ISymbolCategory const* const aCategory
                );

            //! End Parsing
            //!
            //! Notify the parser that no more Parse() calls are expected
            void EndParsing();

            //! Parse a chunk of data
            //!
            //! @return true if the buffer contains at least partially complete data, or false if the
            //!     Symbolizer cannot continue parsing the stream.
            bool Parse
                (
                Byte const* const aBytes, //!< [in] Pointer to bytes to read, must not be NULL
                size_t const aNumBytes, //!< [in] Number of bytes pointed to by aBytes, must be > 0
                size_t& aBytesRead //!< [out] Number of bytes read from aBytes
                );

            //! Can be used to reset the Symbolizer after a failed parsing operation
            void Reset();

            //! Change the byte converter to the given converter
            void SetByteConverter
                (
                Unicode::IConverter& aByteConverter
                );

        private:
            //! Lookup information about the category
            //!
            //! If a mapped category contains the symbol, write to output parameters:
            //! * aCategory - the value of ISymbolCategory::GetCategory()
            //! * aShouldConcatenate - the value of ISymbolCategory::ShouldConcatenate()
            //!
            //! If no category is found; the output parameters are not modified.
            void LookupCategory
                (
                Unicode::CodePoint const aSymbol, //!< [in] The symbol to lookup
                int& aCategory, //!< [out] The category which aSymbol belongs to
                bool& aShouldConcatenate //!< [out] The category's concatenation policy
                );

            //! Report that a symbol was found to the observer for the given category
            //!
            //! @return true if the parser should keep parsing.
            bool ReportSymbolFound
                (
                int const aCategory
                );

            //! Observer for symbols found by the Symbolizer
            ISymbolObserver& mObserver;

            //! Buffer used for symbol conversion
            ISymbolBuffer& mSymbolBuffer;

            //! Category associated with the symbol in mSymbolBuffer
            int mSavedCategory;

            //! Used to convert bytes to Unicode symbols
            Unicode::IConverter& mByteConverter;

            //! Map of symbols to categories
            SymbolCategoryMap mSymbolMap;

            //! Iterator pointing to last map entry used
            //!
            //! @note Must be declared after mSymbolMap due to initialization dependency
            SymbolCategoryMap::const_iterator mLastMapEntry;
        };

    } // namespace Lexer

} // namespace Bfdp

#endif // Bfdp_Lexer_Symbolizer

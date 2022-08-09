/**
    BFDP Lexer Symbolizer Definition

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

// Base types
#include "Bfdp/Lexer/Symbolizer.hpp"

// External includes
#include <algorithm>
#include <cstdlib>
#include <sstream>

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Lexer/CategoryBase.hpp"
#include "Bfdp/Unicode/Utf8Converter.hpp"

#define BFDP_MODULE "Lexer::Symbolizer"

namespace Bfdp
{

    namespace Lexer
    {

        Symbolizer::Symbolizer
            (
            ISymbolObserver& aObserver,
            ISymbolBuffer& aSymbolBuffer,
            Unicode::IConverterPtr const aByteConverter
            )
            : mObserver( aObserver )
            , mSymbolBuffer( aSymbolBuffer )
            , mSavedCategory( CategoryBase::NoCategory )
            , mByteConverter( aByteConverter )
            , mLastMapEntry( mSymbolMap.cend() )
        {
        }

        bool Symbolizer::AddCategory
            (
            ISymbolCategory const* const aCategory
            )
        {
            if( aCategory->GetCategory() < 0 )
            {
                BFDP_MISUSE_ERROR( "Bad category in AddCategory" );
                return false;
            }

            bool success = true;
            SymbolCategoryMap::const_iterator newEntry = mSymbolMap.cend();
            try
            {
                newEntry = mSymbolMap.insert( mSymbolMap.end(), aCategory );
            }
            catch( std::exception const& /* exception */ )
            {
                BFDP_RUNTIME_ERROR( "Failed to add symbol category" );
                success = false;
            }

            if( success && ( mLastMapEntry == mSymbolMap.cend() ) )
            {
                mLastMapEntry = newEntry;
            }

            return success;
        }

        void Symbolizer::EndParsing()
        {
            // If any data was left in the buffer, report it to mark it as complete.
            if( !mSymbolBuffer.IsEmpty() )
            {
                // Ignore the return value when reporting since this is the end
                ReportSymbolFound( mSavedCategory );
            }
            Reset();
        }

        bool Symbolizer::Parse
            (
            Byte const* const aBytes,
            size_t const aNumBytes,
            size_t& aBytesRead
            )
        {
            if( !mByteConverter )
            {
                return false;
            }

            aBytesRead = 0;

            if( ( aBytes == NULL ) ||
                ( aNumBytes == 0 ) )
            {
                BFDP_MISUSE_ERROR( "Bad input to Parse()" );
                return false;
            }

            size_t curPos = 0;
            while( curPos < aNumBytes )
            {
                /* Treat bytes as a multi-byte sequence and convert to a symbol */

                Unicode::CodePoint symbol;
                size_t const bytesToConvert = std::min< size_t >( ( aNumBytes - curPos ), mByteConverter->GetMaxBytes() );
                size_t bytesRead = mByteConverter->ConvertBytes( &aBytes[curPos], bytesToConvert, symbol );

                if( bytesRead == -2 ) // TODO: Magic number
                {
                    // Incomplete MB sequence
                    if( ( curPos + bytesToConvert ) >= aNumBytes )
                    {
                        // This is at the end of the the buffer; so allow parsing to continue
                        aBytesRead = aNumBytes;
                        return true;
                    }
                    else
                    {
                        // This is in the middle of the buffer
                        BFDP_RUNTIME_ERROR( "Incomplete multi-byte character sequence" );
                        aBytesRead = curPos;
                        return false;
                    }
                }
                else if( bytesRead == -1 ) // TODO: Magic number
                {
                    // Invalid MB sequence
                    BFDP_RUNTIME_ERROR( "Invalid multi-byte character sequence" );
                    aBytesRead = curPos;
                    return false;
                }
                else if( bytesRead == 0 )
                {
                    BFDP_RUNTIME_ERROR( "Invalid multi-byte character sequence" );
                    aBytesRead = curPos;
                    return false;
                }

                /* Categorize the symbol */

                int category = CategoryBase::Unknown;
                bool shouldConcatenate = true;
                LookupCategory( symbol, category, shouldConcatenate );

                /* Detect category boundary and send cached data */

                // See if the previous category is complete
                if( mSavedCategory != CategoryBase::NoCategory )
                {
                    // Report the symbol when changing categories
                    if( category != mSavedCategory )
                    {
                        if( ( !mSymbolBuffer.IsEmpty()             ) &&
                            ( !ReportSymbolFound( mSavedCategory ) ) )
                        {
                            aBytesRead = curPos;
                            return true;
                        }
                    }
                }

                /* Save/track categorized data */

                mSavedCategory = category;
                bool saved = mSymbolBuffer.Add( symbol );
                if( ( !saved                            ) &&
                    ( category == CategoryBase::Unknown ) )
                {
                    // For uncategorized data which does not fit in the buffer, send the result and
                    // add it again; this isn't an error since this data could be anything.
                    if( !ReportSymbolFound( category ) )
                    {
                        aBytesRead = curPos;
                        return true;
                    }
                    saved = mSymbolBuffer.Add( symbol );
                }
                if( !saved )
                {
                    BFDP_RUNTIME_ERROR( "Symbol too big" );
                    aBytesRead = curPos;
                    return false;
                }
                else if( !shouldConcatenate )
                {
                    // If concatenation should not occur, go ahead and report the symbol right now
                    if( !ReportSymbolFound( category ) )
                    {
                        aBytesRead = curPos;
                        return true;
                    }
                }

                curPos += bytesRead;
            } // end while

            // Save the final count of bytes parsed
            aBytesRead = curPos;

            // If the final section of data was uncategorized, treat it as complete and fire the
            // event.  Categorized data may span multiple Parse() calls.
            if( mSavedCategory == CategoryBase::Unknown )
            {
                // Report the data, but ignore the return value since Parse() is done anyway
                ReportSymbolFound( mSavedCategory );
            }

            return true;
        }

        void Symbolizer::Reset()
        {
            mSymbolBuffer.Clear();
            mSavedCategory = CategoryBase::NoCategory;
            // There is no need to reset the last map entry; it may or may not help the next parsing
            // operation.
        }

        void Symbolizer::LookupCategory
            (
            Unicode::CodePoint const aSymbol,
            int& aCategory,
            bool& aShouldConcatenate
            )
        {
            if( mSymbolMap.empty() )
            {
                return;
            }

            // Perform a lookup if one of the following is true:
            // * No previous category was found
            // * The last category does not contains the symbol (it is likely the next character
            //   will be in the same category as the last one)
            if( ( mLastMapEntry == mSymbolMap.cend()     ) ||
                ( !(*mLastMapEntry)->Contains( aSymbol ) ) )
            {
                // Look for a new category for mLastMapEntry
                mLastMapEntry = mSymbolMap.cend();
                for( SymbolCategoryMap::const_iterator iter = mSymbolMap.cbegin(); iter != mSymbolMap.cend(); ++iter )
                {
                    if( (*iter)->Contains( aSymbol ) )
                    {
                        mLastMapEntry = iter;
                        break;
                    }
                }
            }

            // If a category was found, update output parameters
            if( mLastMapEntry != mSymbolMap.cend() )
            {
                aCategory = (*mLastMapEntry)->GetCategory();
                aShouldConcatenate = (*mLastMapEntry)->ShouldConcatenate();
            }
        }

        bool Symbolizer::ReportSymbolFound
            (
            int const aCategory
            )
        {
            std::ostringstream utf8String;
            Unicode::Utf8Converter utf8Conv;
            size_t const numBytes = utf8Conv.GetMaxBytes();

            Data::ByteBuffer buf;
            if( !buf.Allocate( numBytes ) )
            {
                BFDP_RUNTIME_ERROR( "Out of memory while converting sequence to UTF-8" );
                return false;
            }

            for( size_t i = 0; i < mSymbolBuffer.GetSize(); ++i )
            {
                size_t bytesConverted = utf8Conv.ConvertSymbol
                    (
                    mSymbolBuffer.GetSymbolAt( i ),
                    buf.GetPtr(),
                    numBytes
                    );
                if( 0 == bytesConverted )
                {
                    BFDP_RUNTIME_ERROR( "Invalid symbol while converting to UTF-8" );
                    return false;
                }
                utf8String << buf.GetString( bytesConverted );
            }

            bool keepParsing = ( aCategory == CategoryBase::Unknown )
                ? mObserver.OnUnmappedSymbols( utf8String.str(), mSymbolBuffer.GetSize() )
                : mObserver.OnMappedSymbols( aCategory, utf8String.str(), mSymbolBuffer.GetSize() );

            mSymbolBuffer.Clear();

            return keepParsing;
        }

    } // namespace Lexer

} // namespace Bfdp

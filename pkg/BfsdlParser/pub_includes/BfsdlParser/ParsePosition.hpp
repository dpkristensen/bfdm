/**
    BFSDL Parse Position Declarations

    Copyright 2022, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef BfsdlParser_ParsePosition
#define BfsdlParser_ParsePosition

// External Includes
#include <string>

// Internal Includes
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/Data/ByteWindow.hpp"
#include "Bfdp/String.hpp"

namespace BfsdlParser
{

    //! Tracks positional data for keeping track of where parsing happened
    //!
    //! Although the stream parser doesn't care about newlines, humans tend to want information
    //! about where parsing stopped in a file to troubleshoot an error.  This class attempts to
    //! keep track of that for printing nicer error messages.
    class ParsePosition
    {
    public:
        ParsePosition
            (
            std::string const& aName, //!< [in] Name of the context (e.g., file name)
            size_t const aPretextLen, //!< [in] Maximum history of bytes to store
            size_t const aPosttextLen //!< [in] Maximum number of extra bytes to store
            );

        //! @return The column at which the current context data begins
        size_t GetContextBeginColumn() const;

        //! @return The offset within the context data of the current position
        size_t GetContextPositionOffset() const;

        //! @return The current column number within the current line
        size_t GetCurColNumber() const;

        //! @return The current line number
        size_t GetCurLineNumber() const;

        //! @return The name of the position context
        std::string const& GetName() const;

        //! @return A printable representation of the context.
        std::string GetPrintableContext() const;

        //! Process incoming data which has been read successfully
        //!
        //! This data is added to the context and counted towards the current position.
        void ProcessNewData
            (
            Bfdp::Byte const* const aInData,
            size_t const aInSize
            );

        //! Process incoming data which has not been read successfully
        //!
        //! This data is added to the context and counted towards the current position.
        void ProcessRemainderData
            (
            Bfdp::Byte const* const aInData,
            size_t const aInSize
            );

    private:
        Bfdp::Data::ByteWindow mContextWindow;
        Bfdp::Data::ByteBuffer mRemainderBuf;

        size_t mCurLineNumber;
        std::string mName;
        Bfdp::Byte mNewlineChar;
        size_t mRemainderSize;
    };

} // namespace BfsdlParser

#endif // BfsdlParser_ParsePosition

/**
    BFSDL Stream Parser Definitions

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

#define BFDP_MODULE "BfsdlParser::StreamParser"

// Base includes
#include "BfsdlParser/StreamParser.hpp"

// External Includes
#include <cstring>
#include <sstream>

// Internal Includes
#include "Bfdp/Data/ByteBuffer.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Lexer/Symbolizer.hpp"
#include "BfsdlParser/ParsePosition.hpp"
#include "BfsdlParser/Objects/Property.hpp"
#include "BfsdlParser/Token/Interpreter.hpp"
#include "BfsdlParser/Token/Tokenizer.hpp"

#define LOCAL_DEBUG 0

#if( LOCAL_DEBUG )
    #include <iostream>
    #define DEBUG_TRACE( _out ) \
        std::cout << _out  << std::endl
#else
    #define DEBUG_TRACE( _out )
#endif

namespace BfsdlParser
{

    //! Build a parser stack and feed data from the stream into it.
    //!
    //! @return 0 on success, 1 otherwise.
    int ParseStream
        (
        Objects::TreePtr const aDbContext,
        std::istream& aIn,
        size_t const aChunkSize
        )
    {
        Token::Interpreter interpreter( aDbContext );
        BFDP_RETURNIF_VE( !interpreter.IsInitOk(), 1, "Failed to init Interpreter" );
        Token::Tokenizer tokenizer( interpreter );
        BFDP_RETURNIF_VE( !tokenizer.IsInitOk(), 1, "Failed to init Tokenizer" );

        Bfdp::Data::ByteBuffer buf;
        BFDP_RETURNIF_VE( !buf.Allocate( aChunkSize ), 1, "Failed to allocate read buffer" );
        buf.Clear();

        // Get the filename associated with this stream
        std::string fileName;
        auto fileNameProp = aDbContext->FindPropertyT< Objects::Property >( "Filename" );
        if( fileNameProp )
        {
            fileName = fileNameProp->GetString();
        }
        ParsePosition parsePos( fileName, 10, 6 );

        bool ok = true;
        size_t dataStart = 0;
        while( ok )
        {
            std::streamsize bytesToRead = static_cast< std::streamsize >( buf.GetSize() - dataStart );
            DEBUG_TRACE( "read " << dataStart << "..+" << bytesToRead );
            aIn.read( buf.GetPtrT< char >() + dataStart, bytesToRead );
            size_t bytesRead = static_cast< size_t >( aIn.gcount() );
            DEBUG_TRACE( "read: " << bytesRead << " / " << bytesToRead );
            if( bytesRead == 0 )
            {
                break;
            }

            size_t bytesParsed = 0;
            size_t i = 0;
            while( ok && ( i < bytesRead ) )
            {
                size_t bytesLeft = bytesRead - i;
                DEBUG_TRACE( "parse: " << i << "..+" << bytesLeft );
                ok = tokenizer.Parse( &buf[i], bytesLeft, bytesParsed );
                parsePos.ProcessNewData( &buf[i], bytesParsed );
                if( !ok )
                {
                    parsePos.ProcessRemainderData( &buf[i + bytesParsed], bytesLeft - bytesParsed );
                    std::stringstream ss;
                    ss << "Parse Error: " << parsePos.GetName() << "@"
                        << parsePos.GetCurLineNumber() << ":" << parsePos.GetCurColNumber()
                        << std::endl;

                    if( parsePos.GetContextBeginColumn() != 0 )
                    {
                        ss << "...";
                    }
                    ss << parsePos.GetPrintableContext() << std::endl;
                    if( parsePos.GetContextBeginColumn() != 0 )
                    {
                        ss << "   ";
                    }
                    if( parsePos.GetContextPositionOffset() > 0 )
                    {
                        ss << std::string( parsePos.GetContextPositionOffset() - 1, ' ' ) << "^";
                    }
                    std::string msg = ss.str();
                    BFDP_RUNTIME_ERROR( msg.c_str() );
                }
                else if( bytesParsed == 0 )
                {
                    DEBUG_TRACE( "  remainder:" << bytesLeft );
                    // Needs more data, likely; move it to the front of the buffer
                    // and continue.
                    std::memmove( &buf[0], &buf[i], bytesLeft );
                    dataStart = bytesLeft;
                    break;
                }
                else
                {
                    DEBUG_TRACE( "  parsed " << bytesParsed );
                    i += bytesParsed;
                }
            }
        }

        if( ok && !aIn.eof() && aIn.fail() )
        {
            BFDP_RUNTIME_ERROR( "Stream read error" );
            ok = false;
        }

        return ok ? 0 : 1;
    }

} // namespace BfsdlParser

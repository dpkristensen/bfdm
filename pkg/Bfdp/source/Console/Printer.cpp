/**
    BFDP Printer Class

    Copyright 2020, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

// Base Includes
#include "Bfdp/Console/Printer.hpp"

// Internal Includes
#include "Bfdp/Macros.hpp"

namespace Bfdp
{

    namespace Console
    {

        Printer::Printer
            (
            std::ostream& aOut,
            std::string const& aIndentStr,
            size_t const aMaxIndent
            )
            : mIndentLevel( 0 )
            , mIndentString( aIndentStr )
            , mIsNewLine( true )
            , mMaxIndent( aMaxIndent )
            , mOut( aOut )
        {
        }

        void Printer::Indent()
        {
            if( mIndentLevel < mMaxIndent )
            {
                ++mIndentLevel;
            }
        }

        void Printer::Print
            (
            std::string const& aMsg
            )
        {
            Put( aMsg );
            mOut << std::endl;
            mIsNewLine = true;
        }

        void Printer::PrintIndent()
        {
            for( size_t i = 0; i < mIndentLevel; ++i )
            {
                mOut << mIndentString;
            }
        }

        void Printer::Put
            (
            std::string const& aMsg
            )
        {
            BFDP_RETURNIF( aMsg.empty() );

            if( mIsNewLine )
            {
                PrintIndent();
                mIsNewLine = false;
            }
            mOut << aMsg;
        }

        void Printer::Unindent()
        {
            if( mIndentLevel > 0 )
            {
                --mIndentLevel;
            }
        }

        Printer& Printer::operator <<
            (
            std::string const& aMsg
            )
        {
            Print( aMsg );
            return *this;
        }

        Indent::Indent
            (
            Printer& aPrinter,
            Msg const& aHeader,
            Msg const& aFooter
            )
            : mFooter( aFooter )
            , mPrinter( aPrinter )
        {
            if( !aHeader.Empty() )
            {
                mPrinter.Print( aHeader );
            }
            mPrinter.Indent();
        }

        Indent::~Indent()
        {
            mPrinter.Unindent();
            if( !mFooter.Empty() )
            {
                mPrinter.Print( mFooter );
            }
        }

    } // namespace Console

} // namespace Bfdp

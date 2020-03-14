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

#ifndef Bfdp_Console_Printer
#define Bfdp_Console_Printer

// External Includes
#include <iostream>
#include <string>

// Internal Includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"
#include "Bfdp/Console/Msg.hpp"

namespace Bfdp
{

    namespace Console
    {

        //! Encapsulates methods to print formatted output
        class Printer
        {
        public:
            Printer
                (
                std::ostream& aOut = std::cout,
                std::string const& aIndentStr = std::string( "    " ),
                size_t const aMaxIndent = 10
                );

            //! Increase indent level
            void Indent();

            //! Write a line of text
            void Print
                (
                std::string const& aMsg
                );

            //! Write the indentation spaces
            void PrintIndent();

            //! Write text with no newline
            void Put
                (
                std::string const& aMsg
                );

            //! Decrease indent level
            void Unindent();

            //! Write a line of text using the << operator
            Printer& operator <<
                (
                std::string const& aMsg
                );

        private:
            size_t mIndentLevel;
            std::string mIndentString;
            bool mIsNewLine;
            size_t mMaxIndent;
            std::ostream& mOut;
       };

        //! Allows scope-level indentation
        class Indent
            : Bfdp::NonAssignable
            , Bfdp::NonCopyable
        {
        public:
            Indent
                (
                Printer& aPrinter,
                Msg const& aHeader = Msg(),
                Msg const& aFooter = Msg()
                );

            ~Indent();

        private:
            Msg mFooter;
            Printer& mPrinter;
       };

    } // namespace Console

} // namespace Bfdp

#endif // Bfdp_Console_Printer

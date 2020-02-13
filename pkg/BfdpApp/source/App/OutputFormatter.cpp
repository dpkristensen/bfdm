/**
    BFDP Console Application Output Formatter

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
#include "App/OutputFormatter.hpp"

namespace App
{

    static size_t const sMaxIndent = 10;
    static char const* const sIndentStr = "    ";

    OutputFormatter::OutputFormatter()
        : mFile( stdout )
        , mIndent( 0 )
    {
    }

    OutputFormatter::~OutputFormatter()
    {
    }

    void OutputFormatter::Print
        (
        std::string const& aString
        )
    {
        Write( "", aString.c_str(), aString.size() );
    }

    void OutputFormatter::PrintH
        (
        char const* const aHeader,
        std::string const& aString
        )
    {
        Write( aHeader, aString.c_str(), aString.size() );
    }

    void OutputFormatter::PrintArg
        (
        std::string const& aArg,
        std::string const& aDescription
        )
    {
        Print( aArg );
        ++mIndent;
        Print( aDescription );
        --mIndent;
    }

    void OutputFormatter::PrintArgD
        (
        std::string const& aArg,
        std::string const& aDescription,
        std::string const& aDefault
        )
    {
        Print( aArg );
        ++mIndent;
        Print( aDescription );
        PrintH( "Default: ", aDefault );
        --mIndent;
    }

    OutputFormatter& OutputFormatter::operator <<
        (
        std::string const& aIn
        )
    {
        Print( aIn );
        return *this;
    }

    OutputFormatter& OutputFormatter::operator ++ ()
    {
        if( mIndent < sMaxIndent )
        {
            ++mIndent;
        }

        return *this;
    }

    OutputFormatter& OutputFormatter::operator -- ()
    {
        if( mIndent > 0U )
        {
            --mIndent;
        }

        return *this;
    }

    void OutputFormatter::Write
        (
        char const* const aHeader,
        void const* const aData,
        size_t const aSize
        )
    {
        for( unsigned int i = 0; i < mIndent; ++i )
        {
            std::fputs( sIndentStr, mFile );
        }
        std::fputs( aHeader, mFile );
        std::fwrite( aData, 1, aSize, mFile );
        std::fputc( '\n', mFile );
    }

    IndentedSection::IndentedSection
        (
        OutputFormatter& aOut
        )
        : mOut( aOut )
    {
        ++mOut;
    }

    IndentedSection::~IndentedSection()
    {
        --mOut;
        mOut.Print( "" );
    }

} // namespace App

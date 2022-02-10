/**
    BFDP Argument Parser Class

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

#ifndef Bfdp_Console_ArgParser
#define Bfdp_Console_ArgParser

// External Includes
#include <cstdio>
#include <list>
#include <string>

// Internal Includes
#include "Bfdp/Console/Param.hpp"
#include "Bfdp/Console/Printer.hpp"

namespace Bfdp
{

    namespace Console
    {

        //! Parser for command line arguments
        //!
        //! The general order of elements that in the command-line sequence are:
        //! * 0 or more switched (i.e., not positional) parameters
        //! * 0 or more mandatory positional parameters with cardinality=1
        //! * 0 or 1 of the following:
        //!   * Mandatory positional parameter with any cardinality
        //!   * Optional positional parameter
        //!   * Sub-command
        class ArgParser
        {
        public:
            //! Construct a parser with default settings:
            //!
            //! * argv[0] Is the treated name of the executable that launched the program
            //! * argc must be at least 1
            //! * All descriptive strings are empty
            ArgParser
                (
                Param::ArgumentCallback const aDefaultCallback = nullptr,
                uintptr_t const aDefaultUserdata = 0U
                );

            //! Add a parameter to the parser
            //!
            //! @return Reference to parser for call chaining
            ArgParser& Add
                (
                Param const& aParam
                );

            //! Add standard help parameters to the parser
            //!
            //! @post Help arguments accepted are "/h", "-h", "--help"
            //! @return Reference to parser for call chaining
            ArgParser& AddHelp();

            //! Get the active command name
            //!
            //! This will be one of:
            //! * The name passed to SetName()
            //! * If parse() is being executed, the name discovered from the command line
            //! * Empty if parse() is not being executed
            //!
            //! @return The active command name
            std::string GetName() const;

            //! Get Parse Index
            //!
            //! Before Parse() is called, returns 0
            //! During Parse(), this is the index of the parsed argument
            //! After Parse(), this is the end of the list or a terminator argument
            //!
            //! @return The current index of the argument list
            int GetParseIndex() const;

            //! Parse C stdlib-style argument list
            //!
            //! @pre Neither aArgV or any element of aArgV may be NULL
            //! @pre All elements of aArgV must be null-terminated
            //! @post If aCallback is not null, aUserdata is passed to aCallback
            //! @post If aCallback is null, default callback and userdata are used
            //! @post If neither callback is specified, only parameter validation is performed
            //! @return 0 on success, or error code otherwise.
            int Parse
                (
                char const* const* const aArgV,
                int const aArgC,
                Param::ArgumentCallback const aCallback = nullptr,
                uintptr_t const aUserdata = 0U
                );

            void PrintHelp
                (
                std::FILE* const aOut
                ) const;

            //! Set the name of this command, for use in help text
            //!
            //! Setting this will override automatic import from Parse().
            void SetName
                (
                std::string const& aName
                );

        private:
            typedef std::list< Param > ParamList;
            typedef std::list< Param const* > ParamPtrList;

            void Dispatch
                (
                Param const& aParam,
                char const* const* const aArgV,
                int const aArgC
                );

            Param const* GetNextMatchingArg
                (
                std::string const& aText,
                bool const aLongForm,
                bool const aShortForm
                );

            Param const* GetNextPositional();

            void ProcArgs
                (
                std::string const& aText,
                char const* const* const aArgV,
                int const aArgC
                );

            Param::ArgumentCallback mCurCallback;
            ParamPtrList mCurParams;
            uintptr_t mCurUserdata;
            Param::ArgumentCallback mDefaultCallback;
            uintptr_t mDefaultUserdata;
            bool mKeepParsing;
            std::string mName;
            ParamList mParams;
            int mParseIndex;
            bool mSkipNextArg;
            int mResult;
        };

    } // namespace Console

} // namespace Bfdp

#endif // Bfdp_Console_ArgParser

/**
    BFDP Msg Class

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

#ifndef Bfdp_Console_Msg
#define Bfdp_Console_Msg

// External Includes
#include <sstream>
#include <string>

namespace Bfdp
{

    namespace Console
    {

        //! Represents a printable message
        //!
        //! Encapsulates a string, along with logic to differentiate an empty message (i.e., does
        //! not exist) from a zero-length string.
        class Msg
        {
        public:
            Msg();

            Msg
                (
                char const* const aText
                );

            Msg
                (
                std::string const& aText
                );

            Msg
                (
                Msg const& aOther
                );

            //! @return Whether the message has been set (can still be zero length!).
            bool Empty() const;

            //! @return A copy of the message
            std::string Get() const;

            Msg& operator =
                (
                Msg const& aOther
                );

            Msg& operator <<
                (
                std::string const& aText
                );

            operator std::string () const;

        private:
            bool mEmpty;
            std::stringstream mStream;
       };

    } // namespace Console

} // namespace Bfdp

#endif // Bfdp_Console_Printer

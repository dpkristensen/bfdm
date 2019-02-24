/**
    BFDP Data StringMachine Definitions

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

// Base Includes
#include "Bfdp/Data/StringMachine.hpp"

// External Includes
#include <ios>

// Internal Includes
#include "Bfdp/Common.hpp"

namespace Bfdp
{

    namespace Data
    {

        namespace StringMachineInternal
        {
            static std::streampos const POS_BEGIN = std::streampos( 0 );
        }

        using namespace StringMachineInternal;

        StringMachine::StringMachine()
            : mDefined( false )
        {
        }

        void StringMachine::AppendUtf8
            (
            std::string const& aValue
            )
        {
            mBuffer << aValue;
            SetDefined();
        }

        std::string StringMachine::GetUtf8HexString
            (
            std::string const& aSeparator,
            std::string const& aPrefix
            ) const
        {
            std::ostringstream oss;
            char digit[3];
            std::string sep;

            Pack();
            digit[2] = '\0';
            for( std::string::const_iterator iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                char c = *iter;
                digit[0] = GetHex( ( c >> 4 ) & 0xF );
                digit[1] = GetHex( c & 0xF );
                oss << sep << aPrefix << digit;
                sep = aSeparator;
            }

            return oss.str();
        }

        std::string const& StringMachine::GetUtf8String() const
        {
            Pack();
            return mCache;
        }

        bool StringMachine::IsDefined() const
        {
            return mDefined;
        }

        bool StringMachine::IsEmpty() const
        {
            return mCache.empty() && ( mBuffer.tellp() == POS_BEGIN );
        }

        void StringMachine::Reset()
        {
            mDefined = false;
            mCache.clear();
            mBuffer.str() = std::string();
        }

        void StringMachine::SetDefined()
        {
            mDefined = true;
        }

        char StringMachine::GetHex
            (
            char const aNibble
            ) const
        {
            if( IsWithinRange< char >( 0, aNibble, 9 ) )
            {
                return aNibble + '0';
            }
            else if( IsWithinRange< char >( 10, aNibble, 16 ) )
            {
                return aNibble - 10 + 'a';
            }
            return '?';
        }

        void StringMachine::Pack() const
        {
            if( mBuffer.tellp() != POS_BEGIN )
            {
                mCache = mCache + mBuffer.str();
                mBuffer.clear();
                mBuffer.str( "" );
            }
        }

    } // namespace Data

} // namespace Bfdp

/**
    BFDP Command-line Parameter Class

    Copyright 2021, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/Console/Param.hpp"

// External Includes
#include <sstream>

// Internal Includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Macros.hpp"

#define BFDP_MODULE "Console::Param"

namespace Bfdp
{

    namespace Console
    {

        /* static */ Param Param::CreateCommand
            (
            std::string const& aName
            )
        {
            return Param( aName, '\0', true ).SetTerminator();
        }

        /* static */ Param Param::CreateLong
            (
            std::string const& aLongName,
            char const aShortName
            )
        {
            return Param( aLongName, aShortName, false );
        }

        /* static */ Param Param::CreateShort
            (
            char const aShortName
            )
        {
            return Param( std::string(), aShortName, false );
        }

        /* static */ Param Param::CreatePositional
            (
            std::string const& aName
            )
        {
            return Param( aName, '\0', true );
        }

        Param::ArgumentCallback Param::GetCallback() const
        {
            return mCallback;
        }

        std::string Param::GetDefaultValue() const
        {
            return mDefaultValue;
        }

        std::string Param::GetDescription() const
        {
            std::stringstream ss;

            if( !mDescription.empty() )
            {
                ss << mDescription;
            }

            if( mIsOptional && !mDefaultValue.empty())
            {
                if( !mDescription.empty() )
                {
                    ss << " (";
                }
                ss << "default: " << mDefaultValue;
                if( !mDescription.empty() )
                {
                    ss << ")";
                }
            }
            return ss.str();
        }

        std::string Param::GetName() const
        {
            if( !mLongName.empty() )
            {
                return mLongName;
            }
            else if( mShortName != '\0' )
            {
                return std::string( 1, mShortName );
            }

            return "<unkP>";
        }

        //! @return A string suitable for showing in a list
        std::string Param::GetShortDescription() const
        {
            return GetNameValueDescriptor();
        }


        std::string Param::GetUsage() const
        {
            std::stringstream ss;

            if( mIsOptional )
            {
                ss << "[";
            };

            std::string nvd = GetNameValueDescriptor();

            BFDP_RETURNIF_V( nvd.empty(), std::string() );

            if( !mLongName.empty() )
            {
                ss << "--" << nvd;
            }
            else if( mShortName != '\0' )
            {
                ss << "-" << nvd;
            }

            if( mIsOptional )
            {
                ss << "]";
            };

            return ss.str();
        }

        uintptr_t Param::GetUserdata() const
        {
            return mUserdata;
        }

        std::string Param::GetValueName() const
        {
            return mValueName;
        }

        bool Param::HasValue() const
        {
            return mHasValue;
        }

        bool Param::IsOptional() const
        {
            return mIsOptional;
        }

        bool Param::IsPositional() const
        {
            return mIsPositional;
        }

        bool Param::IsTerminator() const
        {
            return mIsTerminator;
        }

        bool Param::MatchLongName
            (
            std::string const& aText
            ) const
        {
            return !mIsPositional &&
                !mLongName.empty() &&
                ( mLongName == aText );
        }

        bool Param::MatchShortName
            (
            std::string const& aText
            ) const
        {
            return !mIsPositional &&
                ( mShortName != '\0') &&
                ( aText.size() == 1 ) &&
                ( aText[0] == mShortName );
        }

        Param& Param::SetCallback
            (
            ArgumentCallback const aCallback
            )
        {
            mCallback = aCallback;
            return *this;
        }

        Param& Param::SetDefault
            (
            std::string const& aValue,
            std::string const& aValueName
            )
        {
            if( mIsOptional )
            {
                BFDP_MISUSE_ERROR( "Argument already optional" );
            }
            else if( mIsPositional )
            {
                BFDP_MISUSE_ERROR( "Positional arguments can't have default values" );
            }
            else
            {
                if( !aValueName.empty() )
                {
                    mValueName = aValueName;
                }
                mDefaultValue = aValue;
                mIsOptional = true;
                mHasValue = true;
            }
            return *this;
        }

        Param& Param::SetDescription
            (
            std::string const& aDescription
            )
        {
            mDescription = aDescription;
            return *this;
        }

        Param& Param::SetOptional()
        {
            if( mHasValue && mDefaultValue.empty() )
            {
                BFDP_INTERNAL_ERROR( "Argument requires a default value" );
            }
            else
            {
                mIsOptional = true;
            }
            return *this;
        }

        Param& Param::SetTerminator()
        {
            mIsTerminator = true;
            return *this;
        }

        Param& Param::SetUserdata
            (
            uintptr_t const aValue
            )
        {
            mUserdata = aValue;
            return *this;
        }

        Param& Param::SetValueName
            (
            std::string const& aValueName
            )
        {
            mValueName = aValueName;
            mHasValue = true;
            return *this;
        }

        Param::Param
            (
            std::string const& aLongName,
            char const aShortName,
            bool const aIsPositional
            )
            : mCallback( nullptr )
            , mHasValue( false )
            , mIsOptional( false )
            , mIsPositional( aIsPositional )
            , mIsTerminator( false )
            , mLongName( aLongName )
            , mShortName( aShortName )
            , mUserdata( 0U )
            , mValueName( "value" )
        {
        }


        //! @return A string naming the argument and the value (if used)
        std::string Param::GetNameValueDescriptor() const
        {
            std::stringstream ss;

            if( !mLongName.empty() )
            {
                ss << mLongName;
            }
            else if( mShortName != '\0' )
            {
                ss << mShortName;
            }
            else
            {
                // Has no name, ignore.
                return std::string();
            }

            if( mHasValue )
            {
                ss << " <" << mValueName << ">";
            }

            return ss.str();
        }

    } // namespace Console

} // namespace Bfdp

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

#ifndef Bfdp_Console_Param
#define Bfdp_Console_Param

// External Includes
#include <string>

// Internal Includes

namespace Bfdp
{

    namespace Console
    {

        // Forward declaration
        class ArgParser;

        //! Encapsulates a Command Line Parameter
        class Param
        {
        public:
            typedef int (*ArgumentCallback)
                (
                ArgParser const& aParser,
                Param const& aParam,
                std::string const& aValue,
                uintptr_t const aUserdata
                );

            //! @note This stops the parser, allowing the rest of the arguments to be passed to a
            //!     sub-parser, if desired.
            //! @return A positional terminator parameter
            static Param CreateCommand
                (
                std::string const& aCommandName = "command"
                );

            //! @return A switched parameter with a long-form name, and optionally a short name as well
            static Param CreateLong
                (
                std::string const& aLongName,
                char const aShortName = '\0'
                );

            //! @return A switched parameter identified only by the short name
            static Param CreateShort
                (
                char const aShortName
                );

            //! @return A positional parameter
            static Param CreatePositional
                (
                std::string const& aName
                );

            //! @return The callback function, or nullptr if empty.
            ArgumentCallback GetCallback() const;

            //! @pre SetDefault() was called
            //! @return The default value
            std::string GetDefaultValue() const;

            //! @return A description of the field (default: empty)
            std::string GetDescription() const;

            //! @return The effective name of the field
            std::string GetName() const;

            //! @return A string suitable for showing in a list
            std::string GetShortDescription() const;

            //! @return A string suitable for inclusion in usage syntax
            std::string GetUsage() const;

            //! @return The value passed to SetUserdata()
            uintptr_t GetUserdata() const;

            //! @return The value passed to SetUserdata() and friends as a pointer
            template< class T >
            T* GetUserdataPtr() const
            {
                return reinterpret_cast< T* >( GetUserdata() );
            }

            //! @return The value passed to SetUserdata() and friends as a const pointer
            template< class T >
            T const* GetUserdataConstPtr() const
            {
                return reinterpret_cast< T const* >( GetUserdata() );
            }

            //! @pre HasValue() returns true
            //! @return The name of the value expected for the argument (default: value)
            std::string GetValueName() const;

            //! @return True if the parameter expects a value, False otherwise.
            bool HasValue() const;

            //! @return True if multiple instances of the parameter may be seen.
            bool IsCounter() const;

            //! @return True if the parameter is optional, False otherwise.
            bool IsOptional() const;

            //! @return True if the parameter is a positional argument, False otherwise.
            bool IsPositional() const;

            //! @return True if parsing should stop on this argument.
            bool IsTerminator() const;

            //! @return true if the text matches the long name, false otherwise
            bool MatchLongName
                (
                std::string const& aText
                ) const;

            //! @return true if the text matches the short name, false otherwise
            bool MatchShortName
                (
                std::string const& aText
                ) const;

            //! Set a custom callback when handling this parameter
            //!
            //! The associated userdata from SetUserdata() will be given in the callback.
            //!
            //! @return Reference for call chaining
            Param& SetCallback
                (
                ArgumentCallback const aCallback
                );

            //! Set a default value
            //!
            //! @pre Cannot use with SetOptional()
            //! @pre Argument cannot be positional
            //! @post IsOptional() returns true
            //! @post HasValue() returns true
            //! @post If aValueName is not empty, it will be returned in GetValueName()
            //! @return Reference for call chaining
            Param& SetDefault
                (
                std::string const& aValue,
                std::string const& aValueName = std::string()
                );

            //! Set a description
            //!
            //! @return Reference for call chaining
            Param& SetDescription
                (
                std::string const& aDescription
                );

            //! Set the value as optional
            //!
            //! @pre Cannot use with SetDefault()
            //! @post IsOptional() returns true
            //! @return Reference for call chaining
            Param& SetOptional();

            //! Allow multiple instances of the parameter
            //!
            //! @pre Cannot use with associated values
            //! @post IsCounter() returns true
            //! @post IsOptional() returns true
            //! @return Reference for call chaining
            Param& SetCounter();

            //! Set this parameter as a terminator
            //! @return Reference for call chaining
            Param& SetTerminator();

            //! Set a descriptive name associated with the value
            //!
            //! @post HasValue() returns true
            //! @return Reference for call chaining
            Param& SetValueName
                (
                std::string const& aValueName
                );

            //! Set a value to be associated with the parameter
            //!
            //! @post GetUserdata() will return this value
            //! @post If SetCallback() was used, this will be the userdata in the callback.
            //! @return Reference for call chaining
            Param& SetUserdata
                (
                uintptr_t const aValue
                );

            //! Like SetUserdata(), but as a pointer
            //!
            //! @return Reference for call chaining
            template< class T >
            Param& SetUserdataPtr
                (
                T* const aPtr
                )
            {
                return SetUserdata( reinterpret_cast< uintptr_t >( aPtr ) );
            }

            //! Like SetUserdata(), but as a const pointer
            //!
            //! @return Reference for call chaining
            template< class T >
            Param& SetUserdataPtr
                (
                T const* const aPtr
                )
            {
                return SetUserdata( reinterpret_cast< uintptr_t >( aPtr ) );
            }

        private:
            Param
                (
                std::string const& aLongName,
                char const aShortName,
                bool const aIsPositional
                );

            //! @return A string naming the argument and the value (if used)
            std::string GetNameValueDescriptor() const;

            ArgumentCallback mCallback;
            std::string mDefaultValue;
            std::string mDescription;
            bool mHasValue;
            bool mIsCounter;
            bool mIsOptional;
            bool mIsPositional;
            bool mIsTerminator;
            std::string mLongName;
            char mShortName;
            uintptr_t mUserdata;
            std::string mValueName;
        };

    } // namespace Console

} // namespace Bfdp

#endif // Bfdp_Console_Param

/**
    BFSDL Parser Interpreter Declarations

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

#ifndef BfsdlParser_Token_Interpreter
#define BfsdlParser_Token_Interpreter

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"
#include "BfsdlParser/Token/ITokenObserver.hpp"

// Internal Includes
#include "Bfdp/StateMachine/Engine.hpp"
#include "BfsdlParser/Objects/Tree.hpp"
#include "BfsdlParser/Token/Tokenizer.hpp"

namespace BfsdlParser
{

    namespace Token
    {

        //! Interpreter
        //!
        //! This class assembles tokens into statements using the grammar rules of BFSDL and emits language objects.
        class Interpreter
            : public ITokenObserver
            , private Bfdp::NonAssignable
            , private Bfdp::NonCopyable
        {
        public:
            Interpreter
                (
                Objects::TreePtr const aDbContext
                );

            //! @return whether the Interpreter initialized successfully.
            bool IsInitOk() const;

        private:
            struct Header
            {
                enum StreamProgressType
                {
                    StreamBegin,
                    StreamInProgress,
                    StreamDone
                };
            };

            struct In
            {
                enum Type
                {
                    Control,
                    NumericLiteral,
                    StringLiteral,
                    Word,

                    Count,
                    Invalid = Count
                };
            };

            struct InputRef
            {
                In::Type type;
                union
                {
                    std::string const* ctrl;
                    Objects::NumericLiteral const* num;
                    Bfdp::Data::StringMachine const* str;
                    std::string const* word;
                } d;
            };

            void LogError
                (
                std::string const& aMessage
                );

            void LogError();

            BFDP_OVERRIDE( bool OnControlCharacter
                (
                std::string const& aControlCharacter
                ) );

            BFDP_OVERRIDE( bool OnNumericLiteral
                (
                Objects::NumericLiteral const& aValue
                ) );

            BFDP_OVERRIDE( bool OnStringLiteral
                (
                Bfdp::Data::StringMachine const& aValue
                ) );

            BFDP_OVERRIDE( bool OnWord
                (
                std::string const& aValue
                ) );


            template< typename T >
            void SetNumericPropertyDefault
                (
                std::string const& aName,
                typename T const aValue
                )
            {
                if( mDb->FindProperty( aName ) == NULL )
                {
                    if( !SetNumericProperty( mDb, aName, aValue ) )
                    {
                        LogError( Bfdp::Console::Msg( "Failed to set default for " ) << aName );
                    }
                }
            }

            void SetStringPropertyDefault
                (
                std::string const& aName,
                std::string const& aValue
                );

            // States
            void StateHeaderBeginEntry();
            void StateHeaderBeginEvaluate();
            void StateHeaderIdentifierEvaluate();
            void StateHeaderIdentifierExit();
            void StateHeaderEqualsEvaluate();
            void StateHeaderParameterEvaluate();
            void StateStatementBeginEvaluate();

            Objects::TreePtr mDb;

            // Header tracking variables
            Header::StreamProgressType mHeaderStreamProgress;

            // An identifier (relevancy depends on the context of the current state)
            std::string mIdentifier;

            //! Whether the interpreter has been initialized
            bool mInitOk;

            //! Provides access to the input data
            InputRef mInput;

            //! Whether a parsing error occurred
            bool mParseError;

            //! State machine engine to use for algorithm control
            Bfdp::StateMachine::Engine mStateMachine;
        };

    } // namespace Token

} // namespace BfsdlParser

#endif // BfsdlParser_Token_Interpreter

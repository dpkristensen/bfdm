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

// Base Includes
#include "BfsdlParser/Token/Interpreter.hpp"

// External Includes
#include <sstream>

// Internal Includes
#include "Bfdp/BitManip/Conversion.hpp"
#include "Bfdp/Console/Msg.hpp"
#include "Bfdp/StateMachine/Actions.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/CodingMap.hpp"
#include "Bfdp/Unicode/Common.hpp"
#include "Bfdp/Unicode/Functions.hpp"
#include "BfsdlParser/Objects/Property.hpp"

#define BFDP_MODULE "Token::Interpreter"

namespace BfsdlParser
{

    namespace Token
    {

        using namespace Bfdp;

        using BfsdlParser::Objects::BitBase;
        using BfsdlParser::Objects::Endianness;
        using BfsdlParser::Objects::Property;
        using BfsdlParser::Objects::PropertyPtr;
        using BfsdlParser::Objects::TreePtr;

        namespace InternalInterpreter
        {

            struct ParseState
            {
                enum Type
                {
                    // Header Definition states (BFSDL section 4)
                    HeaderBegin,
                    HeaderIdentifier,
                    HeaderEquals,
                    HeaderParameter,

                    // Stream Data Definition states
                    StatementBegin,

                    Count
                };
            };

            template< typename T >
            static bool SetNumericProperty
                (
                TreePtr& aTree,
                std::string const& aName,
                typename T const aValue
                )
            {
                PropertyPtr pp = std::make_shared< Property >( aName );
                BFDP_RETURNIF_V( !pp, false );

                BFDP_RETURNIF_V( !pp->SetNumericValue( aValue ), false );
                BFDP_RETURNIF_V( !aTree->Add( pp ), false );

                return true;
            }

            static bool SetStringProperty
                (
                TreePtr& aTree,
                std::string const& aName,
                std::string const& aValue
                )
            {
                PropertyPtr pp = std::make_shared< Property >( aName );
                BFDP_RETURNIF_V( !pp, false );

                BFDP_RETURNIF_V( !pp->SetString( aValue ), false );
                BFDP_RETURNIF_V( !aTree->Add( pp ), false );

                return true;
            }

        };
        using namespace InternalInterpreter;

        Interpreter::Interpreter
            (
            Objects::TreePtr const aDbContext
            )
            : mDb( aDbContext )
            , mHeaderStreamProgress( Header::StreamBegin )
            , mInitOk( false )
            , mParseError( false )
        {
            std::memset( &mInput, 0, sizeof( mInput ) );
            mInput.type = In::Invalid;

            bool ok = true;
            typedef StateMachine::CallMethod< Interpreter > CallMethod;

            BFDP_STATE_MAP_BEGIN( ok, mStateMachine, ParseState::Count );

            BFDP_STATE_ACTION( ParseState::HeaderBegin, Entry, CallMethod( *this, &Interpreter::StateHeaderBeginEntry ) );
            BFDP_STATE_ACTION( ParseState::HeaderBegin, Evaluate, CallMethod( *this, &Interpreter::StateHeaderBeginEvaluate ) );
            BFDP_STATE_ACTION( ParseState::HeaderIdentifier, Evaluate, CallMethod( *this, &Interpreter::StateHeaderIdentifierEvaluate ) );
            BFDP_STATE_ACTION( ParseState::HeaderIdentifier, Exit, CallMethod( *this, &Interpreter::StateHeaderIdentifierExit ) );
            BFDP_STATE_ACTION( ParseState::HeaderEquals, Evaluate, CallMethod( *this, &Interpreter::StateHeaderEqualsEvaluate ) );
            BFDP_STATE_ACTION( ParseState::HeaderParameter, Evaluate, CallMethod( *this, &Interpreter::StateHeaderParameterEvaluate ) );
            BFDP_STATE_ACTION( ParseState::StatementBegin, Evaluate, CallMethod( *this, &Interpreter::StateStatementBeginEvaluate ) );

            BFDP_STATE_MAP_END();

            if( !ok )
            {
                BFDP_RUNTIME_ERROR( "Failed to init state machine" );
                return;
            }

            mStateMachine.Transition( ParseState::HeaderBegin );
            if( !mStateMachine.DoTransition() )
            {
                BFDP_RUNTIME_ERROR( "Failed to init Interpreter" );
                return;
            }
            mInitOk = true;
        }

        bool Interpreter::IsInitOk() const
        {
            return mInitOk;
        }

        void Interpreter::LogError
            (
            std::string const& aMessage
            )
        {
            std::stringstream ss;
            ss << ( aMessage.empty() ? "Unexpected" : aMessage );
            ss << " ";

            switch( mInput.type )
            {
                case In::Control:
                    ss << "control character(s) '" << *(mInput.d.ctrl) << "'";
                    break;

                case In::NumericLiteral:
                    ss << "numeric literal '" << mInput.d.num->GetStr( false ) << "'";
                    break;

                case In::StringLiteral:
                    {
                        // TODO: Create a "printable" converter?
                        Bfdp::Unicode::AsciiConverter conv;
                        std::string out;
                        if( !mInput.d.str->GetString( conv, out ) )
                        {
                            out = mInput.d.str->GetUtf8HexString( " " );
                        }
                        ss << "string literal '" << out << "'";
                    }
                    break;

                case In::Word:
                    ss << "identifier '" << *(mInput.d.word) << "'";
                    break;

                case In::Invalid:
                default:
                    BFDP_INTERNAL_ERROR( "Unhandled input type" );
                    ss << "input type " << mInput.type << " (unknown)";
                    break;
            }

            std::string msg = ss.str();
            BFDP_RUNTIME_ERROR( msg.c_str() );
            mParseError = true;
        }

        void Interpreter::LogError()
        {
            LogError( std::string() );
        }

        bool Interpreter::OnControlCharacter
            (
            std::string const& aControlCharacter
            )
        {
            mInput.type = In::Control;
            mInput.d.ctrl = &aControlCharacter;
            mStateMachine.EvaluateState();
            mInput.type = In::Invalid;
            return !mParseError;
        }

        bool Interpreter::OnNumericLiteral
            (
            Objects::NumericLiteral const& aValue
            )
        {
            mInput.type = In::NumericLiteral;
            mInput.d.num = &aValue;
            mStateMachine.EvaluateState();
            mInput.type = In::Invalid;
            return !mParseError;
        }

        bool Interpreter::OnStringLiteral
            (
            Bfdp::Data::StringMachine const& aValue
            )
        {
            mInput.type = In::StringLiteral;
            mInput.d.str = &aValue;
            mStateMachine.EvaluateState();
            mInput.type = In::Invalid;
            return !mParseError;
        }

        bool Interpreter::OnWord
            (
            std::string const& aValue
            )
        {
            mInput.type = In::Word;
            mInput.d.word = &aValue;
            mStateMachine.EvaluateState();
            return !mParseError;
        }

        void Interpreter::SetStringPropertyDefault
            (
            std::string const& aName,
            std::string const& aValue
            )
        {
            if( mDb->FindProperty( aName ) == NULL )
            {
                if( !SetStringProperty( mDb, aName, aValue ) )
                {
                    LogError( Bfdp::Console::Msg( "Failed to set default for " ) << aName );
                }
            }
        }

        void Interpreter::StateHeaderBeginEntry()
        {
            mIdentifier.clear();
        }

        void Interpreter::StateHeaderBeginEvaluate()
        {
            if( ( mInput.type != In::Control ) ||
                ( *mInput.d.ctrl != ":" ) )
            {
                LogError( "Expected ':', found" );
                return;
            }

            mStateMachine.Transition( ParseState::HeaderIdentifier );
        }

        void Interpreter::StateHeaderIdentifierEvaluate()
        {
            if( ( mInput.type == In::Control ) &&
                ( *mInput.d.ctrl == ":" ) )
            {
                // Blank setting, ignore.
                return;
            }
            else if( mInput.type != In::Word )
            {
                LogError( "Expected identifier, found" );
                return;
            }

            mIdentifier = *mInput.d.word;
            if( mIdentifier == "BFSDL_HEADER" )
            {
                BFDP_RETURNIF_E( mHeaderStreamProgress != Header::StreamBegin, "Duplicate header definition" );

                mHeaderStreamProgress = Header::StreamInProgress;
                mStateMachine.Transition( ParseState::HeaderBegin );
            }
            else if( mIdentifier == "END_HEADER" )
            {
                BFDP_RETURNIF_E( mHeaderStreamProgress != Header::StreamInProgress, "Unexpected end of header stream" );

                mHeaderStreamProgress = Header::StreamDone;
                mStateMachine.Transition( ParseState::StatementBegin );
            }
            else if( mHeaderStreamProgress != Header::StreamInProgress )
            {
                LogError( "Expected 'BFSDL_HEADER', found" );
            }
            else
            {
                mStateMachine.Transition( ParseState::HeaderEquals );
            }
        }

        void Interpreter::StateHeaderIdentifierExit()
        {
            if( mHeaderStreamProgress == Header::StreamDone )
            {
                SetNumericPropertyDefault( "BitBase", BitBase::Default );
                SetNumericPropertyDefault( "DefaultByteOrder", Endianness::Default );
                SetNumericPropertyDefault( "DefaultBitOrder", Endianness::Default );
                SetStringPropertyDefault( "DefaultStringCode", "ASCII" );
                SetNumericPropertyDefault< Bfdp::Unicode::CodePoint >( "DefaultStringTerm", 0U );
                SetNumericPropertyDefault< Objects::BfsdlVersionType >( "Version", 1U );
            }
        }

        void Interpreter::StateHeaderEqualsEvaluate()
        {
            if( ( mInput.type != In::Control ) ||
                ( *mInput.d.ctrl != "=" ) )
            {
                LogError( "Expected '=', found" );
                return;
            }

            mStateMachine.Transition( ParseState::HeaderParameter );
        }

        void Interpreter::StateHeaderParameterEvaluate()
        {
            enum ErrCode
            {
                ErrNone = 0,
                ErrTypeNum,
                ErrTypeStr,
                ErrInvalid,
                ErrUnsupported,
                ErrRuntime,
                ErrRedefinition,
                ErrUnknown
            } errCode = ErrNone;

            if( mIdentifier == "Version" )
            {
                Objects::BfsdlVersionType version = 0;
                if( mInput.type != In::NumericLiteral )
                {
                    errCode = ErrTypeNum;
                }
                else if( mDb->FindProperty( mIdentifier ) != NULL )
                {
                    errCode = ErrRedefinition;
                }
                else if( !mInput.d.num->GetUint( version, Bfdp::BitManip::BytesToBits( sizeof( version ) ) ) )
                {
                    errCode = ErrInvalid;
                }

                if( ( errCode == ErrNone ) && !SetNumericProperty( mDb, mIdentifier, version ) )
                {
                    errCode = ErrRuntime;
                }
            }
            else if( mIdentifier == "BitBase" )
            {
                BitBase::Type bitBase = BitBase::Default;
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else if( mDb->FindProperty( mIdentifier ) != NULL )
                {
                    errCode = ErrRedefinition;
                }
                else if( mInput.d.str->GetUtf8String() == "Bit" )
                {
                    bitBase = BitBase::Bit;
                }
                else if( mInput.d.str->GetUtf8String() == "Byte" )
                {
                    bitBase = BitBase::Byte;
                }
                else
                {
                    errCode = ErrInvalid;
                }

                if( ( errCode == ErrNone ) && !SetNumericProperty( mDb, mIdentifier, bitBase ) )
                {
                    errCode = ErrRuntime;
                }
            }
            else if( mIdentifier == "DefaultByteOrder" )
            {
                Endianness::Type defaultByteOrder = Endianness::Default;
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else if( mInput.d.str->GetUtf8String() == "LE" )
                {
                    defaultByteOrder = Endianness::Little;
                }
                else if( mInput.d.str->GetUtf8String() == "BE" )
                {
                    defaultByteOrder = Endianness::Big;
                }
                else
                {
                    errCode = ErrInvalid;
                }

                if( ( errCode == ErrNone ) && !SetNumericProperty( mDb, mIdentifier, defaultByteOrder ) )
                {
                    errCode = ErrRuntime;
                }
            }
            else if( mIdentifier == "DefaultBitOrder" )
            {
                Endianness::Type defaultBitOrder = Endianness::Default;
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else if( mInput.d.str->GetUtf8String() == "LE" )
                {
                    defaultBitOrder = Endianness::Little;
                }
                else if( mInput.d.str->GetUtf8String() == "BE" )
                {
                    defaultBitOrder = Endianness::Big;
                }
                else
                {
                    errCode = ErrInvalid;
                }

                if( ( errCode == ErrNone ) && !SetNumericProperty( mDb, mIdentifier, defaultBitOrder ) )
                {
                    errCode = ErrRuntime;
                }
            }
            else if( mIdentifier == "DefaultFloatFormat" )
            {
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else
                {
                    // TODO: Add floating point support
                    errCode = ErrUnsupported;
                }
            }
            else if( mIdentifier == "DefaultStringCode" )
            {
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else if( !mInput.d.str->IsDefined() )
                {
                    errCode = ErrInvalid;
                }
                else if( !Bfdp::Unicode::IsValidCoding( mInput.d.str->GetUtf8String() ) )
                {
                    errCode = ErrInvalid;
                }
                else if( !SetStringProperty( mDb, mIdentifier, mInput.d.str->GetUtf8String() ) )
                {
                    errCode = ErrRuntime;
                }
            }
            else if( mIdentifier == "DefaultStringTerm" )
            {
                if( mInput.type != In::NumericLiteral )
                {
                    errCode = ErrTypeNum;
                }
                else
                {
                    Bfdp::Unicode::CodePoint stringTerm = 0U;
                    if( !mInput.d.num->GetUint( stringTerm, Bfdp::BitManip::BytesToBits( sizeof( stringTerm ) ) ) )
                    {
                        errCode = ErrTypeNum;
                    }
                    else if( !Bfdp::Unicode::IsCharacter(stringTerm) )
                    {
                        // Only valid characters are supported as terminators
                        errCode = ErrUnsupported;
                    }
                    else if( !SetNumericProperty( mDb, mIdentifier, stringTerm ) )
                    {
                        errCode = ErrRuntime;
                    }
                }
            }
            else if( mIdentifier == "CustomExtension" )
            {
                if( mInput.type != In::StringLiteral )
                {
                    errCode = ErrTypeStr;
                }
                else
                {
                    // Blue Sky: Plugin architecture to support extensions without forking?
                    errCode = ErrUnsupported;
                }
            }
            else
            {
                errCode = ErrUnknown;
            }

            if( errCode == ErrTypeNum )
            {
                std::stringstream ss;
                ss << mIdentifier << " requires a Numeric Literal parameter, found";
                LogError( ss.str() );
            }
            else if( errCode == ErrTypeStr )
            {
                std::stringstream ss;
                ss << mIdentifier << " requires a String Literal parameter, found";
                LogError( ss.str() );
            }
            else if( errCode == ErrInvalid )
            {
                std::stringstream ss;
                ss << "Invalid value for " << mIdentifier << ":";
                LogError( ss.str() );
            }
            else if( errCode == ErrUnsupported )
            {
                std::stringstream ss;
                ss << "Unsupported value for " << mIdentifier << ":";
                LogError( ss.str() );
            }
            else if( errCode == ErrRuntime )
            {
                std::stringstream ss;
                ss << "Failed to set " << mIdentifier << " to parameter";
                LogError( ss.str() );
            }
            else if( errCode == ErrRedefinition )
            {
                std::stringstream ss;
                ss << "Redefinition of " << mIdentifier << " to parameter";
                LogError( ss.str() );
            }
            else if( errCode == ErrNone )
            {
                mStateMachine.Transition( ParseState::HeaderBegin );
            }
            else
            {
                std::stringstream ss;
                ss << "Unknown config '" << mIdentifier << "' with parameter";
                LogError( ss.str() );
            }
        }

        void Interpreter::StateStatementBeginEvaluate()
        {
            LogError( "Unexpected " );
        }

    } // namespace Token

} // namespace BfsdlParser

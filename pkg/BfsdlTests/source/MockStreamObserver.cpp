/**
    BFDP Mock Stream Observer Definitions

    Copyright 2023, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "BfsdlTests/MockStreamObserver.hpp"

// External Includes
#include <sstream>

#include "Bfdp/BitManip/BitBuffer.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;
    using Stream::Control;

    MockStreamObserver::MockStreamObserver()
    {
    }

    void MockStreamObserver::DoEndOfStream()
    {
        ActionListElement newAction( Action::EndOfStream );
        mActionList.push_back( newAction );
    }

    void MockStreamObserver::DoReadUint
        (
        size_t const aNumBits
        )
    {
        ActionListElement newAction( Action::ReadUint );
        newAction.numBits = aNumBits;
        mActionList.push_back( newAction );
    }

    void MockStreamObserver::DoReturn
        (
        Control::Type aControlCode
        )
    {
        ActionListElement newAction( Action::ControlReturn );
        newAction.controlCode = aControlCode;
        mActionList.push_back( newAction );
    }

    /* static */ std::string MockStreamObserver::Action::ToString
        (
        Type const aValue
        )
    {
        return (aValue == Action::ControlReturn) ? "Return"
            : (aValue == Action::EndOfStream) ? "EndOfStream"
            : (aValue == Action::ReadUint) ? "Read U"
            : "Unknown";
    }

    MockStreamObserver::ActionListElement::ActionListElement
        (
        Action::Type const aActionType
        )
        : type( aActionType )
        , controlCode( Control::Continue )
        , numBits( 0U )
    {
    }

    std::string MockStreamObserver::ActionListElement::GetStr
        (
        std::string const& aPrefix
        ) const
    {
        std::stringstream ss;
        ss << aPrefix << Action::ToString( type );
        if(type == Action::ControlReturn )
        {
            std::string controlStr = (controlCode == Control::Continue) ? "Continue"
                : (controlCode == Control::Error) ? "Error"
                : (controlCode == Control::NoData) ? "NoData"
                : (controlCode == Control::Stop) ? "Stop"
                : "Unknown";
            ss << " " << controlStr;
        }
        else if(type == Action::ReadUint )
        {
            ss << numBits;
        }
        std::string msg = ss.str();
        return msg;
    }

    ::testing::AssertionResult MockStreamObserver::VerifyNone()
    {
        ::testing::AssertionResult result = MockObserverBase::VerifyNone();
        if( !result )
        {
            return result;
        }
        if( !mActionList.empty() )
        {
            std::stringstream ss;
            ss << "Pending actions:" << std::endl;
            while( !mActionList.empty() )
            {
                ActionListElement& nextAction = mActionList.front();
                ss << "  " << nextAction.GetStr() << std::endl;
                mActionList.pop_front();
            }
            return ::testing::AssertionFailure() << ss.str();
        }
        return ::testing::AssertionSuccess();
    }

    Control::Type MockStreamObserver::OnStreamData
        (
        Bfdp::BitManip::GenericBitStream& aInBitStream
        )
    {
        Control::Type ret = Control::Error;
        bool actionsDone = false;
        while( !mActionList.empty() && !actionsDone ) {
            ActionListElement& nextAction = mActionList.front();
            bool notEnoughBitsYet = false;
            if( Action::ControlReturn == nextAction.type )
            {
                RecordEvent( nextAction.GetStr() );
                ret = nextAction.controlCode;
                actionsDone = true;
            }
            else if( Action::EndOfStream == nextAction.type )
            {
                if( 0 == aInBitStream.GetBitsTillEnd() )
                {
                    RecordEvent( nextAction.GetStr() );
                    // Let the stream end naturally instead of explicitly stopping
                    ret = Control::Continue;
                }
                else
                {
                    RecordEvent( nextAction.GetStr( "Unread data at " ) );
                    ret = Control::Error;
                }
                // Either way, we need to return
                actionsDone = true;
            }
            else if( Action::ReadUint == nextAction.type )
            {
                uint64_t uintValue = 0;
                Byte* bytePtrToValue = reinterpret_cast< Byte* >( &uintValue );
                if( nextAction.numBits > ( sizeof( uintValue ) * BitManip::BitsPerByte ) )
                {
                    RecordEvent( nextAction.GetStr( "Unsupported " ) );
                    ret = Control::Error;
                    actionsDone = true;
                }
                else if( aInBitStream.GetBitsTillEnd() < nextAction.numBits )
                {
                    // The number of bits is not yet available
                    notEnoughBitsYet = true;
                    // Do not log an event here, as we haven't actually done anything
                    // and the number of bits cannot be pre-known.
                }
                else if( !aInBitStream.ReadBits( bytePtrToValue, nextAction.numBits ) )
                {
                    RecordEvent( nextAction.GetStr( "Read failure on " ) );
                    ret = Control::Error;
                    actionsDone = true;
                }
                else
                {
                    // Success: Log the value and keep looping for more actions
                    std::stringstream ss;
                    ss << nextAction.GetStr() << ": 0x" << std::hex << uintValue << std::dec;
                    std::string msg = ss.str();
                    RecordEvent( msg );
                }
            }
            else
            {
                RecordEvent( nextAction.GetStr("Unhandled action ") );
                actionsDone = true;
            }
            if( notEnoughBitsYet )
            {
                // Leave the action in the queue but go ahead and return to let
                // more data be piped in.
                ret = Control::Continue;
                break;
            }
            mActionList.pop_front();
        }
        return ret;
    }

} // namespace BfsdlTests

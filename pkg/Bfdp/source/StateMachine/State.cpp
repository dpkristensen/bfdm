/**
    BFDP StateMachine State Definition

    Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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
#include "Bfdp/StateMachine/State.hpp"

// External includes
#include <cstdlib>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/StateMachine/IAction.hpp"

#define BFDP_MODULE "StateMachine::State"

namespace Bfdp
{

    namespace StateMachine
    {

        State::State()
        {
        }

        State::~State()
        {
            for( size_t i = 0; i < ActionTrigger::Count; ++i )
            {
                while( !mActionList[i].empty() )
                {
                    delete mActionList[i].back();
                    mActionList[i].pop_back();
                }
            }
        }

        bool State::AddAction
            (
            ActionTrigger::Type const aTrigger,
            IAction* const aAction
            )
        {
            if( ActionTrigger::Count <= aTrigger )
            {
                BFDP_MISUSE_ERROR( "Invalid action trigger" );
                return false;
            }

            if( aAction == NULL )
            {
                BFDP_RUNTIME_ERROR( "Failed to create action" );
                return false;
            }

            try
            {
                mActionList[aTrigger].push_back( aAction );
            }
            catch( std::exception const& /* exceptionObj */ )
            {
                BFDP_RUNTIME_ERROR( "Failed to add action" );
                return false;
            }

            return true;
        }

        void State::DoAction
            (
            ActionTrigger::Type const aTrigger
            )
        {
            if( ActionTrigger::Count <= aTrigger )
            {
                BFDP_INTERNAL_ERROR( "Invalid action trigger" );
                return;
            }

            ActionList& list = mActionList[aTrigger];
            for( ActionList::iterator iter = list.begin(); iter != list.end(); ++iter )
            {
                (*iter)->Do();
            }
        }

    } // namespace StateMachine

} // namespace Bfdp

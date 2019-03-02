/**
    BFDP StateMachine Engine Definitions

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
#include "Bfdp/StateMachine/Engine.hpp"

// External includes
#include <cstdlib>
#include <new>

// Internal includes
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/StateMachine/State.hpp"

#define BFDP_MODULE "StateMachine::Engine"

namespace Bfdp
{

    namespace StateMachine
    {

        Engine::Engine()
            : mCurState( 0 )
            , mNextState( 0 )
            , mNextStatePending( false )
            , mNumStates( 0 )
            , mStates( NULL )
        {
        }

        Engine::~Engine()
        {
            delete [] mStates;
        }

        bool Engine::AddAction
            (
            SizeT const aStateId,
            ActionTrigger::Type const aTrigger,
            IAction* const aAction
            )
        {
            if( mNumStates <= aStateId )
            {
                BFDP_RUNTIME_ERROR( "Invalid state ID" );
                return false;
            }

            return mStates[aStateId].AddAction( aTrigger, aAction );
        }

        bool Engine::DoTransition()
        {
            if( 0 == mNumStates )
            {
                BFDP_MISUSE_ERROR( "Engine has no states" );
                return false;
            }

            bool const transitionOccurred = mNextStatePending;

            while( mNextStatePending )
            {
                // Reset the flag
                mNextStatePending = false;

                // Save the next state in case Exit requests a transition, to allow the previous
                // transition to complete.
                SizeT targetState = mNextState;

                // If there is a current state, perform Exit actions
                if( mCurState < mNumStates )
                {
                    mStates[mCurState].DoAction( ActionTrigger::Exit );
                }

                // Perform entry actions on the new state
                mCurState = targetState;
                mStates[mCurState].DoAction( ActionTrigger::Entry );
            } // Loop to see if any new transitions were requested

            return transitionOccurred;
        }

        void Engine::EvaluateState()
        {
            if( 0 == mNumStates )
            {
                BFDP_MISUSE_ERROR( "Engine has no states" );
                return;
            }

            if( mCurState < mNumStates )
            {
                mStates[mCurState].DoAction( ActionTrigger::Evaluate );
                DoTransition(); // Perform any queued transitions
            }
        }

        SizeT Engine::GetCurState() const
        {
            return mCurState;
        }

        bool Engine::InitStates
            (
            SizeT const aNumStates
            )
        {
            if( 0 == aNumStates )
            {
                BFDP_MISUSE_ERROR( "Must have more than 0 states" );
                return false;
            }

            mStates = new( std::nothrow ) State[aNumStates];

            if( NULL == mStates )
            {
                BFDP_RUNTIME_ERROR( "Failed to init states" );
                return false;
            }

            mNumStates = aNumStates;
            mCurState = mNumStates;
            return true;
        }

        void Engine::Transition
            (
            SizeT const aNewState
            )
        {
            if( aNewState >= mNumStates )
            {
                BFDP_MISUSE_ERROR( "Invalid transition state" );
                return;
            }

            if( 0 == mNumStates )
            {
                BFDP_MISUSE_ERROR( "Engine has no states" );
                return;
            }

            mNextState = aNewState;
            mNextStatePending = true;
        }

    } // namespace StateMachine

} // namespace Bfdp

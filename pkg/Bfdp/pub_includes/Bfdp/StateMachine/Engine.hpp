/**
    BFDP StateMachine Engine Declarations

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

#ifndef Bfdp_StateMachine_Engine
#define Bfdp_StateMachine_Engine

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// Internal includes
#include "Bfdp/Common.hpp"
#include "Bfdp/StateMachine/ActionTrigger.hpp"
#include "Bfdp/StateMachine/IAction.hpp"

//! Macro to be used at the beginning of state map registration
//!
//! @note To prevent conflicts, do not use variable names beginning with bfdp_ in calling code
#define BFDP_STATE_MAP_BEGIN( _bool_result, _engine, _num_states ) \
    { \
    ::Bfdp::StateMachine::Engine& bfdp_sme = _engine; \
    bool& bfdp_ok = _bool_result; \
    bfdp_ok = bfdp_ok && bfdp_sme.InitStates( _num_states );

//! Macro to define state entry actions
#define BFDP_STATE_ACTION( _id, _trigger, _action ) \
    bfdp_ok = bfdp_ok && bfdp_sme.AddAction \
        ( \
        _id, \
        ::Bfdp::StateMachine::ActionTrigger::_trigger, \
        new( std::nothrow ) _action \
        );

//! Macro to be used at the end of state map registration
#define BFDP_STATE_MAP_END() \
    }

namespace Bfdp
{

    namespace StateMachine
    {

        class State;

        //! Extensible Finite State Machine engine
        class Engine
            : private NonAssignable
            , private NonCopyable
        {
        public:
            //! Constructor
            Engine();

            //! Destructor
            ~Engine();

            //! Add Action
            //!
            //! @return true if action is added successfully
            bool AddAction
                (
                SizeT const aStateId,
                ActionTrigger::Type const aTrigger,
                IAction* const aAction
                );

            //! Make the transition request from the last Transition() call effective
            //!
            //! Any transitions requested on Exit or Entry will take effect immediately,
            //! allowing automatic fall-through to a final state.  However, transitions
            //! on Exit are discouraged, since this occurs as part of another transition,
            //! and it does not block the Entry actions for the target state from being
            //! performed.
            //!
            //! @return true if a transition occurred, false otherwise.
            bool DoTransition();

            //! Evaluate current state
            //!
            //! Causes actions associated with ActionTrigger::Evaluate to be performed on the
            //! current state (if any).
            //!
            //! @note Any transitions requested will be performed after Evaluate actions are
            //!     performed.
            void EvaluateState();

            //! @return the currently active state, or the number of registered states if no
            //!     transitions have occurred yet.
            SizeT GetCurState() const;

            //! Initialize States
            //!
            //! @return true if states are initialized, false otherwise
            bool InitStates
                (
                SizeT const aNumStates
                );

            //! Transition to aNewState
            //!
            //! Change the current state to aNewState;
            //!
            //! Causes actions associated with ActionTrigger::Exit to be performed on the current
            //! state (if any), and actions associated with ActionTrigger::Entry to be performed
            //! on the new state.
            //!
            //! @note This is not executed immediately, which allows action sequences to complete
            //!     prior to executing the next transition.  This will be executed on the next
            //!     call to DoTransition(), however EvaluateState() will keep calling until all
            //!     requests are fulfilled.
            void Transition
                (
                SizeT const aNewState
                );

        private:
            SizeT mCurState;
            SizeT mNextState;
            bool mNextStatePending;
            SizeT mNumStates;
            State* mStates;
        };

    } // namespace StateMachine

} // namespace Bfdp

#endif // Bfdp_StateMachine_Engine

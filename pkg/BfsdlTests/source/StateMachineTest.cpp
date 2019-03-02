/**
    BFDP StateMachine Test

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
#include "BfsdlTests/MockObserverBase.hpp"

// External Includes
#include <gtest/gtest.h>
#include <list>

// Internal Includes
#include "Bfdp/StateMachine/Actions.hpp"
#include "Bfdp/StateMachine/Engine.hpp"
#include "BfsdlTests/TestUtil.hpp"

#define DEF_TEST_STATE( _state, _action ) \
    void _state##_action##() \
    { \
        RecordEvent( BFDP_STRINGIZE( _state ) " " BFDP_STRINGIZE( _action ) ); \
    }

namespace BfsdlTests
{

    using namespace Bfdp;

    struct TestState
    {
        enum Type
        {
            One,
            Two,

            Count
        };

    };

    class StateMachineTest
        : public ::testing::Test
    {
    public:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }

    protected:
        StateMachineTest()
        {
        }
    };

    class TestActionObserver
        : public MockObserverBase

    {
    public:
        DEF_TEST_STATE( One, Entry );
        DEF_TEST_STATE( One, Evaluate );
        DEF_TEST_STATE( One, Exit );
        DEF_TEST_STATE( Two, Entry );
        DEF_TEST_STATE( Two, Evaluate );
        DEF_TEST_STATE( Two, Exit );
    };

    TEST_F( StateMachineTest, Simple )
    {
        StateMachine::Engine engine;
        bool isOk = true;
        TestActionObserver observer;
        typedef StateMachine::CallMethod< TestActionObserver > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, engine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &TestActionObserver::OneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &TestActionObserver::OneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &TestActionObserver::OneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &TestActionObserver::TwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &TestActionObserver::TwoEvaluate ) );
        // Two has no exit actions
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // No transitions pending
        ASSERT_FALSE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::Count, engine.GetCurState() );

        // Call with no active state does nothing
        engine.EvaluateState();
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::Count, engine.GetCurState() );

        // Perform the initial transition, which does not take effect immediately
        engine.Transition( TestState::One );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::Count, engine.GetCurState() );

        // Make the transition effective
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "One Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::One, engine.GetCurState() );

        // A second call to DoTransition() indicates nothing occurred
        ASSERT_FALSE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNone() );

        // Evaluate the state
        engine.EvaluateState();
        ASSERT_TRUE( observer.VerifyNext( "One Evaluate" ) );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::One, engine.GetCurState() );

        engine.Transition( TestState::Two );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "One Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::Two, engine.GetCurState() );

        engine.EvaluateState();
        ASSERT_TRUE( observer.VerifyNext( "Two Evaluate" ) );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::Two, engine.GetCurState() );

        // No exit action registered for Two
        engine.Transition( TestState::One );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "One Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );
        ASSERT_EQ( TestState::One, engine.GetCurState() );
    }

    class RunToCompletionObserver
        : public MockObserverBase
    {
    public:
        enum TransitionBehavior
        {
            FROM_ONE_ENTRY,
            FROM_ONE_EVALUATE,
            FROM_TWO_EXIT
        };

        RunToCompletionObserver
            (
            StateMachine::Engine& aEngine,
            TransitionBehavior const aTransitionFrom
            )
            : mEngine( aEngine )
            , mTransitionFrom( aTransitionFrom )
        {
        }

        void OneEntry()
        {
            RecordEvent( "One Entry" );
            if( mTransitionFrom == FROM_ONE_ENTRY )
            {
                TransitionToTwo();
            }
        }

        void OneEvaluate()
        {
            RecordEvent( "One Evaluate" );
            if( mTransitionFrom == FROM_ONE_EVALUATE )
            {
                TransitionToTwo();
            }
        }

        DEF_TEST_STATE( One, Exit );
        DEF_TEST_STATE( Two, Entry );
        DEF_TEST_STATE( Two, Evaluate );

        void TwoExit()
        {
            RecordEvent( "Two Exit" );
            if( mTransitionFrom == FROM_TWO_EXIT )
            {
                TransitionToTwo();
            }
        }

    private:
        void TransitionToTwo()
        {
            mEngine.Transition( TestState::Two );
            RecordEvent( "Return" );
        }

        TransitionBehavior const mTransitionFrom;
        StateMachine::Engine& mEngine;
    };

    TEST_F( StateMachineTest, RunToCompletionReturnFromTransitionFirst )
    {
        StateMachine::Engine engine;
        bool isOk = true;
        RunToCompletionObserver observer( engine, RunToCompletionObserver::FROM_ONE_EVALUATE );

        typedef StateMachine::CallMethod< RunToCompletionObserver > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, engine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &RunToCompletionObserver::OneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &RunToCompletionObserver::OneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &RunToCompletionObserver::OneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &RunToCompletionObserver::TwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &RunToCompletionObserver::TwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &RunToCompletionObserver::TwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state One first
        engine.Transition( TestState::One );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "One Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Cause a transition from Evaluate and verify the method returns prior to the transition.
        // The pending transition take effect automatically.
        engine.EvaluateState();
        ASSERT_TRUE( observer.VerifyNext( "One Evaluate" ) );
        ASSERT_TRUE( observer.VerifyNext( "Return" ) );
        ASSERT_TRUE( observer.VerifyNext( "One Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Verify no transition is pending.
        ASSERT_FALSE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( StateMachineTest, RunToCompletionTransitionOnExit )
    {
        StateMachine::Engine engine;
        bool isOk = true;
        RunToCompletionObserver observer( engine, RunToCompletionObserver::FROM_TWO_EXIT );

        typedef StateMachine::CallMethod< RunToCompletionObserver > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, engine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &RunToCompletionObserver::OneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &RunToCompletionObserver::OneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &RunToCompletionObserver::OneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &RunToCompletionObserver::TwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &RunToCompletionObserver::TwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &RunToCompletionObserver::TwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state Two first
        engine.Transition( TestState::Two );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Cause a transition from Exit and verify the method returns prior to the transition.
        // The pending transition will take effect automatically.
        // TODO: Should it be allowed that the internal transition can completely override the
        // external transition, such that One is not entered at all?  Maybe as an option flag?
        engine.Transition( TestState::One );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "Two Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "Return" ) );
        ASSERT_TRUE( observer.VerifyNext( "One Entry" ) );
        ASSERT_TRUE( observer.VerifyNext( "One Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Verify no transition is pending.
        ASSERT_FALSE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    TEST_F( StateMachineTest, RunToCompletionTransitionOnEntry )
    {
        StateMachine::Engine engine;
        bool isOk = true;
        RunToCompletionObserver observer( engine, RunToCompletionObserver::FROM_ONE_ENTRY );

        typedef StateMachine::CallMethod< RunToCompletionObserver > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, engine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &RunToCompletionObserver::OneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &RunToCompletionObserver::OneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &RunToCompletionObserver::OneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &RunToCompletionObserver::TwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &RunToCompletionObserver::TwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &RunToCompletionObserver::TwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state Two first
        engine.Transition( TestState::Two );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Cause a transition from Entry and verify the method returns prior to the transition.
        // The pending transition will take effect automatically.
        engine.Transition( TestState::One );
        ASSERT_TRUE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNext( "Two Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "One Entry" ) );
        ASSERT_TRUE( observer.VerifyNext( "Return" ) );
        ASSERT_TRUE( observer.VerifyNext( "One Exit" ) );
        ASSERT_TRUE( observer.VerifyNext( "Two Entry" ) );
        ASSERT_TRUE( observer.VerifyNone() );

        // Verify no transition is pending.
        ASSERT_FALSE( engine.DoTransition() );
        ASSERT_TRUE( observer.VerifyNone() );
    }

    /* TODO: More robust action system to represent as much logic in state map as possible:
        Saved global variables in engine:
        * Has enumerated ID for lookup in actions?
        * Can be used as conditionals to perform actions
        * Save from method call
        * Save from pointer address
        * Load to pointer address
        * Pass as parameter to method

        Other actions:
        * Call static method
        * Save variable
        * Transition to another state
        * Do action on condition (saved variable)
        * Do action on condition (state ID)
        * Do the first action in a set whose condition is met, or an "else" action
    */

} // namespace BfsdlTests

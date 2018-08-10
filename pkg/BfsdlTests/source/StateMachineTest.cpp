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

// External Includes
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Internal Includes
#include "Bfdp/StateMachine/Actions.hpp"
#include "Bfdp/StateMachine/Engine.hpp"
#include "BfsdlTests/TestUtil.hpp"

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
    {
    public:
        typedef ::testing::StrictMock< TestActionObserver > Type;

        MOCK_METHOD0( TestActionOneEntry, void() );
        MOCK_METHOD0( TestActionOneEvaluate, void() );
        MOCK_METHOD0( TestActionOneExit, void() );
        MOCK_METHOD0( TestActionTwoEntry, void() );
        MOCK_METHOD0( TestActionTwoEvaluate, void() );
        MOCK_METHOD0( TestActionTwoExit, void() );

    protected:
        TestActionObserver()
        {
        }
    };

    TEST_F( StateMachineTest, Simple )
    {
        StateMachine::Engine engine;
        bool isOk = true;
        TestActionObserver::Type observer;
        typedef StateMachine::CallMethod< TestActionObserver::Type > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, engine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &TestActionObserver::TestActionOneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &TestActionObserver::TestActionOneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &TestActionObserver::TestActionOneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &TestActionObserver::TestActionTwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &TestActionObserver::TestActionTwoEvaluate ) );
        // Two has no exit actions
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // No transitions pending
        ASSERT_FALSE( engine.DoTransition() );

        // Call with no active state does nothing
        engine.EvaluateState();

        // Perform the initial transition, which does not take effect immediately
        engine.Transition( TestState::One );

        // Make the transition effective
        EXPECT_CALL( observer, TestActionOneEntry() );
        ASSERT_TRUE( engine.DoTransition() );

        // A second call to DoTransition() indicates nothing occurred
        ASSERT_FALSE( engine.DoTransition() );

        EXPECT_CALL( observer, TestActionOneEvaluate() );
        engine.EvaluateState();

        engine.Transition( TestState::Two );
        EXPECT_CALL( observer, TestActionOneExit() );
        EXPECT_CALL( observer, TestActionTwoEntry() );
        ASSERT_TRUE( engine.DoTransition() );

        EXPECT_CALL( observer, TestActionTwoEvaluate() );
        engine.EvaluateState();

        // No exit action registered for Two
        engine.Transition( TestState::One );
        EXPECT_CALL( observer, TestActionOneEntry() );
        ASSERT_TRUE( engine.DoTransition() );
    }

    class RunToCompletionModel
    {
    public:
        void TransitionToTwo()
        {
            stateMachine.Transition( TestState::Two );
            HasReturned();
        }

        MOCK_METHOD0( HasReturned, void() );

        StateMachine::Engine stateMachine;
    };

    TEST_F( StateMachineTest, RunToCompletionReturnFromTransitionFirst )
    {
        bool isOk = true;
        RunToCompletionModel model;
        TestActionObserver::Type observer;

        typedef StateMachine::CallMethod< TestActionObserver::Type > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, model.stateMachine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &TestActionObserver::TestActionOneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &TestActionObserver::TestActionOneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &TestActionObserver::TestActionOneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &TestActionObserver::TestActionTwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &TestActionObserver::TestActionTwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &TestActionObserver::TestActionTwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state One first
        ::testing::Sequence s;
        model.stateMachine.Transition( TestState::One );
        EXPECT_CALL( observer, TestActionOneEntry() )
            .InSequence( s );
        ASSERT_TRUE( model.stateMachine.DoTransition() );

        // Cause a transition from Evaluate and verify the method returns prior to the transition.
        // The pending transition take effect automatically.
        EXPECT_CALL( observer, TestActionOneEvaluate() )
            .InSequence( s )
            .WillOnce( ::testing::Invoke( &model, &RunToCompletionModel::TransitionToTwo ) );;
        EXPECT_CALL( model, HasReturned() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionOneExit() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionTwoEntry() )
            .InSequence( s );
        model.stateMachine.EvaluateState();

        // Verify no transition is pending.
        ASSERT_FALSE( model.stateMachine.DoTransition() );
    }

    TEST_F( StateMachineTest, RunToCompletionTransitionOnExit )
    {
        bool isOk = true;
        RunToCompletionModel model;
        TestActionObserver::Type observer;

        typedef StateMachine::CallMethod< TestActionObserver::Type > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, model.stateMachine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &TestActionObserver::TestActionOneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &TestActionObserver::TestActionOneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &TestActionObserver::TestActionOneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &TestActionObserver::TestActionTwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &TestActionObserver::TestActionTwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &TestActionObserver::TestActionTwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state Two first
        ::testing::Sequence s;
        model.stateMachine.Transition( TestState::Two );
        EXPECT_CALL( observer, TestActionTwoEntry() )
            .InSequence( s );
        ASSERT_TRUE( model.stateMachine.DoTransition() );

        // Cause a transition from Exit and verify the method returns prior to the transition.
        // The pending transition will take effect automatically.
        model.stateMachine.Transition( TestState::One );
        EXPECT_CALL( observer, TestActionTwoExit() )
            .InSequence( s )
            .WillOnce( ::testing::Invoke( &model, &RunToCompletionModel::TransitionToTwo ) );;
        EXPECT_CALL( model, HasReturned() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionOneEntry() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionOneExit() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionTwoEntry() )
            .InSequence( s );
        ASSERT_TRUE( model.stateMachine.DoTransition() );

        // Verify no transition is pending.
        ASSERT_FALSE( model.stateMachine.DoTransition() );
    }

    TEST_F( StateMachineTest, RunToCompletionTransitionOnEntry )
    {
        bool isOk = true;
        RunToCompletionModel model;
        TestActionObserver::Type observer;

        typedef StateMachine::CallMethod< TestActionObserver::Type > CallMethod;

        BFDP_STATE_MAP_BEGIN( isOk, model.stateMachine, TestState::Count );
        BFDP_STATE_ACTION( TestState::One, Entry, CallMethod( observer, &TestActionObserver::TestActionOneEntry ) );
        BFDP_STATE_ACTION( TestState::One, Evaluate, CallMethod( observer, &TestActionObserver::TestActionOneEvaluate ) );
        BFDP_STATE_ACTION( TestState::One, Exit, CallMethod( observer, &TestActionObserver::TestActionOneExit ) );
        BFDP_STATE_ACTION( TestState::Two, Entry, CallMethod( observer, &TestActionObserver::TestActionTwoEntry ) );
        BFDP_STATE_ACTION( TestState::Two, Evaluate, CallMethod( observer, &TestActionObserver::TestActionTwoEvaluate ) );
        BFDP_STATE_ACTION( TestState::Two, Exit, CallMethod( observer, &TestActionObserver::TestActionTwoExit ) );
        BFDP_STATE_MAP_END();
        ASSERT_TRUE( isOk );

        // Get into state Two first
        ::testing::Sequence s;
        model.stateMachine.Transition( TestState::Two );
        EXPECT_CALL( observer, TestActionTwoEntry() )
            .InSequence( s );
        ASSERT_TRUE( model.stateMachine.DoTransition() );

        // Cause a transition from Entry and verify the method returns prior to the transition.
        // The pending transition will take effect automatically.
        model.stateMachine.Transition( TestState::One );
        EXPECT_CALL( observer, TestActionTwoExit() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionOneEntry() )
            .InSequence( s )
            .WillOnce( ::testing::Invoke( &model, &RunToCompletionModel::TransitionToTwo ) );
        EXPECT_CALL( model, HasReturned() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionOneExit() )
            .InSequence( s );
        EXPECT_CALL( observer, TestActionTwoEntry() )
            .InSequence( s );
        ASSERT_TRUE( model.stateMachine.DoTransition() );

        // Verify no transition is pending.
        ASSERT_FALSE( model.stateMachine.DoTransition() );
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

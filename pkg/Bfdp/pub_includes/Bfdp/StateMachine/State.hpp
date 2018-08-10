/**
    BFDP StateMachine State Declaration

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

#ifndef Bfdp_StateMachine_State
#define Bfdp_StateMachine_State

// Base includes
#include "Bfdp/NonAssignable.hpp"
#include "Bfdp/NonCopyable.hpp"

// External includes
#include <list>

// Internal Includes
#include "Bfdp/StateMachine/ActionTrigger.hpp"

namespace Bfdp
{

    namespace StateMachine
    {

        class IAction;

        //! A state endpoint with customizable behavior
        class State
            : private NonAssignable
            , private NonCopyable
        {
        public:
            //! Constructor
            State();

            //! Destructor
            ~State();

            //! Add Action
            //!
            //! @return true if action is added successfully
            bool AddAction
                (
                ActionTrigger::Type const aTrigger,
                IAction* const aAction
                );

            //! Do Action
            void DoAction
                (
                ActionTrigger::Type const aTrigger
                );

        private:
            typedef std::list< IAction* > ActionList;

            ActionList mActionList[ActionTrigger::Count];
        };

    } // namespace StateMachine

} // namespace Bfdp

#endif // Bfdp_StateMachine_State

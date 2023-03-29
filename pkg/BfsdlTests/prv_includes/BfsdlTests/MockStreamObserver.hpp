/**
    BFDP Mock Stream Observer Declarations

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

#ifndef BfsdlTests_MockStreamObserver
#define BfsdlTests_MockStreamObserver

// Base Includes
#include "Bfdp/Stream/IStreamObserver.hpp"
#include "BfsdlTests/MockObserverBase.hpp"

// External Includes
#include "gtest/gtest.h"

// Internal Includes
#include "Bfdp/Stream/StreamBase.hpp"

namespace BfsdlTests
{

    //! Test double for handling stream callbacks
    //!
    //! Unlike other test doubles using MockObserverBase, this class requires
    //! placing expectations for what to do ahead of time since parsing a
    //! stream requires active control to affect the result.
    class MockStreamObserver
        : public Bfdp::Stream::IStreamObserver
        , public BfsdlTests::MockObserverBase
    {
    public:
        MockStreamObserver();

        void DoEndOfStream();

        void DoReadUint
            (
            size_t const aNumBits
            );

        void DoReturn
            (
            Bfdp::Stream::Control::Type aControlCode
            );

        BFDP_OVERRIDE( ::testing::AssertionResult VerifyNone() );

    private:
        struct Action
        {
            enum Type
            {
                ControlReturn,
                EndOfStream,
                ReadUint
            };

            static std::string ToString
                (
                Type const aValue
                );
        };

        struct ActionListElement
        {
            ActionListElement
                (
                Action::Type const aActionType
                );

            std::string GetStr
                (
                std::string const& aPrefix = std::string()
                ) const;

            Action::Type type;
            Bfdp::Stream::Control::Type controlCode;
            size_t numBits;
        };

        typedef std::list< ActionListElement > ActionList;

        BFDP_OVERRIDE( Bfdp::Stream::Control::Type OnStreamData
            (
            Bfdp::BitManip::GenericBitStream& aInBitStream
            ) );

        ActionList mActionList;
    };

} // namespace BfsdlTests

#endif // BfsdlTests_MockStreamObserver

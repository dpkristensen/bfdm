/**
    BFDP Unicode Functions Tests

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

// External includes
#include <iomanip>
#include <map>
#include "gtest/gtest.h"

// Internal includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/Unicode/Functions.hpp"
#include "BfsdlTests/MockErrorHandler.hpp"
#include "BfsdlTests/TestUtil.hpp"

namespace BfsdlTests
{

    using namespace Bfdp;

    class UnicodeFunctionTest
        : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            SetDefaultErrorHandlers();
        }
    };

    TEST_F( UnicodeFunctionTest, PropertyTest )
    {
        static struct TestData {
            Unicode::CodePoint cp;
            bool isCharacter;
            bool isValid;
        } TEST_DATA[] = {
            { 0x00000000,  true,  true },
            { 0x0000FDCF,  true,  true },
            { 0x0000FDD0, false,  true },
            { 0x0000FDEF, false,  true },
            { 0x0000FDF0,  true,  true },
            { 0x0000FFFD,  true,  true },
            { 0x0000FFFE, false,  true },
            { 0x0000FFFF, false,  true },
            { 0x00010000,  true,  true },
            { 0x0001FFFD,  true,  true },
            { 0x0001FFFE, false,  true },
            { 0x0001FFFF, false,  true },
            { 0x00020000,  true,  true },
            { 0x0002FFFD,  true,  true },
            { 0x0002FFFE, false,  true },
            { 0x0002FFFF, false,  true },
            { 0x00030000,  true,  true },
            { 0x0003FFFD,  true,  true },
            { 0x0003FFFE, false,  true },
            { 0x0003FFFF, false,  true },
            { 0x00040000,  true,  true },
            { 0x0004FFFD,  true,  true },
            { 0x0004FFFE, false,  true },
            { 0x0004FFFF, false,  true },
            { 0x00050000,  true,  true },
            { 0x0005FFFD,  true,  true },
            { 0x0005FFFE, false,  true },
            { 0x0005FFFF, false,  true },
            { 0x00060000,  true,  true },
            { 0x0006FFFD,  true,  true },
            { 0x0006FFFE, false,  true },
            { 0x0006FFFF, false,  true },
            { 0x00070000,  true,  true },
            { 0x0007FFFD,  true,  true },
            { 0x0007FFFE, false,  true },
            { 0x0007FFFF, false,  true },
            { 0x00080000,  true,  true },
            { 0x0008FFFD,  true,  true },
            { 0x0008FFFE, false,  true },
            { 0x0008FFFF, false,  true },
            { 0x00090000,  true,  true },
            { 0x0009FFFD,  true,  true },
            { 0x0009FFFE, false,  true },
            { 0x0009FFFF, false,  true },
            { 0x000A0000,  true,  true },
            { 0x000AFFFD,  true,  true },
            { 0x000AFFFE, false,  true },
            { 0x000AFFFF, false,  true },
            { 0x000B0000,  true,  true },
            { 0x000BFFFD,  true,  true },
            { 0x000BFFFE, false,  true },
            { 0x000BFFFF, false,  true },
            { 0x000C0000,  true,  true },
            { 0x000CFFFD,  true,  true },
            { 0x000CFFFE, false,  true },
            { 0x000CFFFF, false,  true },
            { 0x000D0000,  true,  true },
            { 0x000DFFFD,  true,  true },
            { 0x000DFFFE, false,  true },
            { 0x000DFFFF, false,  true },
            { 0x000E0000,  true,  true },
            { 0x000EFFFD,  true,  true },
            { 0x000EFFFE, false,  true },
            { 0x000EFFFF, false,  true },
            { 0x000F0000,  true,  true },
            { 0x000FFFFD,  true,  true },
            { 0x000FFFFE, false,  true },
            { 0x000FFFFF, false,  true },
            { 0x00100000,  true,  true },
            { 0x0010FFFD,  true,  true },
            { 0x0010FFFE, false,  true },
            { 0x0010FFFF, false,  true },
            { 0x00110000,  true,  true },
            { 0x0011FFFD,  true,  true },
            { 0x0011FFFE,  true,  true },
            { 0x0011FFFF,  true,  true },
            { 0x7FFFFFFF,  true,  true },
            { 0x80000000, false, false },
        };
        for(size_t i = 0; i < BFDP_COUNT_OF_ARRAY(TEST_DATA); ++i) {
            SCOPED_TRACE(::testing::Message("cp=0x") << std::hex << std::setfill('0') << std::setw(8) << TEST_DATA[i].cp << std::dec);
            ASSERT_EQ(TEST_DATA[i].isCharacter, Unicode::IsCharacter(TEST_DATA[i].cp));
            ASSERT_EQ(TEST_DATA[i].isValid, Unicode::IsValidCodePoint(TEST_DATA[i].cp));
        }
    }

} // namespace BfsdlTests

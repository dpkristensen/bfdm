/**
    BFDP Unicode Coding Map Definitions

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
#include "Bfdp/Unicode/CodingMap.hpp"

// External Includes
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>

// Internal Includes
#include "Bfdp/Macros.hpp"
#include "Bfdp/ErrorReporter/Functions.hpp"
#include "Bfdp/Unicode/AsciiConverter.hpp"
#include "Bfdp/Unicode/Ms1252Converter.hpp"

#define BFDP_MODULE "Unicode::CodingMap"

#define DECLARE_CODING( _name ) \
    static FactoryFn _name##FamilyLookup \
        ( \
        char const* const aRef \
        )

#define DEFINE_SINGLE_CODING( _name ) \
    static FactoryFn _name##FamilyLookup \
        ( \
        char const* const /* aRef */ \
        ) \
    { \
        return Factory< _name##Converter >; \
    }

namespace Bfdp
{

    namespace Unicode
    {

        namespace CodingMapInternal
        {
            typedef IConverterPtr (*FactoryFn)();

            typedef FactoryFn (*FamilyLookupFn)
                (
                char const* const aRef
                );

            struct CodingFamilyInfo
            {
                std::string mName;
                FamilyLookupFn mLookup;
            };
        }
        using namespace CodingMapInternal;

        // Helper function declarations

        template< class T >
        static std::shared_ptr< IConverter > Factory();

        static FactoryFn FindFactory
            (
            std::string const& aCoding
            );

        // Codec access function declarations

        DECLARE_CODING( Ascii );
        DECLARE_CODING( Microsoft );

        static FactoryFn UnsupportedFamily
            (
            char const* const aRef
            );

        // Codec familiy table (must be below codec functions)

        CodingFamilyInfo const sFamily[] =
        {
            { std::string( "ASCII" ), AsciiFamilyLookup },
            { std::string( "HP" ), UnsupportedFamily },
            { std::string( "IBM" ), UnsupportedFamily },
            { std::string( "IEC" ), UnsupportedFamily },
            { std::string( "ISO" ), UnsupportedFamily },
            { std::string( "MS" ), MicrosoftFamilyLookup },
        };
        BFDP_CTIME_ASSERT( BFDP_COUNT_OF_ARRAY( sFamily ) == CodingFamily::Count, "Coding family table mismatch" );

        // PUBLIC API

        IConverterPtr GetCodec
            (
            CodingId const aCodingId
            )
        {
            if( InvalidCodingId == aCodingId )
            {
                return NULL;
            }

            FactoryFn factory = reinterpret_cast< FactoryFn >( aCodingId );
            return factory();
        }

        CodingId GetCodingId
            (
            std::string const& aCoding
            )
        {
            return FindFactory( aCoding );
        }

        bool IsValidCoding
            (
            std::string const& aCoding
            )
        {
            return NULL != FindFactory( aCoding );
        }

        // HELPER FUNCTIONS

        template< class T >
        static std::shared_ptr< IConverter > Factory()
        {
            return std::make_shared< T >();
        }

        static FactoryFn FindFactory
            (
            std::string const& aCoding
            )
        {
            for( size_t i = 0; i < CodingFamily::Count; ++i )
            {
                if( 0 == std::strncmp( aCoding.c_str(), sFamily[i].mName.c_str(), sFamily[i].mName.length() ) )
                {
                    char const* refPtr = aCoding.c_str();
                    refPtr += sFamily[i].mName.length();
                    return sFamily[i].mLookup( refPtr );
                }
            }

            return NULL;
        }

        // CODEC ACCESS FUNCTIONS

        DEFINE_SINGLE_CODING( Ascii );

        static FactoryFn MicrosoftFamilyLookup
            (
            char const* const aRef
            )
        {
            static struct MsPageType
            {
                unsigned long int mPage;
                FactoryFn mFactory;
            } const sMsPages[] =
            {
                { 1252, Factory< Ms1252Converter > },
            };
            static size_t BFDP_CONSTEXPR sNumMsPages = BFDP_COUNT_OF_ARRAY( sMsPages );

            BFDP_RETURNIF_V( aRef[0] != '-', NULL );

            unsigned long int page = std::strtoul( &aRef[1], NULL, 10 );
            for( size_t i = 0; i < sNumMsPages; ++i )
            {
                if( sMsPages[i].mPage == page )
                {
                    return sMsPages[i].mFactory;
                }
            }

            return NULL;
        }

        static FactoryFn UnsupportedFamily
            (
            char const* const /* aRef */
            )
        {
            return NULL;
        }

    } // namespace Unicode

} // namespace Bfdp

/**
    BFDP Macro Definitinos

    Copyright 2016-2019, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
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

#ifndef Bfdp_Macros
#define Bfdp_Macros

#define BFDP_COUNT_OF_ARRAY( _a ) ( sizeof( _a ) / sizeof( _a[0] ) )

#define BFDP_CONCATENATE( _a, _b ) a##b

#define __BFDP_STRINGIZE( _text ) #_text
#define BFDP_STRINGIZE( _text ) __BFDP_STRINGIZE( _text )

/**
    @fn BFDP_CTIME_ASSERT
    @brief Compile-time assert macro

    @param _expr Compile-time constant expression to be evaluated (true = pass, false = fail)
    @param _msg Identifier to serve as a message (not a string! must be usable as a variable name)
*/
#if( ( __cplusplus >= 201103L ) || \
     ( defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) ) )
    #define BFDP_CTIME_ASSERT( _expr, _msg ) static_assert( _expr, BFDP_STRINGIZE( _msg ) )
#else
    #define BFDP_CTIME_ASSERT( _expr, _msg ) \
        struct BFDP_CONCATENATE( ctime_assert_check_, __LINE__ ) \
        { \
            int _msg:(( _expr ) ? 1 : -1); \
        };
#endif

#if( ( __cplusplus >= 201103L ) || \
     ( defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) ) )
    #define BFDP_CONSTEXPR constexpr
#else
    #define BFDP_CONSTEXPR const
#endif

#if( ( __cplusplus >= 201103L ) || \
     ( defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) ) )
    #define BFDP_FINAL  final
#else
    #define BFDP_FINAL
#endif

#define BFDP_OFFSET_BYTE( _ptr, _pos ) \
    ( reinterpret_cast< char* >( _ptr  ) + _pos )

#if( ( __cplusplus >= 201103L ) || \
     ( defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) ) )
    #define BFDP_OVERRIDE( _declaration ) \
        _declaration override
#else
    #define BFDP_OVERRIDE( _declaration ) \
        virtual _declaration
#endif

//! If condition, return
#define BFDP_RETURNIF( _cond ) \
    if( _cond ) \
    { \
        return; \
    }

//! If condition, return value
#define BFDP_RETURNIF_V( _cond, _value ) \
    if( _cond ) \
    { \
        return _value; \
    }

#define BFDP_UNUSED_PARAMETER( _param ) (void)( _param )

#define BFDP_UNUSED_RETURN( _func ) (void)_func

#endif // Bfdp_Macros

#pragma once
#include "Platform/Platform.h"

//Platform independant macros for compiler specific functions
#if FL_ENABLED(FL_COMPILER_GCC) || FL_ENABLED(FL_COMPILER_CLANG)
	#define FL_ALIGN_DECL(_align, _decl)	_decl __attribute__( (aligned(_align) ) )
	#define FL_FORCE_INLINE inline			__attribute__( (__always_inline__) )
	#define FL_FUNCTION						__PRETTY_FUNCTION__
	#define FL_NO_INLINE					__attribute__( (noinline) )

	#define FL_DLL_EXPORT
#elif FL_ENABLED(FL_COMPILER_MSVC)
	#define FL_ALIGN_DECL(_align, _decl)	__declspec(align(_align) ) _decl
	#define FL_FUNCTION						__FUNCTION__
	#define FL_FORCE_INLINE					__forceinline
	#define FL_NO_INLINE					__declspec(noinline)

	#ifdef FL_COMPILE_AS_DLL
		#define FL_DLL_EXPORT				__declspec(dllexport)
	#else
		#define FL_DLL_EXPORT				__declspec(dllimport)
	#endif
#else
	#error "Unknown Compiler"
#endif
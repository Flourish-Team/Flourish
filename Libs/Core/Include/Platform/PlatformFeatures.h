#pragma once

#include "Platform.h"
#include "Warnings.h"

//Common Includes
#include <cstdint>

//Platform independant macros for compiler specific functions
#if FL_ENABLED(FL_COMPILER_GCC) || FL_ENABLED(FL_COMPILER_CLANG)
	#define FL_ALIGN(alignment)				__attribute__((aligned(alignment)))
	#define FL_FORCE_INLINE					__attribute__( (__always_inline__) )
	#define FL_FUNCTION						__PRETTY_FUNCTION__
	#define FL_NO_INLINE					__attribute__( (noinline) )

#define FL_DLL_EXPORT
#elif FL_ENABLED(FL_COMPILER_MSVC)
	#define FL_ALIGN(alignment)				__declspec(align(alignment))
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

//Help functions for aligning common types correctly
#define FL_ALIGNED_STRUCT(alignment, structName) struct FL_ALIGN(alignment) structName
#define FL_ALIGNED_CLASS(alignment, structName) class FL_ALIGN(alignment) structName
#define FL_ALIGNED_UNION(alignment, structName) union FL_ALIGN(alignment) structName
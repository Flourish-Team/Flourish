#pragma once

#include "Platform.h"

#if FL_ENABLED(FL_COMPILER_GCC)
#define FL_WARNINGS_PUSH	#pragma GCC diagnostic push
#define FL_WARNINGS_POP		#pragma GCC diagnostic pop
#elif FL_ENABLED(FL_COMPILER_CLANG)
#define FL_WARNINGS_PUSH	#pragma clang diagnostic push
#define FL_WARNINGS_POP		#pragma clang diagnostic pop
#elif FL_ENABLED(FL_COMPILER_MSVC)
#define FL_WARNINGS_PUSH	__pragma(warning(push))	
#define FL_WARNINGS_POP		__pragma(warning(pop))	
#else
#error "Unknown Compiler"
#endif

//Below we define common errors in a cross compiler way so you can disable the on
//all compilers at once

// Conditional Expression is Constant
#if FL_ENABLED(FL_COMPILER_GCC)
#define FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT	//No Warning on this compiler
#elif FL_ENABLED(FL_COMPILER_CLANG)
#define FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT	//No Warning on this compiler
#elif FL_ENABLED(FL_COMPILER_MSVC)
#define FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT	__pragma(warning(disable:4127))	
#else
#error "Unknown Compiler"
#endif

// structure was padded due to alignment
#if FL_ENABLED(FL_COMPILER_GCC)
#define FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT_DUE_TO_ALIGNMENT	#pragma GCC diagnostic ignored "-Wpadded"
#elif FL_ENABLED(FL_COMPILER_CLANG)
#define FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT_DUE_TO_ALIGNMENT	#pragma clang diagnostic ignored "-Wpadded"
#elif FL_ENABLED(FL_COMPILER_MSVC)
#define FL_DISABLE_WARNING__STRUCTURE_WAS_PADDED_DUE_TO_ALIGNMENT	__pragma(warning(disable:4324))	\
																	__pragma(warning(disable:4820))	
#else
#error "Unknown Compiler"
#endif
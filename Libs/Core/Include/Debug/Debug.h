#pragma once

// Will cause the program to break when called
#define FL_BREAKPOINT() Flourish::Debug::DebugBreak();

// Will cause the program to break if conditon evaluates to true
#define FL_BREAKPOINT_COND(conditon)		\
	do										\
	{										\
		if((conditon))						\
		{									\
			FL_BREAKPOINT();				\
		}									\
		__pragma(warning(push))				\
		__pragma(warning(disable:4127))		\
	} while(0)								\
	__pragma(warning(pop))	

//Outputs text for debugging purposes using printf style formating
#define FL_DEBUG_PRINTF(x, ...) Flourish::Debug::DebugOutputf((x), __VA_ARGS__)

namespace Flourish { namespace Debug
{
	// Will cause the program to break when called
	void DebugBreak();

	//Outputs text for debugging purposes using printf style formating
	void DebugPrintf(const char* message, ...);

	//TODO add crossplatform stacktrace funtions
}}

//Include inline impimentation
#define INCLUDING_DEBUG_IMP_FILE
#include "Internal/Debug.inl"
#undef INCLUDING_STRING_IMP_FILE
#pragma once

// Causes a break in the program to be triggered
#define FL_DEBUG_BREAK() \
	_IMPL_FL_DEBUG_BREAK()

// Causes a break in the program to be triggered if the condition is true
#define FL_DEBUG_BREAK_COND(condition) \
	_IMPL_FL_DEBUG_BREAK_COND((condition))

namespace Flourish { namespace Debug
{
	//Outputs text for debugging purposes using printf style formating
	void DebugPrintf(const char* message, ...);

	//Same as above but takes a va_list instead to allow forwarding
	void DebugPrintfVArgs(const char* message, va_list args);

	enum class StackTraceResult
	{
		OK,
		OKTruncated,
		Failed,
	}; 

	// Walks the stack and converts it to a string using symbols is found to get function Names.
	// numFramesToOmit sets how many stack frames to skip in the output
	// Return value indicated if the stacktrace was obtained ok and if it was truncated
	StackTraceResult GetStackTrace(int numFramesToOmit,	char* callstackBufOut, int callstackBufOutLength );
}}

//Include inline impimentation
#include "Internal/Debug.inl"
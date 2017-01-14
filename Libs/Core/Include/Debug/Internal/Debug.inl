#pragma once
#include <cstdio>

#ifndef INCLUDING_DEBUG_IMP_FILE
#error Trying to include String.inl separately. Include String.h instead
#else

#include "Platform/Platform.h"
#include "debugbreak.h"

namespace Flourish { namespace Debug
{
	inline void DebugBreak()
	{
		//Use the lovely debugbreak lib
		debug_break();
	}

	inline void DebugPrintf(const char* message, ...)
	{
		const size_t bufferSize = 1024; 
		char messageBuffer[bufferSize];

		int numCharsWritten;

		va_list args;
		va_start(args, message);
		numCharsWritten = snprintf(messageBuffer, bufferSize, message, args);
		va_end(args);

		if(numCharsWritten >= bufferSize)
		{
			//Unable to write the full message to the buffer, allocate a correct size buffer and try again
			//TODO: finish this when stack memor area is finished
		}

	#if FL_ENABLED(FL_PLATFORM_WINDOWS)
		OutputDebugStringA(messageBuffer);
	#elif FL_ENABLED(FL_PLATFORM_OSX)
		#if defined(__OBJC__)
			NSLog(@"%s", messageBuffer);
		#else
			NSLog(__CFStringMakeConstantString("%s"), messageBuffer);
		#endif 
	#elif FL_ENABLED(FL_PLATFORM_LINUX)
		fputs(messageBuffer, stdout);
		fflush(stdout);
	#else
		#error Unsuported Platform
	#endif
	}
}}
#endif
#include "Debug/Assert.h"
#include <cstdio>
#include <cstdlib>
#include <string.h>

namespace Flourish { namespace Debug
{
	namespace
	{
		bool ignoreAllAsserts = false;
		AssertHandler currentAssertHandler = nullptr;
	}

	void SetAssertHandler(AssertHandler handler)
	{
		currentAssertHandler = handler; 
	}

	AssertHandler GetAssertHandler()
	{
		return currentAssertHandler;
	}

	void SetIgnoreAllAsserts(bool value)
	{
		ignoreAllAsserts = value;
	}

	bool ShouldIgnoreAllAsserts()
	{
		return ignoreAllAsserts;
	}

	namespace Internal
	{
		AssertResult DefaultAssertHandler(
			SourceInfo sourceInfo,
			const char* condition,
			const char* message,
			const char* callstack)
		{
			DebugPrintf(
				"\n"
				"------------------------------\n"
				"       ASSERT TRIGGERED       \n"
				"------------------------------\n\n");

			if (message != nullptr && strlen(message) > 0)
			{
				DebugPrintf("Message:      \"%s\"\n\n", message);
			}

			DebugPrintf("File(Line):   %s(%d)\n", sourceInfo.FileName, sourceInfo.Line);
			DebugPrintf("Function:     %s\n\n", sourceInfo.FunctionName);

			DebugPrintf("Failed Cond:  \"%s\"\n", condition);

			if (callstack != nullptr && strlen(callstack) > 0)
			{
				DebugPrintf("\nStack Trace:\n%s", callstack);
			}

			DebugPrintf("\n");

			return AssertResult::Break;
		}

		bool HandleAssert(
			bool& ignoreLine,
			SourceInfo sourceInfo,
			const char* condition,
			const char* message, 
			...)
		{
			const size_t bufferSize = 1024; 
			char messageBuffer[bufferSize];

			if(message != nullptr)
			{
				va_list args;
				va_start(args, message);
				vsnprintf(messageBuffer, bufferSize, message, args);
				va_end(args);
			}

#if FL_ENABLED(FL_STACK_TRACE_ENABLED)
			const size_t callstackBufferSize = 2048; 
			char callstackBuffer[callstackBufferSize];

			//Skip the stack frame of this function so we get a call stack from the assert
			GetStackTrace(2, callstackBuffer, callstackBufferSize);
#else
			char* callstackBuffer = null;
#endif

			AssertHandler handler = (currentAssertHandler == nullptr ? DefaultAssertHandler : currentAssertHandler);

			auto assertResponce = handler(sourceInfo, condition, messageBuffer, callstackBuffer);

			bool shouldBreak = false;

			switch(assertResponce)
			{
			case AssertResult::IgnoreAlways: 
				ignoreLine = true;
				break;

			case AssertResult::IgnoreAll: 
				SetIgnoreAllAsserts(true);
				break;

			case AssertResult::Break:
				shouldBreak = true;
				break;

			case AssertResult::Abort: 
				abort();

			case AssertResult::Ignore: 
			default:
				//Do Nothing
				break;

			}

			return shouldBreak;
		}
	}
}}

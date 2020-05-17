#include "Platform/Platform.h"
#include "Debug/Debug.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Flourish { namespace Debug
{
#if FL_ENABLED(FL_DEBUG_PRINTF_ENABLED)
	void DebugPrintf(const char* message, ...)
	{
		va_list args;
		va_start(args, message);
		DebugPrintfVArgs(message, args);
		va_end(args);
	}
	
	void DebugPrintfVArgs(const char* message, va_list args)
	{
		va_list argsCopy;
		va_copy(argsCopy, args);

		const size_t bufferSize = 2048; 
		char messageBuffer[bufferSize];

		char* messageBufferPtr = messageBuffer;

		size_t numCharsWritten = vsnprintf(messageBufferPtr, bufferSize, message, args);

		char* allocBuffer = nullptr;

		if(numCharsWritten >= bufferSize)
		{
			//Unable to write the full message to the buffer, allocate a correct size buffer and try again
			allocBuffer = static_cast<char*>(malloc(sizeof(char) * (numCharsWritten + 1)));
			messageBufferPtr = allocBuffer;

			vsnprintf(messageBufferPtr, (numCharsWritten + 1), message, argsCopy);
			va_end(argsCopy);
		}

		fputs(messageBuffer, stdout);
		fflush(stdout);

		//Also output to any platform specific logs
		#if FL_ENABLED(FL_PLATFORM_WINDOWS)
			OutputDebugStringA(messageBuffer);
        #endif

		if(allocBuffer != nullptr)
		{
			free(allocBuffer);
		}
	}
	


#else
	void DebugPrintf(const char* message, ...)
	{
		FL_UNUSED(message);
	}

	namespace Internal
	{
		void DebugPrintfVargs(const char* message, va_list args)
		{
			FL_UNUSED(message);
			FL_UNUSED(args);
		}
	}
#endif


#if FL_ENABLED(FL_STACK_TRACE_ENABLED)

	#if FL_ENABLED(FL_PLATFORM_WINDOWS)
		//Only include when doing stacktraces so we are not forced to link dbghelp.lib when not using it
		#include "dbghelp.h"

		StackTraceResult GetStackTrace(int32_t numFramesToOmit, char* callstackBufOut, size_t callstackBufOutLength )
		{
			const auto maxNumFramesToCapture = 512;
			const auto maxSymbolNameSize = 256;

			auto process = GetCurrentProcess();

			SymInitialize( process, nullptr, TRUE );

			void* stackTraceFrames[ maxNumFramesToCapture ];
			int32_t numActualFramesCaptured = CaptureStackBackTrace( numFramesToOmit, maxNumFramesToCapture, stackTraceFrames, nullptr );

			if(numActualFramesCaptured == 0)
			{
				return StackTraceResult::Failed;
			}

			//TODO: replace with allocator to track?
			auto symbol = static_cast<SYMBOL_INFO * >(malloc( sizeof( SYMBOL_INFO ) + maxSymbolNameSize * sizeof( char )));
			symbol->MaxNameLen   = maxSymbolNameSize - 1; //-1 for null terminator
			symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

			callstackBufOut[0] = '\0';

			bool truncated = false;

			for(int32_t i = 0; i < numActualFramesCaptured; ++i )
			{
				SymFromAddr( process, reinterpret_cast<DWORD64 >(stackTraceFrames[ i ]), nullptr, symbol );

				int32_t stringLength = strlen(callstackBufOut);
				int32_t bufferLeftLength = callstackBufOutLength - stringLength;

				if(snprintf(callstackBufOut + stringLength, bufferLeftLength, "%i: (0x%08I64X) - %s()\n", numActualFramesCaptured - i - 1, symbol->Address, symbol->Name) >= bufferLeftLength)
				{
					truncated = true;
					break;
				}
			}

			free(symbol);

			return truncated ? StackTraceResult::OKTruncated : StackTraceResult::OK;
		}

	#elif FL_ENABLED(FL_PLATFORM_OSX) || FL_ENABLED(FL_PLATFORM_LINUX)
		#include <execinfo.h>
		#include <cxxabi.h>
        #include <inttypes.h>

		StackTraceResult GetStackTrace(int32_t numFramesToOmit, char* callstackBufOut, size_t callstackBufOutLength )
		{
			//Source: https://panthema.net/2008/0901-stacktrace-demangled/

			size_t maxNumFramesToCapture = 512;
			size_t maxSymbolNameSize = 256;

			// storage array for stack trace address data
			void* addrlist[maxNumFramesToCapture];

			// retrieve current stack addresses
			int32_t numFramesCaptured = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

			if (numFramesCaptured == 0) 
			{
				return StackTraceResult::Failed;
			}

			// resolve addresses into strings containing "filename(function+address)",
			// this array must be free()-ed
			char** symbollist = backtrace_symbols(addrlist, numFramesCaptured);

			// allocate string which will be filled with the demangled function name
			//TODO: replace with allocator to track? (Kinda hard as __cxa_demangle can call remalloc)
			auto funcname = static_cast<char*>(malloc(maxSymbolNameSize));

			int32_t stringLength;
			int32_t bufferLeftLength;
			callstackBufOut[0] = '\0';

			bool truncated = false;

			// iterate over the returned symbol lines. skip the first, it is the
			// address of this function.
			for (int32_t i = numFramesToOmit; i < numFramesCaptured; i++)
			{
				char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

                #if FL_ENABLED(FL_PLATFORM_OSX)
                    for ( char *p = symbollist[i]; *p; ++p )
                    {
                        if (( *p == '_' ) && ( *(p-1) == ' ' ))
                            begin_name = p-1;
                        else if ( *p == '+' )
                            begin_offset = p-1;
                        
                        end_offset = p;
                    }
                #else
                    // find parentheses and +address offset surrounding the mangled name:
                    // ./module(function+0x15c) [0x8048a6d]
                    for (char *p = symbollist[i]; *p; ++p)
                    {
                        if (*p == '(')
                            begin_name = p;
                        else if (*p == '+')
                            begin_offset = p;
                        else if (*p == ')' && begin_offset) {
                            end_offset = p;
                            break;
                        }
                    }
                #endif

				stringLength = strlen(callstackBufOut);
				bufferLeftLength = callstackBufOutLength - stringLength;

				if (begin_name && begin_offset && end_offset
					&& begin_name < begin_offset)
				{
					*begin_name++ = '\0';
					*begin_offset++ = '\0';
					*end_offset = '\0';

					// mangled name is now in [begin_name, begin_offset) and caller
					// offset in [begin_offset, end_offset). now apply
					// __cxa_demangle():

					int status;
					char* ret = abi::__cxa_demangle(begin_name,	funcname, &maxSymbolNameSize, &status);
					if (status == 0) 
					{
						//CB: __cxa_demangle may have reaallocated funcname buffer using realloc
						funcname = ret;
						if(snprintf(callstackBufOut + stringLength, bufferLeftLength, "%i: (0x%" PRIXPTR ") - %s::%s\n", numFramesCaptured - i - 1, (uintptr_t)addrlist[i], symbollist[i], funcname) >= bufferLeftLength)
						{
							truncated = true;
							break;
						}
					}
					else 
					{
						// demangling failed. Output function name as a C function with
						// no arguments.
						if(snprintf(callstackBufOut + stringLength, bufferLeftLength, "%i: (0x%" PRIXPTR ") - %s::%s\n", numFramesCaptured - i - 1, (uintptr_t)addrlist[i], symbollist[i], begin_name) >= bufferLeftLength)
						{
							truncated = true;
							break;
						}
					}
				}
				else
				{
					// couldn't parse the line? print the whole line.
					if(snprintf(callstackBufOut + stringLength, bufferLeftLength, "%i: (0x%" PRIXPTR ") - %s\n", numFramesCaptured - i - 1, (uintptr_t)addrlist[i], symbollist[i]) >= bufferLeftLength)
					{
						truncated = true;
						break;
					}
				}
			}

			free(funcname);
			free(symbollist);

			return truncated ? StackTraceResult::OKTruncated : StackTraceResult::OK;
		}
	#endif
#else
	StackTraceResult GetStackTrace(int32_t numFramesToOmit,	char* callstackBufOut, size_t callstackBufOutLength )
	{
		FL_UNUSED(numFramesToOmit);
		FL_UNUSED(callstackBufOut);
		FL_UNUSED(callstackBufOutLength);
		return StackTraceResult::Failed;
	}
#endif

}}

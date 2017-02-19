#pragma once

#include "Platform/PlatformFeatures.h"

namespace Flourish { namespace Debug
{
	//Simple wrapper around file/line/function macro data
	struct SourceInfo
	{
		const char* FileName;
		long Line;
		const char* FunctionName;
	};

	namespace Internal
	{
		FL_FORCE_INLINE SourceInfo CreateSourceInfoStruct(const char* fileName, long lineNumber, const char* functionName)
		{
			return {fileName, lineNumber, functionName};
		}
	}

	//This macro resolves into a struct SourceInfo that contains all the source data for debugging
	#define MAKE_SOURCE_INFO Flourish::Debug::Internal::CreateSourceInfoStruct(__FILE__,__LINE__, FL_FUNCTION)
}}

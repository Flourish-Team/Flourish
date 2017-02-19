#pragma once

#include "Platform/PlatformFeatures.h"

namespace Flourish { namespace Debug
{
	const

	//Simple wrapper around file/line/function macro data
	struct SourceInfo
	{
		char* FileName;
		long Line;
		char* FunctionName;
	};

	namespace Internal
	{
		FORCEINLINE SourceInfo CreateSourceInfoStruct(char* fileName, long lineNumber, char* functionName)
		{
			return {fileName, lineNumber, functionName};
		}
	}

	//This macro resolves into a struct SourceInfo that contains all the source data for debugging
	#define MAKE_SOURCE_INFO Flourish::Debug::Internal::CreateSourceInfoStruct(__FILE__,__LINE__, FL_FUNCTION)
}}
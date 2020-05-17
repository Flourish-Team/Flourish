#pragma once
#include <cstring>
#include <cstdio>
#include <cstdarg>

namespace Flourish::Utils::CharArrayUtils
{
	//Appends a string to the end of a char array using sprintf. buffer must already contain a string (or have it first character null terminated).
	//returns false if the string was truncated
	inline bool sprintfAppend(char* buffer, int bufferLength, const char* string, ...)
	{
		int currentStringLength = strlen(buffer);
		int bufferLeftLength = bufferLength - currentStringLength;

		va_list args;
		va_start(args, string);
		int charactersAdded = vsnprintf(buffer + currentStringLength, bufferLeftLength, string, args);
		va_end(args);

		return charactersAdded < bufferLeftLength;
	}
}
	
#pragma once

#include "FString.h"
#include <codecvt>

namespace Flourish
{
	namespace FStringUtils
	{
		using Utf8Utf16ConvertType = std::codecvt_utf8_utf16<wchar_t>;

		inline FWideString ConvertFStringToFWideString(const FString& string)
		{
			std::wstring_convert<Utf8Utf16ConvertType, wchar_t> stringConverted;
			return stringConverted.from_bytes(string.AsStdString());
		}

		inline FString ConvertFWideStringToFString(const FWideString& string)
		{
			std::wstring_convert<Utf8Utf16ConvertType, wchar_t> stringConverted;
			return stringConverted.to_bytes(string.AsStdString());
		}
	}
}
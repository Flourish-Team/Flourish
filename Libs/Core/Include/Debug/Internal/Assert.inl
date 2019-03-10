#pragma once
#include "Debug/Debug.h"

#if FL_ENABLED(FL_ASSERT_ENABLED)
	#define _IMPL_FL_ASSERT(condition, message, ...)					\
		FL_WARNINGS_PUSH												\
		FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT						\
		do																\
		{																\
			static bool ignoreThisAssert = false;						\
			if(	ignoreThisAssert == false &&							\
				Flourish::Debug::ShouldIgnoreAllAsserts() == false &&	\
				(condition) == false)									\
			{															\
				if(Flourish::Debug::Internal::HandleAssert(				\
					ignoreThisAssert,									\
					MAKE_SOURCE_INFO,									\
					FL_STRINGIFY(condition),							\
					message,											\
					##__VA_ARGS__))										\
					{													\
						FL_DEBUG_BREAK();								\
					}													\
			}															\
		} while(0);														\
		FL_WARNINGS_POP
#else
	#define _IMPL_FL_ASSERT(condition, message, ...)
#endif

#define _IMPL_FL_STATIC_ASSERT(condition, message) static_assert(condition, message)

namespace Flourish { namespace Debug { namespace Internal
{
	bool HandleAssert(
		bool& ignoreLine,
		SourceInfo sourceInfo,
		const char* condition,
		const char* message, 
		...);
}}}



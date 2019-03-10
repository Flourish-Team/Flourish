#pragma once
#include "../../../3rdParty/debugbreak/debugbreak.h"

#define _IMPL_FL_DEBUG_BREAK()						\
	debug_break()

#define _IMPL_FL_DEBUG_BREAK_COND(condition)		\
	FL_WARNINGS_PUSH								\
	FL_DISABLE_WARNING__COND_EXP_IS_CONSTANT		\
	do												\
	{												\
		if((condition))								\
		{											\
			FL_DEBUG_BREAK();						\
		}											\
	} while(0);										\
	FL_WARNINGS_POP
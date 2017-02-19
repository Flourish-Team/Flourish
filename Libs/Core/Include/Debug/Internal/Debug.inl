#pragma once
#include "../../../3rdParty/debugbreak/debugbreak.h"

#define _IMPL_FL_DEBUG_BREAK()						\
	debug_break()

#define _IMPL_FL_DEBUG_BREAK_COND(condition)		\
	FL_PUSH_DISABLE_COND_EXP_IS_CONSTANT_WARNING	\
	do												\
	{												\
		if((condition))								\
		{											\
			FL_DEBUG_BREAK();						\
		}											\
	} while(0);										\
	FL_POP_DISABLE_COND_EXP_IS_CONSTANT_WARNING
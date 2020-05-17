#pragma once

#include "Platform/Platform.h"

namespace Flourish::Memory::Testing 
{
	//Class to allow recording if constructors and destructors arae been called correctly
	struct CtorDtorCallStats
	{
		CtorDtorCallStats()
			: NumCtorCalls(0)
			, NumDtorCalls(0)
		{
			
		}
		int32_t NumCtorCalls;
		int32_t NumDtorCalls;
	};

	FL_WARNINGS_PUSH
	FL_DISABLE_WARNING__STRUCTURE_WAS_PADDED_DUE_TO_ALIGNMENT
		//Aligned test data
		FL_ALIGNED_STRUCT(32, TestClass)
		{
			explicit TestClass(CtorDtorCallStats& stats)
				: mCallStats(stats)
				, Data(0)
			{
				mCallStats.NumCtorCalls++;
			}

			~TestClass()
			{
				mCallStats.NumDtorCalls++;
			}

			DISALLOW_COPY_AND_MOVE(TestClass);

			uint32_t Data;

		private:
			CtorDtorCallStats& mCallStats;
		};
	FL_WARNINGS_POP
}
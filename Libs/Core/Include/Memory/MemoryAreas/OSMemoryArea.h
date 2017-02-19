#pragma once

#include "../MemoryArea.h"
#include "../VirtualMemory.h"

namespace Flourish { namespace Memory
{
	// OS Memory Area
	//
	// Providesa memory area allocated by the OS's virtual memory functions.
	class OSMemoryArea : MemoryArea
	{
	public:
		explicit OSMemoryArea(size_t size)
			: MemoryArea(nullptr, 0)
		{
			mVAllocResult = VAlloc(size);

			mData = mVAllocResult.data;
			mSize = mVAllocResult.size;
		}

		virtual ~OSMemoryArea()
		{
			VFree(mVAllocResult);
		}

		//No Copying Of Moving of Memory Areas
		OSMemoryArea(OSMemoryArea& other) = delete;
		OSMemoryArea(OSMemoryArea&& other) = delete;
		OSMemoryArea& operator=(OSMemoryArea&&) = delete;
		OSMemoryArea& operator=(const OSMemoryArea&) = delete;

	private:
		VAllocResult mVAllocResult;
	};
}}
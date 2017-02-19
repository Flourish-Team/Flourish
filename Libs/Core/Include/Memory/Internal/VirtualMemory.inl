#pragma once

#include "Platform/Platform.h"

namespace Flourish { namespace Memory
{
#if FL_ENABLED(FL_PLATFORM_WINDOWS)
	inline VAllocResult VAlloc(size_t size)
	{
		VAllocResult result;

		auto pageSize = GetVPageSize();
		auto roundedSize = size + pageSize - (size % pageSize);

		result.data = VirtualAlloc(nullptr, roundedSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		result.size = roundedSize;

		return result;
	}

	inline void VFree(VAllocResult& virtualAlloc)
	{
		VirtualFree(virtualAlloc.data, 0, MEM_RELEASE);
	}

	inline size_t GetVPageSize()
	{
		static size_t minAllocationSize = 0;
		if (minAllocationSize == 0) 
		{
			SYSTEM_INFO system_info;
			GetSystemInfo(&system_info);
			minAllocationSize = system_info.dwPageSize;
		}
		return minAllocationSize;
	}
#elif FL_ENABLED(FL_PLATFORM_LINUX) || FL_ENABLED(FL_PLATFORM_LINUX)
	inline VAllocResult VAlloc(size_t size)
	{
		VAllocResult result;

		auto pageSize = GetVSize();
		auto roundedSize = size + pageSize - (size % pageSize);

		auto alloc = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

		result.data = alloc == MAP_FAILED ? nullptr : alloc;
		result.size = roundedSize;

		return result;
	}

	inline void VFree(VAllocResult& virtualAlloc)
	{
		munmap(virtualAlloc.data, virtualAlloc.size);
	}

	inline size_t GetVPageSize()
	{
		static size_t pageSize = 0;
		if (!pagesize)
		{
			pagesize = sysconf(_SC_PAGE_SIZE);
		}
		return pageSize;
	}
#endif
}}

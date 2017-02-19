#pragma once

namespace Flourish { namespace Memory
{
	struct VAllocResult
	{
		void* data;
		size_t size;
	};

	// TODO: Expose more functionalty (e.g locking pages for debugging purposes)

	// Allocates Virtual memory from the OS. The size of the allocation will be rounded up to
	// the nearest GetVirtualPageSize() multiple as so may be bigger then requested
	VAllocResult VAlloc(size_t size);

	// Releases Memory allocated by VirtualAlloc back to the system
	void VFree(VAllocResult& virtualAlloc);

	// Gets the page size for the OS. All allocations will be rounded up to a multiple of this size
	size_t GetVPageSize();
}}

//Include inline impimentation
#include "Internal/VirtualMemory.inl"
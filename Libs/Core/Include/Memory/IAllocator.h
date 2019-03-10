#pragma once
#include "Debug/SourceInfo.h"


namespace Flourish { namespace Memory
{
	struct MemoryNewDelInternal;

	// Allocators
	//
	// All Memory used by the application should go through allocators for correct tracking
	// This Base Allocator class to allow easy passing around of allocators without caring how they work
	class IAllocator
	{
	public:
		IAllocator() = default;
        virtual ~IAllocator() = default;

		DISALLOW_COPY_AND_MOVE(IAllocator);

		//Returns the name of the allocator for debugging purposes
		virtual const char* GetAllocatorName() = 0;
		
		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		virtual void* Alloc(size_t size, const Debug::SourceInfo& sourceInfo) = 0;

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		virtual void Free(void* ptr) = 0;

		//Given a pointer, calculates the size of the allocation.
		virtual size_t GetAllocationSize(void* ptr) = 0;

		//Given a pointer, calculates the size of any internal metadata that was needed for this allocation
		virtual size_t GetMetaDataAllocationSize(void* ptr) = 0;
	};
}}

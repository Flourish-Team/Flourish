#pragma once
#include "Debug/SourceInfo.h"


namespace Flourish::Memory
{
	struct MemoryNewDelInternal;

	// Allocators
	//
	// All Memory used by the application should go through allocators for correct tracking
	// This Base Allocator class to allow easy passing around of allocators without caring how they work
	// 
	// It is advise to use the memory macros in Memory.h then use the allocator directly to correctly handle
	// things like translating raw pointer for alignment or extra array data
	class IAllocator
	{
	public:
		IAllocator(const char* name)
		{
			strcpy_s(_allocatorName, name);
		}
		
        virtual ~IAllocator() = default;

		//Allocators can be moved, but not copied
		IAllocator(IAllocator&& other) noexcept = default;
		IAllocator& operator=(IAllocator&& other) noexcept = default;
		
		DISALLOW_COPY(IAllocator);

		//Returns the name of the allocator for debugging purposes
		virtual const char* GetAllocatorName() { return _allocatorName; }
		
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

		static const size_t ALLOCATOR_NAME_BUFFER_SIZE = 256;

	protected:
		char _allocatorName[ALLOCATOR_NAME_BUFFER_SIZE] { };
	};
}

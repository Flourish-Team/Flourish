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

        //No Copying of allocators
		IAllocator(IAllocator& other) = delete;
		IAllocator& operator=(IAllocator&&) = delete;
		
		//Suport moving of allocators
		IAllocator(IAllocator&& other) = default;
		IAllocator& operator=(const IAllocator&) = default;

		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		virtual void* Alloc(size_t size, size_t alignment, const Debug::SourceInfo& sourceInfo) = 0;

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		virtual void Free(void* ptr, const Debug::SourceInfo& sourceInfo) = 0;

		//Given a pointer, calculates the size of the allocation. This size may be different
		//to the requests size due to internal functionality of the allocator
		virtual size_t GetAllocationSize(void* ptr) = 0;
	};
}}

#pragma once
#include <cstdlib>
#include "Macro/MacroUtils.h"
#include "Memory/Allocator.h"
#include "Memory/AddressUtils.h"

namespace Flourish { namespace Memory {

	// Simmple Allocator implmentation that wraps around malloc/free
	class MallocAllocator : public Allocator
	{
	public:
		MallocAllocator() {}
		virtual ~MallocAllocator() {}

		//No Copying Of Moving of allocators
		MallocAllocator(MallocAllocator& other) = delete;
		MallocAllocator(MallocAllocator&& other) = delete;
		MallocAllocator& operator=(MallocAllocator&&) = delete;
		MallocAllocator& operator=(const MallocAllocator&) = delete;


		//Stored before the pointer of each allocation for quick free/GetAllocationSize calls
		struct AllocationHeader
		{
			size_t allocationSize;
			void* basePtr;
		};

		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		void* Alloc(size_t size, size_t alignment, const Debug::SourceInfo& sourceInfo) override
		{
			FL_UNUSED(sourceInfo);

			auto spaceNeeded = size + sizeof(AllocationHeader) + (alignment - 1);
			auto baseMemPtr = malloc(spaceNeeded);

			auto alignedPtr = AddressUtils::AlignAddress(baseMemPtr, alignment, sizeof(AllocationHeader));

			auto headerData = GetHeaderForPointer(alignedPtr);

			headerData->allocationSize = spaceNeeded;
			headerData->basePtr = baseMemPtr;

			return alignedPtr;
		}

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		void Free(void* ptr, const Debug::SourceInfo& sourceInfo) override
		{
			FL_UNUSED(sourceInfo);

			free(GetHeaderForPointer(ptr)->basePtr);
		}

		//Given a pointer, calculates the size of the allocation. This size may be diffrent
		//to the requests size due to internal functionality of the allocator
		size_t GetAllocationSize(void* ptr) override
		{
			return GetHeaderForPointer(ptr)->allocationSize;
		}

		static AllocationHeader* GetHeaderForPointer(void* ptr)
		{
			return reinterpret_cast<AllocationHeader*>(AddressUtils::AddressAddOffset(ptr, sizeof(AllocationHeader)));
		}
	};
}}

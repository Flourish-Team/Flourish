#pragma once
#include <cstdlib>
#include "Macro/MacroUtils.h"
#include "Memory/IAllocator.h"
#include "Memory/AddressUtils.h"

namespace Flourish::Memory {

	// Simple Allocator implementation that wraps around malloc/free
	class MallocAllocator : public IAllocator
	{
	public:
		explicit MallocAllocator(const char* allocatorName)
		{
			const char* nameString = "%s [MallocAllocator]";

			size_t nameBufferSize = snprintf(NULL, 0, nameString, allocatorName);
			mAllocatorName = static_cast<char*>(malloc(nameBufferSize + 1));

			snprintf(mAllocatorName, nameBufferSize + 1, nameString, allocatorName);
		}

		virtual ~MallocAllocator() = default;

		DISALLOW_COPY_AND_MOVE(MallocAllocator);

		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		void* Alloc(size_t size, const Debug::SourceInfo& sourceInfo) override
		{
			FL_UNUSED(sourceInfo);

			//Allocate and extra size_t worth of memory so we can store the allocation size
			size_t spaceNeeded = size + sizeof(size_t);
			void* baseMemPtr = malloc(spaceNeeded);

			//Store the size and then offset forward and return the user pointer
			size_t* allocSize = static_cast<size_t *>(baseMemPtr);
			(*allocSize) = size;

			return AddressUtils::AddressAddOffset(baseMemPtr, sizeof(size_t));
		}

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		void Free(void* ptr) override
		{
			free(AddressUtils::AddressSubOffset(ptr, sizeof(size_t)));
		}

		//Given a pointer, calculates the size of the allocation. if includeInternalMetadataSize is true
		//then the returned size will include the size of any extra bytes allocated for internal metadata
		size_t GetAllocationSize(void* ptr) override
		{
			size_t* allocSize = static_cast<size_t *>(AddressUtils::AddressSubOffset(ptr, sizeof(size_t)));
			return  (*allocSize);
		}

		//Given a pointer, calculates the size of any internal metadata that was needed for this allocation
		size_t GetMetaDataAllocationSize(void* ptr) override
		{
			FL_UNUSED(ptr);
			return sizeof(size_t);
		}

		const char* GetAllocatorName() override
		{
			return mAllocatorName;
		}


	private:
		char* mAllocatorName;
	};
}

#pragma once

#include <stdlib.h> 
#include "Allocator.h"

namespace Flourish { namespace Memory
{
	//Base for all allocators that supplies numerious policies for easy debugging and multithreading use
	template <
		class ThreadingPolicy,			//How thread safety in the allocator should be handled
		class MemoryTrackingPolicy,		//How the allocator should track memory (normally for debuging purposes)
		class BoundsCheckingPolicy,		//How the allocator should perform bounds checking
		class MemoryMarkingPolicy>		//How memory in the allocator should be marked with bit paterns for debugging purposes)
	class AllocatorBase : public Allocator
	{
	public:
		//Uses 'parentAllocator' to allocate the fixed size buffer 'size' to be used for this allocator
		AllocatorBase(const char* allocatorName)
			: mMemoryTrackingPolicyImpl(allocatorName)
		{
			mMemoryMarkingPolicyImpl.MarkInitialMemory(mMemory, mMemory + mMemorySize);
		}

		virtual ~AllocatorBase()
		{
			mMemoryMarkingPolicyImpl.MarkFinishedMemory(mMemory, mMemory + mMemorySize);
		}

		//No Copying Of Moving of allocators
		AllocatorBase(AllocatorBase& other) = delete;
		AllocatorBase(AllocatorBase&& other) = delete;
		AllocatorBase& operator=(AllocatorBase&&) = delete;
		AllocatorBase& operator=(const AllocatorBase&) = delete;

	protected:
		virtual void* AllocateImpl(size_t size, size_t alignment, size_t frontBoundCheckGuardSize) = 0;
		virtual void FreeImpl(void* ptr) = 0;

		void* Allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) final
		{
			mThreadPolicyImpl.Enter();

			const size_t originalSize = size;
			const size_t newSize = size + BoundsCheckingPolicy::FRONT_GUARD_SIZE + BoundsCheckingPolicy::BACK_GUARD_SIZE;
			
			char* allocatedMem = static_cast<char*>(AllocateImpl(newSize, alignment, BoundsCheckingPolicy::FRONT_GUARD_SIZE));
			char* realMemoryStart = allocatedMem + BoundsCheckingPolicy::FRONT_GUARD_SIZE;

			mBoundsCheckingPolicyImpl.SetFrontGuard(allocatedMem);
			mMemoryMarkingPolicyImpl.MarkAllocatedMemory(realMemoryStart, originalSize);
			mBoundsCheckingPolicyImpl.SetBackGuard(realMemoryStart + originalSize);

			mMemoryTrackingPolicyImpl.OnAllocation(allocatedMem, newSize, alignment, 
				realMemoryStart, originalSize, sourceInfo);

			mThreadPolicyImpl.Leave();

			return realMemoryStart;
		}

		void Free(void* ptr, const SourceInfo& sourceInfo) final
		{
			mThreadPolicyImpl.Enter();

			char* realMemory = static_cast<char*>(ptr);
			char* allocatedMemory = realMemory - BoundsCheckingPolicy::FRONT_GUARD_SIZE;
			const size_t allocationSize = GetAllocationSize(allocatedMemory);
			const size_t realAllocationSize = allocationSize - (BoundsCheckingPolicy::FRONT_GUARD_SIZE + BoundsCheckingPolicy::BACK_GUARD_SIZE);
			
			mBoundsCheckingPolicyImpl.CheckFrontGuard(allocatedMemory);
			mBoundsCheckingPolicyImpl.CheckBack(realMemory + realAllocationSize);

			mMemoryTrackingPolicyImpl.OnDeallocation(allocatedMemory, allocationSize, realMemory, realAllocationSize, sourceInfo);

			mMemoryMarkingPolicyImpl.MarkDeallocatedMemory(allocatedMemory, allocationSize);
			
			FreeImpl(allocatedMemory);

			mThreadPolicyImpl.Leave();
		}



		void BeforeFree()
		{
			
		}

		ThreadingPolicy mThreadPolicyImpl;
		MemoryTrackingPolicy mMemoryTrackingPolicyImpl;
		BoundsCheckingPolicy mBoundsCheckingPolicyImpl;
		MemoryMarkingPolicy mMemoryMarkingPolicyImpl;
	};
}}
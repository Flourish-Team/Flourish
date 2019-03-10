#pragma once

#include "Test.h"
#include "Platform/Platform.h"
#include "Memory/IAllocator.h"

namespace Flourish::Memory::Testing 
{
	// Allocator mock we can use to test memory functions
	template<int BufferSize>
	class TestAllocator : public IAllocator
	{
	public:
		static const unsigned int FRONT_GUARD = 0x02468ACE;
		static const unsigned int BACK_GUARD =  0x13579BDF;

		TestAllocator()
			: mAllocSize(0)
			, mDidAlloc(false)
			, mDidFree(false)
			, mNextAllocator(nullptr)
		{
		}

		virtual ~TestAllocator()
		{
			if(mNextAllocator != nullptr)
			{
				delete mNextAllocator;
			}
		}

		DISALLOW_COPY_AND_MOVE(TestAllocator);

		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		void* Alloc(size_t size, const Debug::SourceInfo& sourceInfo) override
		{
			FL_UNUSED(sourceInfo);
			FL_ASSERT(size <= BufferSize);

			//CB: the test allocator can only manage a single allocation, so if we need more (e.g shared ptr internal data) we passed it onto a new
			//copy of the allocator
			if(mDidAlloc)
			{
				mNextAllocator = new TestAllocator<BufferSize>();
				return mNextAllocator->Alloc(size, sourceInfo);
			}

			mAllocSize = size;
			mDidAlloc = true;

			(*reinterpret_cast<unsigned int*>(mBuffer)) = FRONT_GUARD;
			(*reinterpret_cast<unsigned int*>(mBuffer + mAllocSize + sizeof(FRONT_GUARD))) = BACK_GUARD;

			void* userPtr = (mBuffer + sizeof(FRONT_GUARD));
			memset(userPtr, 0, size);

			return userPtr;
		}

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		void Free(void* ptr) override
		{
			ASSERT_EQUAL(mDidAlloc, true);

			//CB: If the free ptr is not correct, might be part of a linked allocator, so pass it down the chain until it passed or fails
			if(mNextAllocator != nullptr)
			{
				if(ptr != (mBuffer + sizeof(FRONT_GUARD)))
				{
					mNextAllocator->Free(ptr);
				}
			}
			else
			{
				ASSERT_EQUAL(ptr, mBuffer + sizeof(FRONT_GUARD));
			}

			mDidFree = true;
		}

		//Given a pointer, calculates the size of the allocation.
		size_t GetAllocationSize(void* ptr) override
		{
			if(mNextAllocator != nullptr)
			{
				if(ptr != (mBuffer + sizeof(FRONT_GUARD)))
				{
					return mNextAllocator->GetAllocationSize(ptr);
				}
			}
			else
			{
				FL_ASSERT_MSG(ptr == mBuffer + sizeof(FRONT_GUARD), "Incorrect ptr");
			}

			return mAllocSize;
		}

		//Given a pointer, calculates the size of any internal metadata that was needed for this allocation
		size_t GetMetaDataAllocationSize(void* ptr) override
		{
			FL_UNUSED(ptr);
			return 0;
		}

		const char* GetAllocatorName() override
		{
			return "Test Allocator";
		}

		void* GetBuffer()
		{
			return &mBuffer;
		}

		void GTest_ValidateAllocator()
		{
			//make sure we did the alloc and free
			ASSERT_EQUAL(mDidAlloc, true);
			ASSERT_EQUAL(mDidFree, true);

			//Check the guards are intact
			ASSERT_EQUAL((*reinterpret_cast<unsigned int*>(mBuffer)), FRONT_GUARD);
			ASSERT_EQUAL((*reinterpret_cast<unsigned int*>(mBuffer + mAllocSize + sizeof(FRONT_GUARD))), BACK_GUARD);

			if(mNextAllocator != nullptr)
			{
				mNextAllocator->GTest_ValidateAllocator();
			}
		}

	private:
		char mBuffer[BufferSize + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD)];
		size_t mAllocSize;
		bool mDidAlloc;
		bool mDidFree;
		TestAllocator* mNextAllocator;
	};
}
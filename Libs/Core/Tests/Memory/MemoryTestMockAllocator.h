#pragma once

#include "Test.h"
#include "Platform/Platform.h"
#include "Memory/IAllocator.h"

namespace Flourish::Memory::Testing 
{
	// Allocator mock we can use to test memory functions
	template<int32_t BufferSize>
	class TestAllocator : public IAllocator
	{
	public:
		static const uint32_t FRONT_GUARD = 0x02468ACE;
		static const uint32_t BACK_GUARD =  0x13579BDF;

		static const size_t FAKE_METADATA_SIZE = 24;

		TestAllocator() : IAllocator("Test Allocator")	{ }

		virtual ~TestAllocator()
		{
			if(_nextAllocator != nullptr)
			{
				delete _nextAllocator;
			}
		}

		DISALLOW_COPY(TestAllocator);

		TestAllocator(TestAllocator&& other) noexcept = default;
		TestAllocator& operator=(TestAllocator&& other) noexcept = default;
		
		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		void* Alloc(size_t size, const Debug::SourceInfo& sourceInfo) override
		{
			FL_UNUSED(sourceInfo);
			FL_ASSERT(size <= BufferSize);

			//CB: the test allocator can only manage a single allocation, so if we need more (e.g shared ptr internal data) we passed it onto a new
			//copy of the allocator
			if(_didAlloc)
			{
				_nextAllocator = new TestAllocator<BufferSize>();
				return _nextAllocator->Alloc(size, sourceInfo);
			}

			_allocSize = size;
			_didAlloc = true;

			(*reinterpret_cast<uint32_t*>(_buffer)) = FRONT_GUARD;
			(*reinterpret_cast<uint32_t*>(_buffer + _allocSize + sizeof(FRONT_GUARD))) = BACK_GUARD;

			_userPtr = (_buffer + sizeof(FRONT_GUARD));
			memset(_userPtr, 0, size);
			
			return _userPtr;
		}

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		void Free(void* ptr) override
		{
			ASSERT_EQUAL(_didAlloc, true);

			//CB: If the free ptr is not correct, might be part of a linked allocator, so pass it down the chain until it passed or fails
			if(_nextAllocator != nullptr)
			{
				if(ptr != (_buffer + sizeof(FRONT_GUARD)))
				{
					_nextAllocator->Free(ptr);
				}
			}
			else
			{
				ASSERT_EQUAL(ptr, _buffer + sizeof(FRONT_GUARD));
			}

			_didFree = true;
		}

		//Given a pointer, calculates the size of the allocation.
		size_t GetAllocationSize(void* ptr) override
		{
			if(_nextAllocator != nullptr)
			{
				if(ptr != (_buffer + sizeof(FRONT_GUARD)))
				{
					return _nextAllocator->GetAllocationSize(ptr);
				}
			}
			else
			{
				FL_ASSERT_MSG(ptr == _buffer + sizeof(FRONT_GUARD), "Incorrect ptr");
			}

			return _allocSize;
		}

		//Given a pointer, calculates the size of any internal metadata that was needed for this allocation
		size_t GetMetaDataAllocationSize(void* ptr) override
		{
			//Return a fake size for testing
			FL_UNUSED(ptr);
			return FAKE_METADATA_SIZE;
		}

		void* GetBuffer()
		{
			return &_buffer;
		}

		void* GetUserPtr()
		{
			return _userPtr;
		}

		void GTest_ValidateAllocator()
		{
			//make sure we did the alloc and free
			ASSERT_EQUAL(_didAlloc, true);
			ASSERT_EQUAL(_didFree, true);

			//Check the guards are intact
			ASSERT_EQUAL((*reinterpret_cast<uint32_t*>(_buffer)), FRONT_GUARD);
			ASSERT_EQUAL((*reinterpret_cast<uint32_t*>(_buffer + _allocSize + sizeof(FRONT_GUARD))), BACK_GUARD);

			if(_nextAllocator != nullptr)
			{
				_nextAllocator->GTest_ValidateAllocator();
			}
		}

	private:
		char _buffer[BufferSize + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD)] { };
		size_t _allocSize {0};
		bool _didAlloc {false};
		bool _didFree {false};
		void* _userPtr;
		TestAllocator* _nextAllocator {nullptr};
	};
}
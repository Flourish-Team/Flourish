#pragma once

namespace Flourish { namespace Memory
{
	// Memory Area
	//
	// Memory Areas control where something gets its memory from (e.g. static array, stack, heap, Console specific areas (like MEM1 and MEM2 on Wii) etc).
	// Note that the memory area should have full ownership of the memory for the lifetime of the MemoryArea object
	class MemoryArea
	{
	public:
		MemoryArea(void* data, size_t size)
			: mData(data)
			, mSize(size)
		{
			
		}

		virtual ~MemoryArea() {}

		//No Copying Of Moving of Memory Areas
		MemoryArea(MemoryArea& other) = delete;
		MemoryArea(MemoryArea&& other) = delete;
		MemoryArea& operator=(MemoryArea&&) = delete;
		MemoryArea& operator=(const MemoryArea&) = delete;

		//Returns the size of this memory Area
		size_t GetSize() const { return mSize; }
		void* GetData() const { return mData; }

	protected:
		void* mData;
		size_t mSize;
	};
}}
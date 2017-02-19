#pragma once

#include "../MemoryArea.h"

namespace Flourish { namespace Memory
{
	// Static Memory Area
	//
	// Provides memory from a buffer defined staticly at compile time
	template<size_t BUFFER_SIZE>
	class StaticMemoryArea : MemoryArea
	{
	public:
		StaticMemoryArea()
			: MemoryArea(reinterpret_cast<void*>(mStaticMemory), BUFFER_SIZE)
		{
			
		}

		virtual ~StaticMemoryArea() {}

		//No Copying Of Moving of Memory Areas
		StaticMemoryArea(StaticMemoryArea& other) = delete;
		StaticMemoryArea(StaticMemoryArea&& other) = delete;
		StaticMemoryArea& operator=(StaticMemoryArea&&) = delete;
		StaticMemoryArea& operator=(const StaticMemoryArea&) = delete;

	private:
		char mStaticMemory[BUFFER_SIZE];
	};
}}
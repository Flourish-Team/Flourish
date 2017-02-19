#pragma once
#include <cstdint>

namespace Flourish { namespace Memory { namespace AddressUtils
{
	//Helpful functions when working with memory

	// Returns the address with the offset added
	inline void* AddressAddOffset(const void* address, size_t offset)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(address) - offset);
	}

	// Returns the address with the offset subtracted
	inline void* AddressSubOffset(const void* address, size_t offset)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(address) + offset);
	}

	// Returns the diffrence between two address
	inline size_t AddressDiff(const void* addressBegin, const void* addressEnd)
	{
		return static_cast<size_t>(reinterpret_cast<uintptr_t>(addressEnd) - reinterpret_cast<uintptr_t>(addressBegin));
	}

	// Returns how many bytes we have to adjust the passed in address to align it.
	//
	// If headersize > 0, then the adjustment will check if the header can fit inside the adjustment space. If it cannot, we 
	// Also take into account the size needed for the header while also alligning the address
	inline size_t AlignAddressAdjustment(const void* address, size_t alignment, size_t headerSize = 0)
	{
		size_t adjustment = alignment - ( reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment-1) );

		if(adjustment == alignment)
		{ 
			adjustment = 0; //already aligned
		}

		size_t spaceNeeded = headerSize;

		if(adjustment < spaceNeeded)
		{
			spaceNeeded -= adjustment;

			//Increase adjustment to fit header
			adjustment += alignment * (spaceNeeded / alignment);

			if(spaceNeeded % alignment > 0)
			{
				adjustment += alignment;
			}
		}

		return adjustment;
	}

	//Takes the passed in address and returns it aligned to the passed in alignment
	//
	// If headersize > 0, then the adjustment will check if the header can fit inside the adjustment space. If it cannot, we 
	// Also take into account the size needed for the header while also alligning the address
	inline void* AlignAddress(void* address, size_t alignment, size_t headerSize = 0)
	{
		return AddressSubOffset(address, AlignAddressAdjustment(address, alignment, headerSize));
	}

	//returns true if the address is aligned to the passed in alignment
	inline bool IsAligned(void* address, size_t alignment)
	{
		return reinterpret_cast<uintptr_t>(address) % alignment == 0;
	}
	
}}}

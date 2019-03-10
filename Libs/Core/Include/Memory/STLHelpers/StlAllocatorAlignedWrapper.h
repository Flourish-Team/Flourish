#pragma once
#include "Memory/Memory.h"
#include <type_traits>

namespace Flourish { namespace Memory
{
	//This wrappers allows the use of Flourish allocators where std:allocators are used
	template <typename T>
	class StlAllocatorAlignedWrapper
	{
	public:
		using value_type = T;
		
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;

		explicit StlAllocatorAlignedWrapper(IAllocator& allocator, size_t alignment) 
			: mWrappedAllocator(allocator)
			, mAlignment(alignment)
		{
		}

		value_type* allocate(std::size_t n)
		{
			return FL_NEW_RAW_BUFFER_ALIGN(mWrappedAllocator, n, mAlignment);
		}

		void deallocate(value_type* p, std::size_t n)
		{
            FL_UNUSED(n);
			FL_DELETE_RAW_BUFFER_ALIGN(mWrappedAllocator, p);
		}

		IAllocator& GetWrappedAllocator() const
		{
			return mWrappedAllocator;
		}

		StlAllocatorWrapper<T> select_on_container_copy_construction() const
		{
			return *this;
		}

	private: 
		template <typename T1, typename T2>
		friend bool operator==(const StlAllocatorAlignedWrapper<T1>&, const StlAllocatorAlignedWrapper<T2>&);

		template <typename T1, typename T2>
		friend bool operator!=(const StlAllocatorAlignedWrapper<T1>&, const StlAllocatorAlignedWrapper<T2>&);

		IAllocator& mWrappedAllocator;
		size_t mAlignment;
	};

	template <typename T, typename U>
	bool operator==(const StlAllocatorAlignedWrapper<T>& lhs, const StlAllocatorAlignedWrapper<U>& rhs)
	{
		//On equal if same allocator, check memory address
		return &lhs.mWrappedAllocator == &rhs.mWrappedAllocator;
	}

	template <typename T, typename U>
	bool operator!=(const StlAllocatorAlignedWrapper<T>& lhs, const StlAllocatorAlignedWrapper<U>& rhs)
	{
		return !(lhs == rhs);
	}
}}

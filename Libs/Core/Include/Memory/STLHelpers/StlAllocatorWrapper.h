#pragma once
#include "Memory/Memory.h"
#include <type_traits>

namespace Flourish { namespace Memory
{
	//This wrappers allows the use of Flourish allocators where std:allocators are used
	template <typename T, size_t ALIGN = alignof(T)>
	class StlAllocatorWrapper
	{
	public:
		using value_type      = T;
		
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;

		template <typename U, size_t U_ALIGN = alignof(U)>
		struct rebind
		{
			using other = StlAllocatorWrapper<U, U_ALIGN>;
		};

		explicit StlAllocatorWrapper(IAllocator& allocator) 
			: mWrappedAllocator(allocator)
		{
		}

		template <typename U, size_t U_ALIGN = alignof(U)>
		StlAllocatorWrapper(const StlAllocatorWrapper<U, U_ALIGN>& other) 
			: mWrappedAllocator(other.mWrappedAllocator)
		{
		}

		value_type* allocate(std::size_t n)
		{
			return FL_NEW_RAW_BUFFER_ALIGN(mWrappedAllocator, n, ALIGN);
		}

		void deallocate(value_type* p, std::size_t n)
		{
            FL_UNUSED(n);
			FL_DELETE_RAW_BUFFER(mWrappedAllocator, p);
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
		friend bool operator==(const StlAllocatorWrapper<T1>&, const StlAllocatorWrapper<T2>&);

		template <typename T1, typename T2>
		friend bool operator!=(const StlAllocatorWrapper<T1>&, const StlAllocatorWrapper<T2>&);

		IAllocator& mWrappedAllocator;
	};

	template <typename T, typename U>
	bool operator==(const StlAllocatorWrapper<T>& lhs, const StlAllocatorWrapper<U>& rhs)
	{
		return &lhs.mWrappedAllocator == &rhs.mWrappedAllocator;
	}

	template <typename T, typename U>
	bool operator!=(const StlAllocatorWrapper<T>& lhs, const StlAllocatorWrapper<U>& rhs)
	{
		return !(lhs == rhs);
	}
}}

#pragma once
#include <stdlib.h> 
#include <type_traits>
#include "Memory/Memory.h"


namespace Flourish { namespace Memory
{
	//This wrappers allows the use of Flourish allocators where std:allocators are used
	template <typename T, size_t ALIGN = alignof(T)>
	class StlAllocatorWrapper
	{
	public:
		using value_type      = T;
		using pointer         = T*;
		using const_pointer   = const T*;
		using reference       = T&;
		using const_reference = const T&;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;

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

		IAllocator& GetWrappedAllocator() const
		{
			return mWrappedAllocator;
		}

		StlAllocatorWrapper<T> select_on_container_copy_construction() const
		{
			return *this;
		}

		pointer allocate(size_type n, void* = nullptr)
		{
			return FL_NEW_RAW_BUFFER_ALIGN(mWrappedAllocator, n, ALIGN);
		}

		void deallocate(pointer p, size_type n)
		{
            FL_UNUSED(n);
			FL_DELETE_RAW_BUFFER(mWrappedAllocator, p);
		}

		template <typename U, typename... Args>
		void construct(U* p, Args&&... args)
		{
			void* mem = p;
			new (mem) U(std::forward<Args>(args)...);
		}

		template <typename U>
		void destroy(U* p)
		{
			p->~U();
		}

		size_type max_size() const
		{
			return std::numeric_limits<size_type>::max() / sizeof(value_type);
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

#pragma once
#include <type_traits>

namespace Flourish { namespace Memory
{
	//This wrappers allows the use of Flourish allocators where std:allocators are used
	template <typename T>
	class StlAllocatorWrapper
	{
	public:
		using value_type = T;
		
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;

		explicit StlAllocatorWrapper(IAllocator& allocator) 
			: mWrappedAllocator(allocator)
		{
		}

		template <class U> 
		StlAllocatorWrapper(StlAllocatorWrapper<U> const& other)
			: mWrappedAllocator(other.GetWrappedAllocator())
		{
			
		}

		value_type* allocate(std::size_t n)
		{
			//CB: Have to use allocator directly due to circular dependencies
			return static_cast<value_type*>(mWrappedAllocator.Alloc(n * sizeof(value_type), MAKE_SOURCE_INFO));
		}

		void deallocate(value_type* p, std::size_t n)
		{
            FL_UNUSED(n);
			//CB: Have to use allocator directly due to circular dependencies
			mWrappedAllocator.Free(p);
		}

		IAllocator& GetWrappedAllocator() const
		{
			return mWrappedAllocator;
		}

		StlAllocatorWrapper<T> select_on_container_copy_construction() const
		{
			return *this;
		}

		IAllocator& mWrappedAllocator;
	};

	template <typename T, typename U>
	bool operator==(const StlAllocatorWrapper<T>& lhs, const StlAllocatorWrapper<U>& rhs)
	{
		//On equal if same allocator, check memory address
		return &lhs.mWrappedAllocator == &rhs.mWrappedAllocator;
	}

	template <typename T, typename U>
	bool operator!=(const StlAllocatorWrapper<T>& lhs, const StlAllocatorWrapper<U>& rhs)
	{
		return !(lhs == rhs);
	}
}}

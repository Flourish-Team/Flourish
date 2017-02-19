#pragma once
#include <stdlib.h> 
#include <type_traits>
#include "Memory/Allocator.h"
#include "Debug/SourceInfo.h"

namespace Flourish { namespace Memory
{	
	//This wrappers allows an allocator to used where a deleter class is needed (e.g unique_ptr) It will delallocate using the provided memory as well as optionally calling destructors
	template <typename T, bool CALL_DESTRUCTOR = true>
	class StlAllocatorDeleter
	{
		// Tag for branching at compile time on the destructor call
		template <bool>
		struct CallingDestructor {};

		StlAllocatorDeleter(Allocator& alloc) 
			: mWrappedAllocator(alloc)
		{
		}

		void operator()(T* pointer)
		{
			//Optionally destructs the object and then frees the memory
			if(pointer != nullptr)
			{
				DestructObject(CallingDestructor<CALL_DESTRUCTOR>{}, pointer);
				mWrappedAllocator.Free(pointer, MAKE_SOURCE_INFO);
			}
		}

		Allocator& GetAllocator() const
		{
			return mWrappedAllocator;
		}


		//Uses tag dispatch to branch at compile time. Should optomise out the empty function call
		static void DestructObject(CallingDestructor<true>, T* pointer)
		{
			pointer->~T();
		}

		static void DestructObject(CallingDestructor<false>, T* pointer)
		{
			//Do Nothing
		}

		Allocator& mWrappedAllocator;
	};
}}

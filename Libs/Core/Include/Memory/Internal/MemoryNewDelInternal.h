#pragma once

#include <stdlib.h> 
#include "../Allocator.h"
#include "../STLHelpers/StlAllocatorWrapper.h"
#include "../STLHelpers/StlAllocatorDeleter.h"
#include "Debug/SourceInfo.h"
#include <memory>

namespace Flourish { namespace Memory { namespace Internal 
{
	//Internal Allocation funtions. Do not use directly, instead use the macro vertsions in Memory.h

	//Alocates a object of type T and returns an unmanaged raw pointer. 
	//Has to be freed manually with DeleteRawPointer()
	template <class T, typename ...ParamArgs> 
	T* NewRawPointer(Allocator& allocator, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		auto mem = allocator.Alloc(sizeof(T), alignment, sourceInfo);
		return new (mem) T(std::forward<ParamArgs>(params)...);
	}

	//Deletes a raw pointer allocated with NewRawPointer/NewRawPointerAligned
	template <class T> 
	void DeleteRawPointer(Allocator& allocator, T *p, const Debug::SourceInfo& sourceInfo) 
	{
		if (p != nullptr) 
		{
			p->~T();
			allocator.Free(p, sourceInfo);
		}
	}

	//Alocates a object of type T and returns an unique_ptr to the object. Will be automaticly cleaned up once out of scope using the provided deleter
	template <class T, typename ...ParamArgs,  typename Deleter = StlAllocatorDeleter<T>> 
	std::unique_ptr<T, Deleter> NewUniquePtr(Allocator& allocator, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		std::unique_ptr<T, Deleter> newUniquePtr(
			NewRawPointer(allocator, alignment, sourceInfo, std::forward<ParamArgs>(params)..., Deleter(allocator)));

		return newUniquePtr;
	}

	//Should be called by the deleted used in NewUniquePtr to return the memory to the allocator in the same way it was aquired
	template <class T>
	void FreeUniquePtrMemory(Allocator& allocator, T* pointer)
	{
		DeleteRawPointer(allocator, pointer);
	}

	//Alocates a object of type T and returns an shared_ptr to the object. Will be automaticly cleaned up once out of scope using the provided deleter
	template <class T, typename ...ParamArgs> 
	std::shared_ptr<T> NewSharedPtr(Allocator& allocator, size_t alignment, ParamArgs&&... params)
	{
		return std::allocate_shared<T>(StlAllocatorWrapper<T>(allocator, alignment), std::forward<ParamArgs>(params)...);
	}

	//Alocates a buffer of raw memory large enough to hold size bytes. Has to be freed manually with DeleteRawMemBuffer()
	inline void* NewRawMemBuffer(Allocator& allocator, int size, const Debug::SourceInfo& sourceInfo)
	{
		return allocator.Alloc(size, 1, sourceInfo);
	}

	//Alocates a buffer of raw memory large enough to hold size bytes, aligned to alignment. Has to be freed manually with DeleteRawMemBuffer()
	inline void* NewRawMemBufferAligned(Allocator& allocator, int size, size_t alignment, const Debug::SourceInfo& sourceInfo)
	{
		return allocator.Alloc(size, alignment, sourceInfo);
	}

	//Deletes a raw buffer allocated with NewRawMemBuffer/NewRawMemBufferAligned.
	inline void DeleteRawMemBuffer(Allocator& allocator, void* p, const Debug::SourceInfo& sourceInfo) 
	{
		if (p != nullptr) 
		{
			allocator.Free(p, sourceInfo);
		}
	}
}}}
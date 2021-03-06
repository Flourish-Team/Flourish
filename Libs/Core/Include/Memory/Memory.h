#pragma once

#include "Platform/Platform.h"

#include <cstdlib> 
#include <memory>
#include "IAllocator.h"
#include "Internal/MemoryNewDelInternal.inl"

//Allocates a object of type T and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW()
#define FL_NEW_RAW(allocator, type, ...) \
    Flourish::Memory::Internal::NewRawPointer<type>((allocator), MAKE_SOURCE_INFO, ##__VA_ARGS__)

//Deletes a raw pointer allocated with FL_NEW_RAW
#define FL_DELETE_RAW(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawPointer((allocator), (&ptr))


//Allocates a object of type T, aligned to alignment, and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW_ALIGNED()
#define FL_NEW_RAW_ALIGNED(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewRawPointerAligned<type>((allocator), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)

//Deletes a raw pointer allocated with FL_NEW_RAW_ALIGNED
#define FL_DELETE_RAW_ALIGNED(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawPointerAligned((allocator), (&ptr))


//Allocates an array of objects of type T and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW_ARRAY()
#define FL_NEW_RAW_ARRAY(allocator, type, count, ...) \
    Flourish::Memory::Internal::NewRawPointerArray<type>((allocator), (count), MAKE_SOURCE_INFO, ##__VA_ARGS__)

//Deletes a raw pointer allocated with FL_NEW_RAW_ARRAY
#define FL_DELETE_RAW_ARRAY(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawPointerArray((allocator), (&ptr))


//Allocates an array of objects of type T, aligned to alignment, and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW_ARRAY_ALIGNED()
//(Note the size of T has to be a multiple of alignment to allow all elements to be aligned)
#define FL_NEW_RAW_ARRAY_ALIGNED(allocator, type, count, alignment, ...) \
	Flourish::Memory::Internal::NewRawPointerArrayAligned<type>((allocator), (count), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)

//Deletes a raw pointer allocated with FL_NEW_RAW_ARRAY_ALIGNED
#define FL_DELETE_RAW_ARRAY_ALIGNED(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawPointerArrayAligned((allocator), (&ptr))


//Allocates a buffer of raw memory large enough to hold size bytes. Has to be freed manually with FL_FREE()
#define FL_ALLOC(allocator, size) \
	Flourish::Memory::Internal::RawAlloc((allocator), (size), MAKE_SOURCE_INFO)

//Deletes a raw memory buffer allocated with FL_ALLOC
#define FL_FREE(allocator, ptr) \
	Flourish::Memory::Internal::RawFree((allocator), (ptr))


//Allocates a buffer of raw memory large enough to hold size bytes, aligned to alignment. Has to be freed manually with FL_FREE_ALIGN()
#define FL_ALLOC_ALIGN(allocator, size, alignment) \
	Flourish::Memory::Internal::RawAllocAligned((allocator), (size), (alignment), MAKE_SOURCE_INFO)

//Deletes a raw memory buffer allocated with FL_ALLOC_ALIGN
#define FL_FREE_ALIGN(allocator, ptr) \
	Flourish::Memory::Internal::RawFreeAligned((allocator), (ptr))


//Allocates a object of type T and returns an FL_UniquePtr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE(allocator, type, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an FL_UniquePtr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE_ALIGNED(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewUniquePtrAligned<type>((allocator), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T and returns an FL_UniquePtrArray to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE_ARRAY(allocator, type, count, ...) \
	Flourish::Memory::Internal::NewUniquePtrArray<type>((allocator), (count), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an FL_UniquePtrArray to the object. Will be automatically cleaned up once out of scope
//(Note the size of T has to be a multiple of alignment to allow all elements to be aligned)
#define FL_NEW_UNIQUE_ARRAY_ALIGNED(allocator, type, count, alignment, ...) \
	Flourish::Memory::Internal::NewUniquePtrArrayAligned<type>((allocator), (count), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T and returns an FL_SharedPtr to the object. Will be automatically cleaned up once all uses are out of scope
#define FL_NEW_SHARED(allocator, type, ...) \
	Flourish::Memory::Internal::NewSharedPtr<type>((allocator), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an FL_SharedPtr to the object. Will be automatically cleaned up once all uses are out of scope
#define FL_NEW_SHARED_ALIGNED(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewSharedPtrAligned<type>((allocator), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)

//Allocates a object of type T and returns an FL_SharedPtrArray to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_SHARED_ARRAY(allocator, type, count, ...) \
	Flourish::Memory::Internal::NewSharedPtrArray<type>((allocator), (count), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an FL_SharedPtrArray to the object. Will be automatically cleaned up once out of scope
//(Note the size of T has to be a multiple of alignment to allow all elements to be aligned)
#define FL_NEW_SHARED_ARRAY_ALIGNED(allocator, type, count, alignment, ...) \
	Flourish::Memory::Internal::NewSharedPtrArrayAligned<type>((allocator), (count), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)


// Allocates memory from the stack. The memory is automaticly freed when the calling functions returns. (NOT when the memory is out of scope)
// Care *MUST* be taken using this function, its is very easy to cause a stack overflow or have the memory been returned from underneath you.
#define FL_ALLOCA(size) alloca(size)

//This gets the address of the actual pointer that is returned from an allocator when performing an aligned allocation.
//(Mostly a helper function for using GetAllocationSize/GetMetaDataAllocationSize of an allocator)
#define FL_GET_ALLOCATED_PTR_FROM_ALIGNED_ALLOC_PTR(ptr) \
	Flourish::Memory::Internal::GetRawAllocatedPointerFromRawAllocAlignedPointer((ptr))

//This gets the address of the actual pointer that is returned from an allocator when performing an array allocation.
//(Mostly a helper function for using GetAllocationSize/GetMetaDataAllocationSize of an allocator)
#define FL_GET_ALLOCATED_PTR_FROM_ARRAY_ALLOC_PTR(ptr) \
	Flourish::Memory::Internal::GetRawAllocatedPointerFromNewRawPointerArrayPointer((ptr))

//This gets the address of the actual pointer that is returned from an allocator when performing an aligned array allocation.
//(Mostly a helper function for using GetAllocationSize/GetMetaDataAllocationSize of an allocator)
#define FL_GET_ALLOCATED_PTR_FROM_ALIGNED_ARRAY_ALLOC_PTR(ptr) \
	Flourish::Memory::Internal::GetRawAllocatedPointerFromNewRawPointerArrayAlignedPointer((ptr))


namespace Flourish::Memory
{
	//Nicer signature of the unique ptr type using our allocators
	template<class T>
	using FL_UniquePtr = std::unique_ptr<T, Internal::UniquePtrAllocatorDeleter<T>>;

	template<class T>
	using FL_UniquePtrArray = std::unique_ptr<T[], Internal::UniquePtrAllocatorDeleter<T>>;

	template<class T>
	using FL_SharedPtr = std::shared_ptr<T>;

	template<class T>
	using FL_SharedPtrArray = std::shared_ptr<T[]>;
	
}
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


//Allocates a object of type T and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE(allocator, type, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE_ALIGN(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewUniquePtrAligned<type>((allocator), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE_ARRAY(allocator, type, count, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), (count), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
#define FL_NEW_UNIQUE_ARRAY_ALIGN(allocator, type, count, alignment, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), (count), (alignment), MAKE_SOURCE_INFO, ##__VA_ARGS__)


//Allocates a object of type T and returns an shared_ptr to the object. Will be automatically cleaned up once all uses are out of scope
#define FL_NEW_SHARED(allocator, type, ...) \
	Flourish::Memory::Internal::NewSharedPtr<type>((allocator), ##__VA_ARGS__)


//Allocates a object of type T, aligned to alignment, and returns an shared_ptr to the object. Will be automatically cleaned up once all uses are out of scope
#define FL_NEW_SHARED_ALIGN(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewSharedPtrAligned<type>((allocator), (alignment), ##__VA_ARGS__)


// Allocates memory from the stack. The memory is automaticly freed when the calling functions returns. (NOT when the memory is out of scope)
// Care *MUST* be taken using this function, its is very easy to cause a stack overflow or have the memory been returned from underneath you.
#define FL_ALLOCA(size) alloca(size)

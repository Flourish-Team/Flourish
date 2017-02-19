#pragma once

#include "Platform/Platform.h"

#include <stdlib.h> 
#include <memory>
#if FL_ENABLED(FL_PLATFORM_WINDOWS)
    #include <malloc.h>
#endif
#include "Allocator.h"
#include "Internal/MemoryNewDelInternal.h"

//Alocates a object of type T and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW()
#define FL_NEW_RAW(allocator, type, ...) \
	Flourish::Memory::Internal::NewRawPointer<type>((allocator), alignof(type), MAKE_SOURCE_INFO, __VA_ARGS__)

//Alocates a object of type T, aligned to alignment, and returns an unmanaged raw pointer. Has to be freed manually with FL_DELETE_RAW()
#define FL_NEW_RAW_ALIGN(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewRawPointer<type>((allocator), (alignment), MAKE_SOURCE_INFO, __VA_ARGS__)

//Deletes a raw pointer allocated with FL_NEW_RAW/FL_NEW_RAW_ALIGN
#define FL_DELETE_RAW(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawPointer((allocator), (ptr), MAKE_SOURCE_INFO)



//Alocates a object of type T and returns an unique_ptr to the object. Will be automaticly cleaned up once out of scope using the provided deleter
#define FL_NEW_UNIQUE(allocator, type, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), alignof(type), MAKE_SOURCE_INFO, __VA_ARGS__)

//Alocates a object of type T, aligned to alignment, and returns an unique_ptr to the object. Will be automaticly cleaned up once out of scope using the procided deleter
#define FL_NEW_UNIQUE_ALIGN(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewUniquePtr<type>((allocator), (alignment), MAKE_SOURCE_INFO, __VA_ARGS__)



//Alocates a object of type T and returns an shared_ptr to the object. Will be automaticly cleaned up once out of scope using the provided deleter
#define FL_NEW_SHARED(allocator, type, ...) \
	Flourish::Memory::Internal::NewSharedPtr<type>((allocator), alignof(type), __VA_ARGS__)

//Alocates a object of type T, aligned to alignment, and returns an shared_ptr to the object. Will be automaticly cleaned up once out of scope using the procided deleter
#define FL_NEW_SHARED_ALIGN(allocator, type, alignment, ...) \
	Flourish::Memory::Internal::NewSharedPtr<type>((allocator), (alignment), __VA_ARGS__)



//Alocates a buffer of raw memory large enough to hold size bytes. Has to be freed manually with DeleteRawMemBuffer()
#define FL_NEW_RAW_BUFFER(allocator, size) \
	Flourish::Memory::Internal::NewRawMemBuffer<type>((allocator), (size), MAKE_SOURCE_INFO)

//Alocates a buffer of raw memory large enough to hold size bytes, aligned to alignment. Has to be freed manually with DeleteRawMemBuffer()
#define FL_NEW_RAW_BUFFER_ALIGN(allocator, size, alignment) \
	Flourish::Memory::Internal::NewRawMemBuffer<type>((allocator), (size), (alignment), MAKE_SOURCE_INFO)

//Deletes a raw memory buffer allocated with FL_NEW_RAW_BUFFER/FL_NEW_RAW_BUFFER_ALIGN
#define FL_DELETE_MEM_BUFFER(allocator, ptr) \
	Flourish::Memory::Internal::DeleteRawMemBuffer((allocator), (ptr), MAKE_SOURCE_INFO)

// Allocates memory from the stack. The memory is automaticly freed when the calling functions returns.(NOT when the memory is out of scope)
// Care *MUST* be taken using this function, its is very easy to cause a stack overflow or have the memory been returned from underneath you.
#define FL_ALLOCA(size) alloca(size)

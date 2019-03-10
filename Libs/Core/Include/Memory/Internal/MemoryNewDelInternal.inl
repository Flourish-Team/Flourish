#pragma once

#include <memory>
#include "../IAllocator.h"
#include "../STLHelpers/StlAllocatorWrapper.h"
#include "../STLHelpers/StlAllocatorAlignedWrapper.h"
#include "Memory/AddressUtils.h"
#include "Debug/SourceInfo.h"
#include "Debug/Assert.h"



namespace Flourish { namespace Memory { namespace Internal 
{
	//Internal Allocation functions. Do not use directly, instead use the macro versions in Memory.h

	//data for NewRawPointerArrayAligned
	struct AllocAlignArrayHeader
	{
		size_t offset;
		size_t arrayCount;
	};

	//Helper function used by others to allocate a buffer of raw memory large enough to hold size bytes, aligned to alignment, and has headerSize bytes space before the alignment.
	//Returns the alligned ptr, and sets basePtrOut to the base ptr allocated by the allocator to allow freeing later 
	FL_FORCE_INLINE void RawAllocAlignedWithHeaderSpaceHelper(IAllocator& allocator, size_t size, size_t alignment, size_t headerSize, const Debug::SourceInfo& sourceInfo, void** basePtrOut,  void** alignedPtrOut)
	{
		FL_ASSERT_MSG(alignment >= 2, "Alignment must be >= 2");
		FL_ASSERT_MSG(alignment && ((alignment & (alignment - 1)) == 0), "Alignment must be power of 2");
		FL_ASSERT_MSG(size % alignment == 0, "Size must be a multiple of alignment");

		//Added extra space so we can guarantee alignment + storage for offset
		const size_t spaceNeeded = size + headerSize + (alignment - 1);

		//allocator memory
		(*basePtrOut) = allocator.Alloc(spaceNeeded, sourceInfo);

		//offset base ptr to align it
		(*alignedPtrOut) = AddressUtils::AlignAddress((*basePtrOut), alignment, headerSize);
	}

	//Allocates a buffer of raw memory large enough to hold size bytes. Has to be freed manually with RawFree()
	FL_FORCE_INLINE void* RawAlloc(IAllocator& allocator, size_t size, const Debug::SourceInfo& sourceInfo)
	{
		return allocator.Alloc(size, sourceInfo);
	}

	//Allocates a buffer of raw memory large enough to hold size bytes, aligned to alignment. Has to be freed manually with RawFreeAligned()
	FL_FORCE_INLINE void* RawAllocAligned(IAllocator& allocator, size_t size, size_t alignment, const Debug::SourceInfo& sourceInfo)
	{
		//allocate memory
		void* baseMemPtr;
		void* alignedPtr;
		RawAllocAlignedWithHeaderSpaceHelper(allocator, size, alignment, sizeof(size_t), sourceInfo, &baseMemPtr, &alignedPtr);

		//record the offset in the space before the aligned ptr
		size_t* offset = static_cast<size_t*>(alignedPtr) - 1;
		(*offset)= static_cast<size_t>(AddressUtils::AddressDiff(baseMemPtr, alignedPtr));

		return alignedPtr;
	}

	//Frees a buffer of raw memory allocated with RawAlloc()
	FL_FORCE_INLINE void RawFree(IAllocator& allocator, void* ptr)
	{
		FL_ASSERT_MSG(ptr != nullptr, "Trying to free null ptr");
		allocator.Free(ptr);
	}

	//Frees a buffer of raw memory allocated with RawAllocAligned()
	FL_FORCE_INLINE void RawFreeAligned(IAllocator& allocator, void* ptr)
	{
		FL_ASSERT_MSG(ptr != nullptr, "Trying to free null ptr");

		//Step back and get the offset to the actual allocated ptr
		size_t* offset = static_cast<size_t *>(ptr) - 1;
		void* basePtr = static_cast<void *>(AddressUtils::AddressSubOffset(ptr, (*offset)));

		allocator.Free(basePtr);
	}

	//Allocates a object of type T and returns an unmanaged raw pointer. 
	//Has to be freed manually with DeleteRawPointer()
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE T* NewRawPointer(IAllocator& allocator, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		void* mem = RawAlloc(allocator, sizeof(T), sourceInfo);
		return new (mem) T(std::forward<ParamArgs>(params)...);
	}

	//Deletes a raw pointer allocated with NewRawPointer, Pointer is passed in via pointer and is null'd after delete
	template <class T> 
	FL_FORCE_INLINE void DeleteRawPointer(IAllocator& allocator, T **pointer) 
	{
		T* pointerValue = (*pointer);
		if (pointerValue != nullptr) 
		{
			pointerValue->~T();
			RawFree(allocator,pointerValue);
			pointer = nullptr;
		}
	}

	//Allocates a object of type T, aligned to alignment and returns an unmanaged raw pointer. 
	//Has to be freed manually with DeleteRawPointer()
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE T* NewRawPointerAligned(IAllocator& allocator, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		void* mem = RawAllocAligned(allocator, sizeof(T), alignment, sourceInfo);
		return new (mem) T(std::forward<ParamArgs>(params)...);
	}

	//Deletes a raw pointer allocated with NewRawPointerAligned, Pointer is passed in via pointer and is null'd after delete
	template <class T> 
	FL_FORCE_INLINE void DeleteRawPointerAligned(IAllocator& allocator, T **pointer) 
	{
		T* pointerValue = (*pointer);
		if (pointerValue != nullptr) 
		{
			pointerValue->~T();
			RawFreeAligned(allocator, pointerValue);
			pointer = nullptr;
		}
	}

	//Allocates an array of objects of type T, and returns an unmanaged raw pointer. 
	//Has to be freed manually with DeleteRawPointer()
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE T* NewRawPointerArray(IAllocator& allocator, size_t count, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		FL_ASSERT_MSG(count > 0, "Array count must be > 0");

		void* mem = RawAlloc(allocator, sizeof(T) * count + sizeof(size_t), sourceInfo);

		//Step back and record the array count for when we delete it
		size_t* arrayCount = static_cast<size_t *>(mem);
		(*arrayCount) = count;

		T* arrayStart = static_cast<T*>(arrayCount + 1);

		for(size_t i = 0; i < count; ++i)
		{
			new (&arrayStart[i]) T(std::forward<ParamArgs>(params)...);
		}

		return arrayStart;
	}

	//Deletes a raw pointer allocated with NewRawPointer, Pointer is passed in via pointer and is null'd after delete
	template <class T> 
	FL_FORCE_INLINE void DeleteRawPointerArray(IAllocator& allocator, T **pointer) 
	{
		T* pointerValue = (*pointer);
		if (pointerValue != nullptr) 
		{
			//Get the array count from before the array
			size_t* arrayCount = static_cast<size_t *>(pointerValue) - 1;

			for(size_t i = 0; i < (*arrayCount); ++i)
			{
				pointerValue[i].~T();
			}

			allocator.Free(pointerValue);
			pointer = nullptr;
		}
	}

	//Allocates am array of objects of type T, aligned to alignment and returns an unmanaged raw pointer. 
	//Has to be freed manually with DeleteRawPointer()
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE T* NewRawPointerArrayAligned(IAllocator& allocator, size_t count, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		FL_ASSERT_MSG(count > 0, "Array count must be > 0");

		//allocate memory
		void* baseMemPtr;
		void* alignedPtr;
		RawAllocAlignedWithHeaderSpaceHelper(allocator, sizeof(T) * count, alignment, sizeof(AllocAlignArrayHeader), sourceInfo, &baseMemPtr, &alignedPtr);

		//record the offset and array count in the space before the aligned ptr
		AllocAlignArrayHeader* header = static_cast<AllocAlignArrayHeader*>(alignedPtr) - 1;

		(*header).offset = static_cast<size_t>(AddressUtils::AddressDiff(baseMemPtr, alignedPtr));
		(*header).arrayCount = count;

		T* arrayStart = static_cast<T*>(alignedPtr);

		for(size_t i = 0; i < count; ++i)
		{
			new (&arrayStart[i]) T(std::forward<ParamArgs>(params)...);
		}

		return arrayStart;
	}

	//Deletes a raw pointer allocated with NewRawPointerArrayAligned, Pointer is passed in via pointer and is null'd after delete
	template <class T> 
	FL_FORCE_INLINE void DeleteRawPointerArrayAligned(IAllocator& allocator, T **pointer) 
	{
		T* pointerValue = (*pointer);
		if (pointerValue != nullptr) 
		{
			//Get the data from before the array
			AllocAlignArrayHeader* header = static_cast<AllocAlignArrayHeader*>(pointerValue) - 1;

			for(size_t i = 0; i < (*header).arrayCount; ++i)
			{
				pointerValue[i].~T();
			}

			void* basePtr = static_cast<void *>(AddressUtils::AddressSubOffset(pointerValue, (*header).offset));

			allocator.Free(basePtr);
			pointer = nullptr;
		}
	}

	//Custom Deleters for unique that can deallocate with allocator
	template <typename T>
	class UniquePtrAllocatorDeleter
	{
	public:
		typedef void (*DeleteFunction)(IAllocator& alloc, T** pointer); 

		UniquePtrAllocatorDeleter(IAllocator& allocator, const DeleteFunction deleteFunction) 
			: mWrappedAllocator(allocator)
			, mDeleteFunction(deleteFunction)
		{
		}

		void operator()(T* pointer)
		{
			mDeleteFunction(mWrappedAllocator, &pointer);
		}

	private:
		IAllocator& mWrappedAllocator;
		DeleteFunction mDeleteFunction;
	};


	//Allocates a object of type T and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE std::unique_ptr<T, UniquePtrAllocatorDeleter<T>> NewUniquePtr(IAllocator& allocator, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		std::unique_ptr<T, UniquePtrAllocatorDeleter<T>> newUniquePtr(
			NewRawPointer(allocator, sourceInfo, std::forward<ParamArgs>(params)..., 
			UniquePtrAllocatorDeleter(allocator, DeleteRawPointer)));

		return newUniquePtr;
	}

	//Allocates a object of type T and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE std::unique_ptr<T[], UniquePtrAllocatorDeleter<T>> NewUniquePtrArray(IAllocator& allocator, size_t count, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		std::unique_ptr<T[], UniquePtrAllocatorDeleter<T>> newUniquePtr(
			NewRawPointerArray<T>(allocator, count, sourceInfo, std::forward<ParamArgs>(params)..., 
			UniquePtrAllocatorDeleter(allocator, DeleteRawPointerArray)));

		return newUniquePtr;
	}

	//Allocates a object of type T with alignment and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE std::unique_ptr<T, UniquePtrAllocatorDeleter<T>> NewUniquePtrAligned(IAllocator& allocator, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		std::unique_ptr<T, UniquePtrAllocatorDeleter<T>> newUniquePtr(
			NewRawPointerAligned(allocator, alignment, sourceInfo, std::forward<ParamArgs>(params)..., 
			UniquePtrAllocatorDeleter(allocator, DeleteRawPointerAligned)));

		return newUniquePtr;
	}

	//Allocates an array of objects of type T with alignment and returns an unique_ptr to the object. Will be automatically cleaned up once out of scope
	template <class T, typename ...ParamArgs> 
	FL_FORCE_INLINE std::unique_ptr<T[], UniquePtrAllocatorDeleter<T>> NewUniquePtrArrayAligned(IAllocator& allocator, size_t count, size_t alignment, const Debug::SourceInfo& sourceInfo, ParamArgs&&... params)
	{
		std::unique_ptr<T[], UniquePtrAllocatorDeleter<T>> newUniquePtr(
			NewRawPointerArrayAligned(allocator, count, alignment, sourceInfo, std::forward<ParamArgs>(params)..., 
			UniquePtrAllocatorDeleter(allocator, DeleteRawPointerArrayAligned)));

		return newUniquePtr;
	}
	

	//Allocates a object of type T and returns an shared_ptr to the object. Will be automatically cleaned up once out of scope using the provided deleter
	template <class T, typename ...ParamArgs> 
	std::shared_ptr<T> NewSharedPtr(IAllocator& allocator, size_t alignment, ParamArgs&&... params)
	{
		return std::allocate_shared<T>(StlAllocatorWrapper<T>(allocator, alignment), std::forward<ParamArgs>(params)...);
	}

	//Allocates a object of type T with alignment and returns an shared_ptr to the object. Will be automatically cleaned up once out of scope using the provided deleter
	template <class T, typename ...ParamArgs> 
	std::shared_ptr<T> NewSharedPtrAligned(IAllocator& allocator, size_t alignment, ParamArgs&&... params)
	{
		return std::allocate_shared<T>(StlAllocatorAlignedWrapper<T>(allocator, alignment), std::forward<ParamArgs>(params)...);
	}

}}}

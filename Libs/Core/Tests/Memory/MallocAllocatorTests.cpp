#include "Test.h"
#include "Memory/Memory.h"
#include "Memory/Allocators/MallocAllocator.h"

using namespace Flourish;
using namespace Memory;

TEST(MallocAllocatorTests, NameSetCorrectly)
{
	MallocAllocator mallocAllocator("TestAllocator");

	ASSERT_STRING_EQUAL(mallocAllocator.GetAllocatorName(), "TestAllocator [MallocAllocator]");
}

TEST(MallocAllocatorTests, NewDeleteRawPointer)
{
	MallocAllocator mallocAllocator("TestAllocator");

	int* intPtr = static_cast<int*>(mallocAllocator.Alloc(sizeof(int), MAKE_SOURCE_INFO));

	ASSERT_NOT_EQUAL(intPtr, nullptr);

	(*intPtr) = 12345;

	ASSERT_EQUAL(mallocAllocator.GetAllocationSize(intPtr), sizeof(int));
	ASSERT_EQUAL(mallocAllocator.GetMetaDataAllocationSize(intPtr), sizeof(size_t));
	ASSERT_EQUAL((*intPtr), 12345);

	mallocAllocator.Free(intPtr);
}
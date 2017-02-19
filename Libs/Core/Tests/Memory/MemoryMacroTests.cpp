#include "Test.h"
#include "Memory/Memory.h"
#include "Memory/Allocators/MallocAllocator.h"

using namespace Flourish;
using namespace Memory;

TEST(MemoryMacroTests, NewDeleteRawPointer)
{
	MallocAllocator mallocAllocator;

	auto intPtr = FL_NEW_RAW(mallocAllocator, int);

	(*intPtr) = 12345;

	ASSERT_EQUAL((*intPtr), 12345);

	FL_DELETE_RAW(mallocAllocator, intPtr);
}

TEST(MemoryMacroTests, NewDeleteRawAlignedPointer)
{
	MallocAllocator mallocAllocator;

	auto intPtr = FL_NEW_RAW_ALIGN(mallocAllocator, int, 64);

	ASSERT_TRUE(AddressUtils::IsAligned(intPtr, 64));

	(*intPtr) = 12345;

	ASSERT_EQUAL((*intPtr), 12345);


	FL_DELETE_RAW(mallocAllocator, intPtr);
}
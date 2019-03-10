#include "Test.h"
#include "Memory/Memory.h"
#include "Memory/Allocators/MallocAllocator.h"

using namespace Flourish;
using namespace Memory;


TEST(MemoryMacroTests, NewDeleteRawPointer)
{
	MallocAllocator mallocAllocator("TestAllocator");

	auto intPtr = FL_NEW_RAW(mallocAllocator, int);

	(*intPtr) = 12345;

	ASSERT_EQUAL((*intPtr), 12345);

	FL_DELETE_RAW(mallocAllocator, intPtr);
}

TEST(MemoryMacroTests, NewDeleteRawAlignedPointer)
{
	struct TestData
	{
		int intA;
		int intB;
	};

	MallocAllocator mallocAllocator("TestAllocator");

	auto intPtr = FL_NEW_RAW_ALIGNED(mallocAllocator, TestData, 4);

	ASSERT_TRUE(AddressUtils::IsAligned(intPtr, 4));

	(*intPtr).intA = 12345;
	(*intPtr).intB = 67890;

	ASSERT_EQUAL((*intPtr).intA, 12345);
	ASSERT_EQUAL((*intPtr).intB, 67890);

	FL_DELETE_RAW_ALIGNED(mallocAllocator, intPtr);
}
#include "Test.h"
#include "Memory/Memory.h"
#include "MemoryTestMockAllocator.h"
#include "MemoryTestData.h"

using namespace Flourish;
using namespace Memory;
using namespace Testing;

TEST(MemoryMacroTests, NewDeleteRawPointer)
{
	TestAllocator<64> testAllocator;
	CtorDtorCallStats callStats;

	TestClass* testPtr = FL_NEW_RAW(testAllocator, TestClass, callStats);

	testPtr->Data = 0xABCDABCDu;

	ASSERT_EQUAL(testPtr->Data, 0xABCDABCDu);

	FL_DELETE_RAW(testAllocator, testPtr);

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewDeleteRawAlignedPointer)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	auto testPtr = FL_NEW_RAW_ALIGNED(testAllocator, TestClass, 128, callStats);

	ASSERT_TRUE(AddressUtils::IsAligned(testPtr, 128));

	testPtr->Data = 0xABCDABCDu;

	ASSERT_EQUAL(testPtr->Data, 0xABCDABCDu);

	FL_DELETE_RAW_ALIGNED(testAllocator, testPtr);

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewDeleteRawPointerArray)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	TestClass* intPtr = FL_NEW_RAW_ARRAY(testAllocator, TestClass, 4, callStats);

	intPtr[0].Data = 0x12341234u;
	intPtr[1].Data = 0x56785678u;
	intPtr[2].Data = 0x90AB90ABu;
	intPtr[3].Data = 0xCDEFCDEFu;

	ASSERT_EQUAL(intPtr[0].Data, 0x12341234u);
	ASSERT_EQUAL(intPtr[1].Data, 0x56785678u);
	ASSERT_EQUAL(intPtr[2].Data, 0x90AB90ABu);
	ASSERT_EQUAL(intPtr[3].Data, 0xCDEFCDEFu);

	FL_DELETE_RAW_ARRAY(testAllocator, intPtr);

	ASSERT_EQUAL(callStats.NumCtorCalls, 4);
	ASSERT_EQUAL(callStats.NumDtorCalls, 4);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewDeleteRawAlignedPointerArray)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	TestClass* testData = FL_NEW_RAW_ARRAY_ALIGNED(testAllocator, TestClass, 4, 32, callStats);

	ASSERT_TRUE(AddressUtils::IsAligned(&testData[0], 32));
	ASSERT_TRUE(AddressUtils::IsAligned(&testData[1], 32));
	ASSERT_TRUE(AddressUtils::IsAligned(&testData[2], 32));
	ASSERT_TRUE(AddressUtils::IsAligned(&testData[3], 32));

	testData[0].Data = 0x12341234u;
	testData[1].Data = 0x56785678u;
	testData[2].Data = 0x90AB90ABu;
	testData[3].Data = 0xCDEFCDEFu;

	ASSERT_EQUAL(testData[0].Data, 0x12341234u);
	ASSERT_EQUAL(testData[1].Data, 0x56785678u);
	ASSERT_EQUAL(testData[2].Data, 0x90AB90ABu);
	ASSERT_EQUAL(testData[3].Data, 0xCDEFCDEFu);

	FL_DELETE_RAW_ARRAY_ALIGNED(testAllocator, testData);

	ASSERT_EQUAL(callStats.NumCtorCalls, 4);
	ASSERT_EQUAL(callStats.NumDtorCalls, 4);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, AllocFreeMemory)
{
	TestAllocator<64> testAllocator;

	void* testPtr = FL_ALLOC(testAllocator, 32);

    unsigned int* intPtr = static_cast<unsigned int*>(testPtr);
	(*intPtr) = 0xABCDABCDu;

	ASSERT_EQUAL((*intPtr), 0xABCDABCDu);

	FL_FREE(testAllocator, testPtr);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, AllocFreeAlignedMemory)
{
	TestAllocator<256> testAllocator;

	void* testPtr = FL_ALLOC_ALIGN(testAllocator, 32, 128);

	ASSERT_TRUE(AddressUtils::IsAligned(testPtr, 128));

    unsigned int* intPtr = static_cast<unsigned int*>(testPtr);
	(*intPtr) = 0xABCDABCDu;

	ASSERT_EQUAL((*intPtr), 0xABCDABCDu);

	FL_FREE_ALIGN(testAllocator, testPtr);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewUniquePtr)
{
	TestAllocator<64> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_UNIQUE(testAllocator, TestClass, callStats);

		testUniquePtr->Data = 0xABCDABCDu;

		ASSERT_EQUAL(testUniquePtr->Data, 0xABCDABCDu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewUniquePtrAligned)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_UNIQUE_ALIGNED(testAllocator, TestClass, 128, callStats);

		ASSERT_TRUE(AddressUtils::IsAligned(testUniquePtr.get(), 128));

		testUniquePtr->Data = 0xABCDABCDu;

		ASSERT_EQUAL(testUniquePtr->Data, 0xABCDABCDu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewUniquePtrArray)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_UNIQUE_ARRAY(testAllocator, TestClass, 4, callStats);

		testUniquePtr[0].Data = 0x12341234u;
		testUniquePtr[1].Data = 0x56785678u;
		testUniquePtr[2].Data = 0x90AB90ABu;
		testUniquePtr[3].Data = 0xCDEFCDEFu;

		ASSERT_EQUAL(testUniquePtr[0].Data, 0x12341234u);
		ASSERT_EQUAL(testUniquePtr[1].Data, 0x56785678u);
		ASSERT_EQUAL(testUniquePtr[2].Data, 0x90AB90ABu);
		ASSERT_EQUAL(testUniquePtr[3].Data, 0xCDEFCDEFu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 4);
	ASSERT_EQUAL(callStats.NumDtorCalls, 4);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewUniquePtrArrayAligned)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_UNIQUE_ARRAY_ALIGNED(testAllocator, TestClass, 4, 32, callStats);

		ASSERT_TRUE(AddressUtils::IsAligned(&testUniquePtr[0], 32));
		ASSERT_TRUE(AddressUtils::IsAligned(&testUniquePtr[1], 32));
		ASSERT_TRUE(AddressUtils::IsAligned(&testUniquePtr[2], 32));
		ASSERT_TRUE(AddressUtils::IsAligned(&testUniquePtr[3], 32));

		testUniquePtr[0].Data = 0x12341234u;
		testUniquePtr[1].Data = 0x56785678u;
		testUniquePtr[2].Data = 0x90AB90ABu;
		testUniquePtr[3].Data = 0xCDEFCDEFu;

		ASSERT_EQUAL(testUniquePtr[0].Data, 0x12341234u);
		ASSERT_EQUAL(testUniquePtr[1].Data, 0x56785678u);
		ASSERT_EQUAL(testUniquePtr[2].Data, 0x90AB90ABu);
		ASSERT_EQUAL(testUniquePtr[3].Data, 0xCDEFCDEFu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 4);
	ASSERT_EQUAL(callStats.NumDtorCalls, 4);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewSharedPtr)
{
	TestAllocator<64> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_SHARED(testAllocator, TestClass, callStats);

		testUniquePtr->Data = 0xABCDABCDu;

		ASSERT_EQUAL(testUniquePtr->Data, 0xABCDABCDu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(MemoryMacroTests, NewSharedPtrAligned)
{
	TestAllocator<256> testAllocator;
	CtorDtorCallStats callStats;

	{
		auto testUniquePtr = FL_NEW_SHARED_ALIGNED(testAllocator, TestClass, 128, callStats);

		ASSERT_TRUE(AddressUtils::IsAligned(testUniquePtr.get(), 128));

		testUniquePtr->Data = 0xABCDABCDu;

		ASSERT_EQUAL(testUniquePtr->Data, 0xABCDABCDu);
	}

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

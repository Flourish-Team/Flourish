#include "Test.h"
#include "Memory/Memory.h"
#include "MemoryTestMockAllocator.h"
#include "MemoryTestData.h"
#include "Memory/Debug/SimpleTrackingAllocatorWrapper.h"


using namespace Flourish;
using namespace Memory;
using namespace Testing;

TEST(SimpleTrackingAllocatorWrapperTests, NameSetCorrectly)
{
	TestAllocator<64> testAllocator;
	SimpleTrackingAllocatorWrapper wrapAllocator(testAllocator);

	ASSERT_STRING_EQUAL(wrapAllocator.GetAllocatorName(), "Test Allocator [Wrapped by SimpleTrackingAllocatorWrapper]");
}

TEST(SimpleTrackingAllocatorWrapperTests, NewAndDeleteWrapCorrectly)
{
	TestAllocator<64> testAllocator;
	SimpleTrackingAllocatorWrapper wrapAllocator(testAllocator);

	CtorDtorCallStats callStats;

	TestClass* testPtr = FL_NEW_RAW(wrapAllocator, TestClass, callStats);

	testPtr->Data = 0xABCDABCDu;

	ASSERT_EQUAL(testPtr->Data, 0xABCDABCDu);

	FL_DELETE_RAW(wrapAllocator, testPtr);

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}
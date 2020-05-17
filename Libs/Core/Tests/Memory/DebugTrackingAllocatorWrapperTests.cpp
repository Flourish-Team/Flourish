#include "Test.h"
#include "Memory/Memory.h"
#include "MemoryTestMockAllocator.h"
#include "MemoryTestData.h"
#include "Memory/Debug/DebugTrackingAllocatorWrapper.h"
#include "Memory/Allocators/MallocAllocator.h"
#include "Debug/AssertMockHandler.h"


using namespace Flourish;
using namespace Memory;
using namespace Testing;

TEST(DebugTrackingAllocatorWrapperTests, NameSetCorrectly)
{
	TestAllocator<64> testAllocator;
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	ASSERT_STRING_EQUAL(wrapAllocator.GetAllocatorName(), "Test Allocator [Wrapped by DebugTrackingAllocatorWrapper]");
}

TEST(DebugTrackingAllocatorWrapperTests, NewAndDeleteWrapCorrectly)
{
	TestAllocator<64> testAllocator;
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	CtorDtorCallStats callStats;

	TestClass* testPtr = FL_NEW_RAW(wrapAllocator, TestClass, callStats);

	testPtr->Data = 0xABCDABCDu;

	ASSERT_EQUAL(testPtr->Data, 0xABCDABCDu);

	FL_DELETE_RAW(wrapAllocator, testPtr);

	ASSERT_EQUAL(callStats.NumCtorCalls, 1);
	ASSERT_EQUAL(callStats.NumDtorCalls, 1);

	testAllocator.GTest_ValidateAllocator();
}

TEST(DebugTrackingAllocatorWrapperTests, TestOverwriteAsserts)
{
	Debug::Testing::AssertMockHandler assertHandler;

	MallocAllocator testAllocator = MallocAllocator("Test Allocator");
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	uint32_t* testPtr = FL_NEW_RAW(wrapAllocator, uint32_t);

	testPtr[0] = 0xABCDABCDu;
	testPtr[1] = 0xABCDABCDu;

	ASSERT_EQUAL((*testPtr), 0xABCDABCDu);
	FL_DELETE_RAW(wrapAllocator, testPtr);

	ASSERT_EQ(assertHandler.AssertCalled, true);

	const char* assertMessageStart = "Memory Curruption! Allocation Back Guard has been overwriten";

	ASSERT_EQ(strncmp(assertHandler.LastAssertMessageString, assertMessageStart, strlen(assertMessageStart)), 0);
}

TEST(DebugTrackingAllocatorWrapperTests, TestUnderwriteAsserts)
{
	Debug::Testing::AssertMockHandler assertHandler;

	MallocAllocator testAllocator = MallocAllocator("Test Allocator");
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	uint32_t* testPtr = FL_NEW_RAW(wrapAllocator, uint32_t);

	testPtr[0] = 0xABCDABCDu;
	testPtr[-1] = 0xABCDABCDu;

	ASSERT_EQUAL((*testPtr), 0xABCDABCDu);
	FL_DELETE_RAW(wrapAllocator, testPtr);

	ASSERT_EQ(assertHandler.AssertCalled, true);

	const char* assertMessageStart = "Memory Curruption! Allocation Front Guard has been overwriten";

	ASSERT_EQ(strncmp(assertHandler.LastAssertMessageString, assertMessageStart, strlen(assertMessageStart)), 0);
}

TEST(DebugTrackingAllocatorWrapperTests, TestUninitilizedMemPattern)
{
	Debug::Testing::AssertMockHandler assertHandler;

	MallocAllocator testAllocator = MallocAllocator("Test Allocator");
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	uint32_t* testPtr = static_cast<uint32_t*>(FL_ALLOC(wrapAllocator, sizeof(uint32_t) * 10));

	for(int32_t i = 0; i < 10; ++i)
	{
		ASSERT_EQUAL(testPtr[i], DebugTrackingAllocatorWrapper::UNINITIALIZED_MEMORY_PATTERN);
	}

	FL_FREE(wrapAllocator, testPtr);
}


TEST(DebugTrackingAllocatorWrapperTests, TestLeakTracking)
{
	Debug::Testing::AssertMockHandler assertHandler;

	testing::internal::CaptureStdout();

	void* addressA;
	void* addressB;
	void* addressC;

	size_t allocSizeA;
	size_t allocSizeB;
	size_t allocSizeC;
	
	size_t metaDataASize;
	size_t metaDataBSize;
	size_t metaDataCSize;

	size_t baseLineNumber;

	const char* fileName = __FILE__;
	const char* functionName = FL_FUNCTION;
	
	void* arrayAllocPtr;
	
	{
		MallocAllocator testAllocator = MallocAllocator("Test Allocator");
		DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

		baseLineNumber = __LINE__;
		addressA = FL_NEW_RAW(wrapAllocator, int32_t);
		addressB = FL_NEW_RAW(wrapAllocator, float_t);
		addressC = FL_NEW_RAW_ARRAY(wrapAllocator, double_t, 32);

		arrayAllocPtr = FL_GET_ALLOCATED_PTR_FROM_ARRAY_ALLOC_PTR(addressC);

		allocSizeA = wrapAllocator.GetAllocationSize(addressA);
		allocSizeB = wrapAllocator.GetAllocationSize(addressB);
		allocSizeC = wrapAllocator.GetAllocationSize(arrayAllocPtr);
		
		metaDataASize = testAllocator.GetMetaDataAllocationSize(addressA);
		metaDataBSize = testAllocator.GetMetaDataAllocationSize(addressB);
		metaDataCSize = testAllocator.GetMetaDataAllocationSize(arrayAllocPtr);
	}

	std::string capturedOutput = testing::internal::GetCapturedStdout();

	char expectedMessage[4096];

	sprintf_s(expectedMessage, "Leaks Detected in allocator 'Test Allocator [MallocAllocator]'\n"
		"Leaked Allocations:\n"
		"(Note 28 bytes (Not shown in stats) currently added by tracking to each allocation)\n\n"
		"- 0x%08X at %s(%d): %s (%d bytes [+%d metadata bytes])\n"
		"- 0x%08X at %s(%d): %s (%d bytes [+%d metadata bytes])\n"
		"- 0x%08X at %s(%d): %s (%d bytes [+%d metadata bytes])\n",
		reinterpret_cast<intptr_t>(addressA), fileName, baseLineNumber + 1, functionName, allocSizeA, metaDataASize, 
		reinterpret_cast<intptr_t>(addressB), fileName, baseLineNumber + 2, functionName, allocSizeB, metaDataBSize, 
		reinterpret_cast<intptr_t>(arrayAllocPtr), fileName, baseLineNumber + 3, functionName, allocSizeC, metaDataCSize);

	ASSERT_EQ(strncmp(capturedOutput.c_str(), expectedMessage, strlen(expectedMessage)), 0);
}


TEST(DebugTrackingAllocatorWrapperTests, TestAllocationReport)
{
	Debug::Testing::AssertMockHandler assertHandler;

	MallocAllocator testAllocator = MallocAllocator("Test Allocator");
	DebugTrackingAllocatorWrapper wrapAllocator(testAllocator);

	const char* fileName = __FILE__;
	const char* functionName = FL_FUNCTION;
	
	const int32_t allocALineNumber = __LINE__ + 1;
	void* addressA = FL_NEW_RAW(wrapAllocator, int32_t);
	void* addressB = FL_NEW_RAW(wrapAllocator, float_t);
	double_t* addressC = FL_NEW_RAW_ARRAY(wrapAllocator, double_t, 32);

	void* arrayAllocPtr = FL_GET_ALLOCATED_PTR_FROM_ARRAY_ALLOC_PTR(addressC);

	const size_t allocSizeA = wrapAllocator.GetAllocationSize(addressA);
	const size_t allocSizeB = wrapAllocator.GetAllocationSize(addressB);
	const size_t allocSizeC = wrapAllocator.GetAllocationSize(arrayAllocPtr);

	const size_t metaDataASize = testAllocator.GetMetaDataAllocationSize(addressA);
	const size_t metaDataBSize = testAllocator.GetMetaDataAllocationSize(addressB);
	const size_t metaDataCSize = testAllocator.GetMetaDataAllocationSize(arrayAllocPtr);

	FL_DELETE_RAW(wrapAllocator, addressB);

	const int32_t allocDLineNumber = __LINE__ + 1;
	void* addressD = FL_NEW_RAW(wrapAllocator, int64_t);
	void* addressE = FL_NEW_RAW(wrapAllocator, int64_t);

	const size_t allocSizeD = wrapAllocator.GetAllocationSize(addressD);
	const size_t allocSizeE = wrapAllocator.GetAllocationSize(addressE);

	const size_t metaDataDSize = testAllocator.GetMetaDataAllocationSize(addressD);
	const size_t metaDataESize = testAllocator.GetMetaDataAllocationSize(addressE);

	FL_DELETE_RAW_ARRAY(wrapAllocator, addressC);
	FL_DELETE_RAW(wrapAllocator, addressE);

	const size_t highWatermark = allocSizeA + allocSizeC + allocSizeD + allocSizeE + 
		metaDataASize + metaDataCSize + metaDataDSize + metaDataESize;
	
	AllocationReport allocReport = wrapAllocator.GetCurrentAllocationsReport();

	ASSERT_EQ(allocReport.GetNumAllocationReportItems(), 2u);
	
	ASSERT_EQ(allocReport.GetAllocatorStats().CurrentTotalAllocSizeData.AllocSize, allocSizeA + allocSizeD);
	ASSERT_EQ(allocReport.GetAllocatorStats().CurrentTotalAllocSizeData.MetadataSize, metaDataASize + metaDataDSize);
	ASSERT_EQ(allocReport.GetAllocatorStats().CurrentTotalAllocSizeData.TrackingDataSize, DebugTrackingAllocatorWrapper::GetTrackingDataOverheadPerAllocation() * 2);
	
	ASSERT_EQ(allocReport.GetAllocatorStats().BytesAllocatedHighWatermark, highWatermark);

	auto reportItemA = allocReport.GetAllocationReportItem(0);
	ASSERT_EQ(reportItemA.LineNumber, allocALineNumber);
	ASSERT_EQ(reportItemA.AllocMemoryAddress, reinterpret_cast<intptr_t>(addressA));
	ASSERT_EQ(reportItemA.AllocSizeData.AllocSize, allocSizeA);
	ASSERT_EQ(reportItemA.AllocSizeData.MetadataSize, metaDataASize);
	ASSERT_EQ(reportItemA.AllocSizeData.TrackingDataSize, DebugTrackingAllocatorWrapper::GetTrackingDataOverheadPerAllocation());
	ASSERT_EQ(strncmp(reportItemA.FileName, fileName, strlen(fileName)), 0);
	ASSERT_EQ(strncmp(reportItemA.FunctionName, functionName, strlen(functionName)), 0);

	auto reportItemD = allocReport.GetAllocationReportItem(1);
	ASSERT_EQ(reportItemD.LineNumber, allocDLineNumber);
	ASSERT_EQ(reportItemD.AllocMemoryAddress, reinterpret_cast<intptr_t>(addressD));
	ASSERT_EQ(reportItemD.AllocSizeData.AllocSize, allocSizeD);
	ASSERT_EQ(reportItemD.AllocSizeData.MetadataSize, metaDataDSize);
	ASSERT_EQ(reportItemD.AllocSizeData.TrackingDataSize, DebugTrackingAllocatorWrapper::GetTrackingDataOverheadPerAllocation());
	ASSERT_EQ(strncmp(reportItemD.FileName, fileName, strlen(fileName)), 0);
	ASSERT_EQ(strncmp(reportItemD.FunctionName, functionName, strlen(functionName)), 0);

	auto reportCopy = allocReport;

	auto reportCopyItemA = allocReport.GetAllocationReportItem(0);
	ASSERT_EQ(reportCopyItemA.LineNumber, allocALineNumber);
	ASSERT_EQ(reportCopyItemA.AllocMemoryAddress, reinterpret_cast<intptr_t>(addressA));
	ASSERT_EQ(reportCopyItemA.AllocSizeData.AllocSize, allocSizeA);
	ASSERT_EQ(reportCopyItemA.AllocSizeData.MetadataSize, metaDataASize);
	ASSERT_EQ(reportCopyItemA.AllocSizeData.TrackingDataSize, DebugTrackingAllocatorWrapper::GetTrackingDataOverheadPerAllocation());
	ASSERT_EQ(strncmp(reportCopyItemA.FileName, fileName, strlen(fileName)), 0);
	ASSERT_EQ(strncmp(reportCopyItemA.FunctionName, functionName, strlen(functionName)), 0);

	auto reportCopyItemD = allocReport.GetAllocationReportItem(1);
	ASSERT_EQ(reportCopyItemD.LineNumber, allocDLineNumber);
	ASSERT_EQ(reportCopyItemD.AllocMemoryAddress, reinterpret_cast<intptr_t>(addressD));
	ASSERT_EQ(reportCopyItemD.AllocSizeData.AllocSize, allocSizeD);
	ASSERT_EQ(reportCopyItemD.AllocSizeData.MetadataSize, metaDataDSize);
	ASSERT_EQ(reportCopyItemD.AllocSizeData.TrackingDataSize, DebugTrackingAllocatorWrapper::GetTrackingDataOverheadPerAllocation());
	ASSERT_EQ(strncmp(reportCopyItemD.FileName, fileName, strlen(fileName)), 0);
	ASSERT_EQ(strncmp(reportCopyItemD.FunctionName, functionName, strlen(functionName)), 0);

	FL_DELETE_RAW(wrapAllocator, addressA);
	FL_DELETE_RAW(wrapAllocator, addressD);
}
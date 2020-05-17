#include "AssertMockHandler.h"
#include "Test.h"
#include <cstring>

using namespace Flourish::Debug;
using namespace Testing;

AssertMockHandler* AssertMockHandler::CurrentAssertTestData = nullptr;

AssertMockHandler::AssertMockHandler()
{
	CurrentAssertTestData = this;
	SetAssertHandler(AssertHandler);
}

AssertMockHandler::~AssertMockHandler()
{
	SetAssertHandler(nullptr);
	CurrentAssertTestData = nullptr;
}

 AssertResult AssertMockHandler::AssertHandler(
	SourceInfo sourceInfo,
	const char* condition,
	const char* message,
	const char* callStack)
{
	AssertMockHandler::CurrentAssertTestData->AssertCalled = true;
	strcpy_s(AssertMockHandler::CurrentAssertTestData->LastAssertConditionString, condition);
	strcpy_s(AssertMockHandler::CurrentAssertTestData->LastAssertMessageString, message);
	strcpy_s(AssertMockHandler::CurrentAssertTestData->LastAssertCallStackString, callStack);
	AssertMockHandler::CurrentAssertTestData->LastAssertSourceInfo = sourceInfo;
	return AssertResult::Ignore;
}

void AssertMockHandler::CheckCallStack() const
{
	char CompareCallStack[2048];
	GetStackTrace(2, CompareCallStack, 2048);

	EXPECT_STRING_EQUAL(CompareCallStack, LastAssertCallStackString);
}

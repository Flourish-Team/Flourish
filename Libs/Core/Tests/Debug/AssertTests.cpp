#include "Test.h"
#include "Debug/Assert.h"

#if FL_DISABLED(FL_PLATFORM_OSX)

using namespace Flourish::Debug;

// TODO: This should probably go somewhere else.
// Seems the strcpy_s that's templated is VS only?
// Also you don't use the return value so we're faking it
#if FL_ENABLED(FL_PLATFORM_OSX)
template <size_t size>
errno_t strcpy_s(char (&strDestination)[size], const char *strSource)
{
    strlcpy(strDestination, strSource, size);
    return 0;
}
#endif

class AssertTests : public ::testing::Test
{
public:
	AssertTests()
	{
		CurrentAssertTestData = this;
		SetAssertHandler(AssertHandler);
	}

	virtual ~AssertTests()
	{
		SetAssertHandler(nullptr);
		CurrentAssertTestData = nullptr;
	}

	static AssertTests* CurrentAssertTestData;

	static AssertResult AssertHandler(
		SourceInfo sourceInfo,
		const char* condition,
		const char* message,
		const char* callstack)
	{
		CurrentAssertTestData->AssertCalled = true;
		strcpy_s(CurrentAssertTestData->ConditionString, condition);
		strcpy_s(CurrentAssertTestData->MessageString, message);
		strcpy_s(CurrentAssertTestData->CallstackString, callstack);
		CurrentAssertTestData->SourceInfo = sourceInfo;
		return AssertResult::Ignore;
	}

	void CheckCallStack() const
	{
		char CompareCallStack[2048];
		GetStackTrace(2, CompareCallStack, 2048);

		EXPECT_STRING_EQUAL(CompareCallStack, CallstackString);
	}

	bool AssertCalled = false;
	char ConditionString[512];
	char MessageString[2048] ;
	char CallstackString[2048];

	

	SourceInfo SourceInfo;
};

AssertTests* AssertTests::CurrentAssertTestData = nullptr;

TEST_F(AssertTests, AssetNoMsgFiresCorrectly)
{
	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT(1 == 2);


	ASSERT_EQ(AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(ConditionString, "1 == 2");
	EXPECT_STRING_EQUAL(MessageString, "");


	CheckCallStack();

	EXPECT_STRING_EQUAL(CallstackString, CallstackString);

	EXPECT_EQ(SourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(SourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(SourceInfo.Line, sourceInfo.Line + 1);
}

TEST_F(AssertTests, AssetMsgFiresCorrectly)
{
	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_MSG(1 == 2, "This is a Test Message with Params: %i '%s'", 5, "ParamString");


	ASSERT_EQ(AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(ConditionString, "1 == 2");
	EXPECT_STRING_EQUAL(MessageString, "This is a Test Message with Params: 5 'ParamString'");


	CheckCallStack();

	EXPECT_STRING_EQUAL(CallstackString, CallstackString);

	EXPECT_EQ(SourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(SourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(SourceInfo.Line, sourceInfo.Line + 1);
}

TEST_F(AssertTests, AssetAlwaysNoMsgFiresCorrectly)
{
	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_ALWAYS();


	ASSERT_EQ(AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(ConditionString, "false");
	EXPECT_STRING_EQUAL(MessageString, "Always Fails");


	CheckCallStack();

	EXPECT_STRING_EQUAL(CallstackString, CallstackString);

	EXPECT_EQ(SourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(SourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(SourceInfo.Line, sourceInfo.Line + 1);
}

TEST_F(AssertTests, AssetAlwaysMsgFiresCorrectly)
{
	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_ALWAYS_MSG("This is a Test Message with Params: %i '%s'", 5, "ParamString");


	ASSERT_EQ(AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(ConditionString, "false");
	EXPECT_STRING_EQUAL(MessageString, "This is a Test Message with Params: 5 'ParamString'");


	CheckCallStack();

	EXPECT_STRING_EQUAL(CallstackString, CallstackString);

	EXPECT_EQ(SourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(SourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(SourceInfo.Line, sourceInfo.Line + 1);
}
#endif

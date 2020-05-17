#include "Test.h"
#include "Debug/Assert.h"
#include "AssertMockHandler.h"

using namespace Flourish::Debug::Testing;

TEST(AssertTests, AssetNoMsgFiresCorrectly)
{
	AssertMockHandler assertHandler;

	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT(1 == 2);


	ASSERT_EQ(assertHandler.AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(assertHandler.LastAssertConditionString, "1 == 2");
	EXPECT_STRING_EQUAL(assertHandler.LastAssertMessageString, "");

	assertHandler.CheckCallStack();

	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.Line, sourceInfo.Line + 1);
}

TEST(AssertTests, AssetMsgFiresCorrectly)
{
	AssertMockHandler assertHandler;

	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_MSG(1 == 2, "This is a Test Message with Params: %i '%s'", 5, "ParamString");


	ASSERT_EQ(assertHandler.AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(assertHandler.LastAssertConditionString, "1 == 2");
	EXPECT_STRING_EQUAL(assertHandler.LastAssertMessageString, "This is a Test Message with Params: 5 'ParamString'");


	assertHandler.CheckCallStack();

	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.Line, sourceInfo.Line + 1);
}

TEST(AssertTests, AssetAlwaysNoMsgFiresCorrectly)
{
	AssertMockHandler assertHandler;

	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_ALWAYS();


	ASSERT_EQ(assertHandler.AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(assertHandler.LastAssertConditionString, "false");
	EXPECT_STRING_EQUAL(assertHandler.LastAssertMessageString, "Always Fails");


	assertHandler.CheckCallStack();

	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.Line, sourceInfo.Line + 1);
}

TEST(AssertTests, AssetAlwaysMsgFiresCorrectly)
{
	AssertMockHandler assertHandler;

	//Generate a source info for this function for the comparason. (note line number +1 then one in this source)
	auto sourceInfo = MAKE_SOURCE_INFO;
	FL_ASSERT_ALWAYS_MSG("This is a Test Message with Params: %i '%s'", 5, "ParamString");


	ASSERT_EQ(assertHandler.AssertCalled, true);

	//Check assert data set correctly
	EXPECT_STRING_EQUAL(assertHandler.LastAssertConditionString, "false");
	EXPECT_STRING_EQUAL(assertHandler.LastAssertMessageString, "This is a Test Message with Params: 5 'ParamString'");


	assertHandler.CheckCallStack();

	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FileName, sourceInfo.FileName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.FunctionName, sourceInfo.FunctionName);
	EXPECT_EQ(assertHandler.LastAssertSourceInfo.Line, sourceInfo.Line + 1);
}

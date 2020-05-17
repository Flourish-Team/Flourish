#include "Debug/Assert.h"

namespace Flourish::Debug::Testing 
{

	//Helper class that overwrite the assert handler, causing them to always be ignored and allowing the user
	//to check the values set by the assert
	class AssertMockHandler
	{
	public:
		AssertMockHandler();

		~AssertMockHandler();

		static AssertResult AssertHandler(
			SourceInfo sourceInfo,
			const char* condition,
			const char* message,
			const char* callStack);

		void CheckCallStack() const;

		static AssertMockHandler* CurrentAssertTestData;

		bool AssertCalled = false;

		char LastAssertConditionString[512];
		char LastAssertMessageString[2048] ;
		char LastAssertCallStackString[2048];
		SourceInfo LastAssertSourceInfo;
	};
}
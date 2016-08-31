#include "../../Test.h"
#include "../../Test/Include/Test.h"

TEST(ExampleTests, EqualsTest)
{
	int a = 1;
	int b = 1;
	ASSERT_EQUAL(a, b) << "1 does not equal 1, I'd just give up if I were you";
}

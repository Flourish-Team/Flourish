#include "Test.h"
#include "FileSystem/FilePath.h"

using namespace Flourish;

TEST(FilePathTests, ConvertsDoubleBackslashToForwardSlash)
{
    FilePath filePath("Some\\path\\with\\backslashes\\");
    
    ASSERT_EQUAL(filePath.AsString(), "Some/path/with/backslashes/");
}

#include "Test.h"
#include "FileSystem/FilePath.h"

using namespace Flourish;

TEST(FilePathTests, ConvertsDoubleBackslashToForwardSlash)
{
    FilePath filePath("Some\\path\\with\\backslashes\\");
    
    EXPECT_EQUAL(filePath.AsString(), "Some/path/with/backslashes/");
}

TEST(FilePathTests, CanGetDirectory)
{
    FilePath filePath("path/to/file.txt");
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), "path/to");
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), "path");
}

TEST(FilePathTests, GetDirectoryOfRootFolderEmptyPath)
{
    FilePath filePath("path");
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), "");
}

TEST(FilePathTests, GetFileNameReturnsFileNameAndExtension)
{
    FilePath justAFile("someFile.txt");
    
    EXPECT_EQUAL(justAFile.GetFileName().AsString(), "someFile.txt");
    
    FilePath dirAndFile("some/path/toAFile.txt");
    
    EXPECT_EQUAL(dirAndFile.GetFileName().AsString(), "toAFile.txt");
    
    FilePath justADir("some/path/");
    
    EXPECT_EQUAL(justADir.GetFileName().AsString(), "");
    
    FilePath noExtension("some/path/toAFileWithoutExtension");
    
    EXPECT_EQUAL(noExtension.GetFileName().AsString(), "toAFileWithoutExtension");
}

#include "Test.h"
#include "FileSystem/FilePath.h"

#include <cstdarg>

using namespace Flourish;

class FilePathTests : public testing::TestWithParam<std::string> {
public:
    
    std::string MakePathString(std::string str)
    {
        return str;
    }
    
    template<typename... Args>
    std::string MakePathString(std::string strPart, Args... args)
    {
        return strPart + GetParam() + MakePathString(args...);
    }
};

TEST_P(FilePathTests, CanGetDirectory)
{
    FilePath filePath(MakePathString("path", "to", "file.txt"));
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), MakePathString("path", "to"));
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), "path");
}

TEST_P(FilePathTests, GetDirectoryOfRootFolderEmptyPath)
{
    FilePath filePath("path");
    
    EXPECT_EQUAL(filePath.GetDirectory().AsString(), "");
}

TEST_P(FilePathTests, GetFileNameReturnsFileNameAndExtension)
{
    FilePath justAFile("someFile.txt");
    
    EXPECT_EQUAL(justAFile.GetFileName().AsString(), "someFile.txt");
    
    FilePath dirAndFile(MakePathString("some", "path", "toAFile.txt"));
    
    EXPECT_EQUAL(dirAndFile.GetFileName().AsString(), "toAFile.txt");
    
    FilePath justADir(MakePathString("some", "path", ""));
    
    EXPECT_EQUAL(justADir.GetFileName().AsString(), "");
    
    FilePath noExtension(MakePathString("some", "path", "toAFileWithoutExtension"));
    
    EXPECT_EQUAL(noExtension.GetFileName().AsString(), "toAFileWithoutExtension");
}

TEST_P(FilePathTests, GetFileNameWithoutExtensionReturnsOnlyFileName)
{
    FilePath justAFile("someFile.txt");
    
    EXPECT_EQUAL(justAFile.GetFileNameWithoutExtension().AsString(), "someFile");
    
    FilePath dirAndFile(MakePathString("some", "path", "toAFile.txt"));
    
    EXPECT_EQUAL(dirAndFile.GetFileNameWithoutExtension().AsString(), "toAFile");
    
    FilePath justADir(MakePathString("some", "path", ""));
    
    EXPECT_EQUAL(justADir.GetFileNameWithoutExtension().AsString(), "");
    
    FilePath noExtension(MakePathString("some", "path", "toAFileWithoutExtension"));
    
    EXPECT_EQUAL(noExtension.GetFileNameWithoutExtension().AsString(), "toAFileWithoutExtension");
}

std::string dirSeperators[] { "/", "\\" };
INSTANTIATE_TEST_CASE_P(ForwardAndBackSlashSeperators, FilePathTests, testing::ValuesIn(dirSeperators));

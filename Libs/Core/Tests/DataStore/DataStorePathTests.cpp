#include "Test.h"
#include "DataStore/DataStorePath.h"

#include <cstdarg>

using namespace Flourish;

std::string MakePathString(std::string str)
{
    return str;
}

template<typename... Args>
std::string MakePathString(std::string strPart, Args... args)
{
    return strPart + '/' + MakePathString(args...);
}

TEST(DataStorePathTests, CanGetDirectory)
{
    DataStorePath path(MakePathString("path", "to", "file.txt"));

    auto fileDir = path.GetDirectory();
        EXPECT_EQUAL(fileDir.AsString(), MakePathString("path", "to"));

        EXPECT_EQUAL(fileDir.GetDirectory().AsString(), "path");
}

TEST(DataStorePathTests, GetDirectoryOfRootFolderEmptyPath)
{
    DataStorePath path("path");

        EXPECT_EQUAL(path.GetDirectory().AsString(), "");
}

TEST(DataStorePathTests, GetFileNameReturnsFileNameAndExtension)
{
    DataStorePath justAFile("someFile.txt");

        EXPECT_EQUAL(justAFile.GetFileName().AsString(), "someFile.txt");

    DataStorePath dirAndFile(MakePathString("some", "path", "toAFile.txt"));

        EXPECT_EQUAL(dirAndFile.GetFileName().AsString(), "toAFile.txt");

    DataStorePath justADir(MakePathString("some", "path", ""));

        EXPECT_EQUAL(justADir.GetFileName().AsString(), "");

    DataStorePath noExtension(MakePathString("some", "path", "toAFileWithoutExtension"));

        EXPECT_EQUAL(noExtension.GetFileName().AsString(), "toAFileWithoutExtension");
}

TEST(DataStorePathTests, GetFileNameWithoutExtensionReturnsOnlyFileName)
{
    DataStorePath justAFile("someFile.txt");

        EXPECT_EQUAL(justAFile.GetFileNameWithoutExtension().AsString(), "someFile");

    DataStorePath dirAndFile(MakePathString("some", "path", "toAFile.txt"));

        EXPECT_EQUAL(dirAndFile.GetFileNameWithoutExtension().AsString(), "toAFile");

    DataStorePath justADir(MakePathString("some", "path", ""));

        EXPECT_EQUAL(justADir.GetFileNameWithoutExtension().AsString(), "");

    DataStorePath noExtension(MakePathString("some", "path", "toAFileWithoutExtension"));

        EXPECT_EQUAL(noExtension.GetFileNameWithoutExtension().AsString(), "toAFileWithoutExtension");
}

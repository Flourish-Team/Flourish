#include "Test.h"
#include "DataStore/MemoryDataStore.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/DataStoreReadStream.h"

#include <algorithm>
#include <future>

using namespace Flourish;


class MemoryDataStoreTests : public ::testing::Test
{
protected:
    MemoryDataStore dataStore;

    void SetUp() override
    {
        SetupCallWait();
    }

    void SetupCallWait()
    {
        promise = std::promise<void>();
        future = promise.get_future();
    }

    void TriggerCallComplete()
    {
        promise.set_value();
    }

    void ExpectCallToCompleteInTime()
    {
        auto result = future.wait_for(std::chrono::seconds(1));
            EXPECT_EQUAL(result, std::future_status::ready) << "Callback was not called in time";
    }

    void WriteDataToPath(const DataStorePath& path)
    {
        SetupCallWait();
        dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam)
        {
            TriggerCallComplete();
        });
        ExpectCallToCompleteInTime();
        SetupCallWait();
    }

private:
    std::promise<void> promise;
    std::future<void> future;
};

TEST_F(MemoryDataStoreTests, ExistsReturnsFalseWithNoDataOrDirectory)
{
    EXPECT_FALSE(dataStore.Exists(DataStorePath("does/not/exist")));
}

TEST_F(MemoryDataStoreTests, ExistsReturnsTrueAfterDataWrite)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_TRUE(dataStore.Exists(path));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, IsDirReturnsFalseForNonExistantPath)
{
    EXPECT_FALSE(dataStore.IsDir(DataStorePath("does/not/exist")));
}

TEST_F(MemoryDataStoreTests, IsDataReturnsFalseForNonExistantPath)
{
    EXPECT_FALSE(dataStore.IsData(DataStorePath("does/not/exist")));
}

TEST_F(MemoryDataStoreTests, IsDirReturnsFalseForData)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_FALSE(dataStore.IsDir(path));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, IsDataReturnsTrueForData)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_TRUE(dataStore.IsData(path));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, OpenForReadOnNonExistantPathCreatesError)
{
    DataStorePath path("does/not/exist");

    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam result)
    {
        EXPECT_TRUE(result.HasError());
            EXPECT_STRING_EQUAL(result.GetError(), "Path 'does/not/exist' does not exist in data store");
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, CanReadBackWrittenData)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        EXPECT_FALSE(openResult.HasError());
        auto dataToWrite = std::string("some data");
        openResult.Value()->Write(dataToWrite.c_str(), dataToWrite.length());
        openResult.Value()->Flush(
            [&](DataStoreWriteCallbackParam writeResult)
            {
                EXPECT_FALSE(writeResult.HasError());
                TriggerCallComplete();
            });
    });

    ExpectCallToCompleteInTime();
    SetupCallWait();

    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL(static_cast<const char*>(readResult.Value()->Data()), "some data");
        TriggerCallComplete();
    });
    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, WritingDataCreatesParentDirectories)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_TRUE(dataStore.IsDir(DataStorePath("")));
        EXPECT_TRUE(dataStore.IsDir(DataStorePath("will")));
        EXPECT_TRUE(dataStore.IsDir(DataStorePath("will/be")));
        EXPECT_TRUE(dataStore.IsDir(DataStorePath("will/be/written")));
        EXPECT_FALSE(result.HasError());
        EXPECT_FALSE(dataStore.IsDir(path));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, EnumerateFindsDataAndDirectories)
{
    WriteDataToPath(DataStorePath("some/path/to/file.txt"));
    WriteDataToPath(DataStorePath("some/path/to/another_file.txt"));
    WriteDataToPath(DataStorePath("some/path/to/subdir/with_file.txt"));

    std::vector<DataStorePath> result;
    dataStore.Enumerate(DataStorePath("some/path/to"), result);

    // We don't care what order things are in, so just assert that
    // all the expected elements (and only those) exist
    EXPECT_EQUAL(result.size(), 3);
    EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/file.txt")), result.end());
    EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/another_file.txt")), result.end());
    EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/subdir")), result.end());
}

TEST_F(MemoryDataStoreTests, MultipleWrites)
{
    TEST_NOT_IMPLEMENTED;
}

TEST_F(MemoryDataStoreTests, MultipleReads)
{
    TEST_NOT_IMPLEMENTED;
}

TEST_F(MemoryDataStoreTests, MultipleReadStreamsSamePath)
{
    TEST_NOT_IMPLEMENTED;
}

TEST_F(MemoryDataStoreTests, MultipleWriteStreamsSamePath)
{
    TEST_NOT_IMPLEMENTED;
}

TEST_F(MemoryDataStoreTests, CloseReadStream)
{
    TEST_NOT_IMPLEMENTED;
}

TEST_F(MemoryDataStoreTests, CloseWriteStream)
{
    TEST_NOT_IMPLEMENTED;
}
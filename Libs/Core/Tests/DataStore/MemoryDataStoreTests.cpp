#include "Test.h"
#include "DataStore/MemoryDataStore.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/DataStoreReadStream.h"

#include <future>

using namespace Flourish;

TEST(MemoryDataStoreTests, ExistsReturnsFalseWithNoDataOrDirectory)
{
    MemoryDataStore dataStore;

    EXPECT_FALSE(dataStore.Exists(DataStorePath("should/not/exist")));
}

TEST(MemoryDataStoreTests, ExistsReturnsTrueAfterDataWrite)
{
    MemoryDataStore dataStore;
    DataStorePath path("will/be/written/to");

    std::promise<void> promise;
    auto future = promise.get_future();

    dataStore.OpenForWrite(path, [&](Error<DataStoreWriteStream*> result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_TRUE(dataStore.Exists(path));
        promise.set_value();
    });

    auto result = future.wait_for(std::chrono::seconds(1));
        EXPECT_EQUAL(result, std::future_status::ready) << "Callback was not called in time";
}

TEST(MemoryDataStoreTests, CanReadBackWrittenData)
{
    MemoryDataStore dataStore;
    DataStorePath path("will/be/written/to");

    std::promise<void> promise;
    auto future = promise.get_future();

    dataStore.OpenForWrite(path, [&](Error<DataStoreWriteStream*> openResult)
    {
        EXPECT_FALSE(openResult.HasError());
        auto dataToWrite = std::string("some data");
        openResult.Value()->Write(dataToWrite.c_str(), dataToWrite.length());
        openResult.Value()->Flush([&](Error<DataStoreWriteStream*> writeResult)
                                  {
                                      EXPECT_FALSE(writeResult.HasError());
                                      dataStore.OpenForRead(path, [&](Error<DataStoreReadStream*> readResult)
                                      {
                                              EXPECT_STRING_EQUAL(static_cast<const char*>(readResult.Value()->Data()), "some data");
                                          promise.set_value();
                                      });
                                  });
    });

    auto result = future.wait_for(std::chrono::seconds(1));
        EXPECT_EQUAL(result, std::future_status::ready) << "Callback was not called in time";
}

TEST(MemoryDataStoreTests, MultipleWrites)
{
    TEST_NOT_IMPLEMENTED;
}

TEST(MemoryDataStoreTests, MultipleReads)
{
    TEST_NOT_IMPLEMENTED;
}

TEST(MemoryDataStoreTests, MultipleReadStreamsSamePath)
{
    TEST_NOT_IMPLEMENTED;
}

TEST(MemoryDataStoreTests, MultipleWriteStreamsSamePath)
{
    TEST_NOT_IMPLEMENTED;
}

TEST(MemoryDataStoreTests, CloseReadStream)
{
    TEST_NOT_IMPLEMENTED;
}

TEST(MemoryDataStoreTests, CloseWriteStream)
{
    TEST_NOT_IMPLEMENTED;
}
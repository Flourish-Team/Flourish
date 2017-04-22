#include "Test.h"
#include "DataStore/MemoryDataStore.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/DataStoreReadStream.h"

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

TEST_F(MemoryDataStoreTests, SequentialWritesAppend)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      std::string secondData(" second data");
                                      writeResult.Value()->Write(secondData.c_str(), secondData.length());
                                      writeResult.Value()->Flush([&](DataStoreWriteCallbackParam secondWriteResult)
                                                                 {
                                                                     TriggerCallComplete();
                                                                 });
                                  });
    });

    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("first data second data", static_cast<const char*>(readResult.Value()->Data()));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, ConsecutiveWritesOverwrite)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      TriggerCallComplete();
                                  });
    });

    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string secondData("second data");
        openResult.Value()->Write(secondData.c_str(), secondData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      TriggerCallComplete();
                                  });
    });

    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("second data", static_cast<const char*>(readResult.Value()->Data()));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, ConsecutiveAppendsAppend)
{
    DataStorePath path("will/be/written/to");

    dataStore.OpenForAppend(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      TriggerCallComplete();
                                  });
    });

    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForAppend(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData(" second data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      TriggerCallComplete();
                                  });
    });

    SetupCallWait();
    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("first data second data", static_cast<const char*>(readResult.Value()->Data()));
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, ConsecutiveReadsReadSameData)
{
    // Note that this test will fail if a read buffer is smaller than
    // the write buffer
    DataStorePath path("will/be/written/to");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        // Write a buffer full of 1's
        const auto openStream = openResult.Value();
        auto data = new uint8_t[openStream->Available()];
        std::fill_n(data, openStream->Available(), 1);
        openStream->Write(data, openStream->Available());
        delete[] data;
        openStream->Flush([&](DataStoreWriteCallbackParam writeResult)
                          {
                              // Write a buffer full of 2's
                              const auto writeStream = writeResult.Value();
                              auto data = new uint8_t[writeStream->Available()];
                              std::fill_n(data, writeStream->Available(), 2);
                              writeStream->Write(data, writeStream->Available());
                              delete[] data;
                              writeStream->Flush([&](DataStoreWriteCallbackParam)
                                                 {
                                                     TriggerCallComplete();
                                                 });
                          });
    });

    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        const auto openStream = openResult.Value();
        const auto openData = static_cast<const uint8_t*>(openStream->Data());
            EXPECT_EQUAL(openData[0], 1);
        TriggerCallComplete();
    });
    ExpectCallToCompleteInTime();

    SetupCallWait();
    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        const auto openStream = openResult.Value();
        const auto openData = static_cast<const uint8_t*>(openStream->Data());
            EXPECT_EQUAL(openData[0], 1);
        TriggerCallComplete();
    });
    ExpectCallToCompleteInTime();
}


TEST_F(MemoryDataStoreTests, ReadToEndOfDataSetsFlag)
{
    DataStorePath path("will/be/written/to");

    WriteDataToPath(path);

    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam openResult){
        EXPECT_TRUE(openResult.Value()->EndOfData());
        TriggerCallComplete();
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, RefreshOnEndOfDataStreamCreatesError)
{
    DataStorePath path("will/be/written/to");

    WriteDataToPath(path);

    dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam openResult){
        EXPECT_TRUE(openResult.Value()->EndOfData());
        openResult.Value()->Refresh([&](DataStoreReadCallbackParam readResult){
            EXPECT_TRUE(readResult.HasError());
                EXPECT_STRING_EQUAL(readResult.GetError(), "Attempted to read past end of stream  (Path: 'will/be/written/to')");
            TriggerCallComplete();
        });
    });

    ExpectCallToCompleteInTime();
}

TEST_F(MemoryDataStoreTests, OpenMultipleStreamsToSamePathCreatesError)
{
    DataStorePath path("some/path");

    dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam){
       dataStore.OpenForRead(path, [&](DataStoreReadCallbackParam read)
        {
            EXPECT_TRUE(read.HasError());
                EXPECT_STRING_EQUAL(read.GetError(), "Path: 'some/path' is already open in another stream");

            dataStore.OpenForAppend(path, [&](DataStoreWriteCallbackParam append)
            {
                EXPECT_TRUE(append.HasError());
                    EXPECT_STRING_EQUAL(append.GetError(), "Path: 'some/path' is already open in another stream");

                dataStore.OpenForWrite(path, [&](DataStoreWriteCallbackParam secondWrite){
                    EXPECT_TRUE(secondWrite.HasError());
                        EXPECT_STRING_EQUAL(secondWrite.GetError(), "Path: 'some/path' is already open in another stream");
                    TriggerCallComplete();
                });
            });
        });
    });

    ExpectCallToCompleteInTime();
}
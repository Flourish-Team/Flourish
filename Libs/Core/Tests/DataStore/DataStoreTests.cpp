#include "Test.h"
#include "DataStore/DataStoreReadStream.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/FileSystem.h"
#include "DataStore/LocalFileDataStore.h"
#include "DataStore/MemoryDataStore.h"

#include <future>

using namespace Flourish;

template<typename T>
IWritableDataStore* CreateDataStore();
template<typename T>
void DestroyDataStore(IWritableDataStore* dataStore);

template<>
IWritableDataStore* CreateDataStore<MemoryDataStore>()
{
    return new MemoryDataStore();
}

template<>
void DestroyDataStore<MemoryDataStore>(IWritableDataStore* dataStore)
{
}

template<>
IWritableDataStore* CreateDataStore<LocalFileDataStore>()
{
    FileSystem::CreateDirectoryTree("temp/test/path");
    return new LocalFileDataStore("temp/test/path");
}

template<>
void DestroyDataStore<LocalFileDataStore>(IWritableDataStore* dataStore)
{
    FileSystem::DeleteDirectory("temp");
}

template<typename T>
class DataStoreTests : public ::testing::Test
{
public:
    DataStoreTests()
        : dataStore(CreateDataStore<T>())
    {
        this->SetupCallWait();
    }

    virtual ~DataStoreTests()
    {
        DestroyDataStore<T>(dataStore);
        delete dataStore;
    }

protected:
    IWritableDataStore* dataStore;

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
        auto result = future.wait_for(std::chrono::milliseconds(500000));
            EXPECT_EQUAL(result, std::future_status::ready) << "Callback was not called in time";
    }

    void WriteDataToPath(const DataStorePath& path)
    {
        SetupCallWait();
        dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam)
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

typedef ::testing::Types<LocalFileDataStore> Implementations;

TYPED_TEST_CASE(DataStoreTests, Implementations);

TYPED_TEST(DataStoreTests, ExistsReturnsFalseWithNoDataOrDirectory)
{
    EXPECT_FALSE(this->dataStore->Exists(DataStorePath("does/not/exist")));
}


TYPED_TEST(DataStoreTests, ExistsReturnsTrueAfterDataWrite)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_TRUE(this->dataStore->Exists(path));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, IsDirReturnsFalseForNonExistantPath)
{
    EXPECT_FALSE(this->dataStore->IsDir(DataStorePath("does/not/exist")));
}

TYPED_TEST(DataStoreTests, IsDataReturnsFalseForNonExistantPath)
{
    EXPECT_FALSE(this->dataStore->IsData(DataStorePath("does/not/exist")));
}

TYPED_TEST(DataStoreTests, IsDirReturnsFalseForData)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_FALSE(this->dataStore->IsDir(path));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, IsDataReturnsTrueForData)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_FALSE(result.HasError());
        EXPECT_TRUE(this->dataStore->IsData(path));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, OpenForReadOnNonExistantPathCreatesError)
{
    DataStorePath path("does/not/exist");

    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam result)
    {
        EXPECT_TRUE(result.HasError());
            EXPECT_STRING_EQUAL(result.GetError(), "Path 'does/not/exist' does not exist in data store");
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, CanReadBackWrittenData)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        EXPECT_FALSE(openResult.HasError());
        auto dataToWrite = std::string("some data");
        openResult.Value()->Write(dataToWrite.c_str(), dataToWrite.length());
        openResult.Value()->Flush(
            [&](DataStoreWriteCallbackParam writeResult)
            {
                EXPECT_FALSE(writeResult.HasError());
                this->dataStore->Close(writeResult.Value().get());
                this->TriggerCallComplete();
            });
    });

    this->ExpectCallToCompleteInTime();
    this->SetupCallWait();

    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL(static_cast<const char*>(readResult.Value()->Data()), "some data");
        this->TriggerCallComplete();
    });
    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, WritingDataCreatesParentDirectories)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam result)
    {
        EXPECT_TRUE(this->dataStore->IsDir(DataStorePath("")));
        EXPECT_TRUE(this->dataStore->IsDir(DataStorePath("will")));
        EXPECT_TRUE(this->dataStore->IsDir(DataStorePath("will/be")));
        EXPECT_TRUE(this->dataStore->IsDir(DataStorePath("will/be/written")));
        EXPECT_FALSE(result.HasError());
        EXPECT_FALSE(this->dataStore->IsDir(path));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, EnumerateFindsDataAndDirectories)
{
    this->WriteDataToPath(DataStorePath("some/path/to/file.txt"));
    this->WriteDataToPath(DataStorePath("some/path/to/another_file.txt"));
    this->WriteDataToPath(DataStorePath("some/path/to/subdir/with_file.txt"));

    std::vector<DataStorePath> result;
    this->dataStore->Enumerate(DataStorePath("some/path/to"), result);

    // We don't care what order things are in, so just assert that
    // all the expected elements (and only those) exist
        EXPECT_EQUAL(result.size(), 3);
        EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/file.txt")), result.end());
        EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/another_file.txt")), result.end());
        EXPECT_NOT_EQUAL(std::find(result.begin(), result.end(), DataStorePath("some/path/to/subdir")), result.end());
}

TYPED_TEST(DataStoreTests, SequentialWritesAppend)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      std::string secondData(" second data");
                                      writeResult.Value()->Write(secondData.c_str(), secondData.length());
                                      writeResult.Value()->Flush([&](DataStoreWriteCallbackParam secondWriteResult)
                                                                 {
                                                                     this->TriggerCallComplete();
                                                                 });
                                  });
    });

    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("first data second data", static_cast<const char*>(readResult.Value()->Data()));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, ConsecutiveWritesOverwrite)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      this->TriggerCallComplete();
                                  });
    });

    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string secondData("second data");
        openResult.Value()->Write(secondData.c_str(), secondData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      this->TriggerCallComplete();
                                  });
    });

    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("second data", static_cast<const char*>(readResult.Value()->Data()));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, ConsecutiveAppendsAppend)
{
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForAppend(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData("first data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      this->TriggerCallComplete();
                                  });
    });

    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForAppend(path, [&](DataStoreWriteCallbackParam openResult)
    {
        std::string firstData(" second data");
        openResult.Value()->Write(firstData.c_str(), firstData.length());
        openResult.Value()->Flush([&](DataStoreWriteCallbackParam writeResult)
                                  {
                                      this->TriggerCallComplete();
                                  });
    });

    this->SetupCallWait();
    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam readResult)
    {
            EXPECT_STRING_EQUAL("first data second data", static_cast<const char*>(readResult.Value()->Data()));
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, ConsecutiveReadsReadSameData)
{
    // Note that this test will fail if a read buffer is smaller than
    // the write buffer
    DataStorePath path("will/be/written/to");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam openResult)
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
                                                     this->TriggerCallComplete();
                                                 });
                          });
    });

    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        const auto openStream = openResult.Value();
        const auto openData = static_cast<const uint8_t*>(openStream->Data());
            EXPECT_EQUAL(openData[0], 1);
        this->TriggerCallComplete();
    });
    this->ExpectCallToCompleteInTime();

    this->SetupCallWait();
    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        const auto openStream = openResult.Value();
        const auto openData = static_cast<const uint8_t*>(openStream->Data());
            EXPECT_EQUAL(openData[0], 1);
        this->TriggerCallComplete();
    });
    this->ExpectCallToCompleteInTime();
}


TYPED_TEST(DataStoreTests, ReadToEndOfDataSetsFlag)
{
    DataStorePath path("will/be/written/to");

    this->WriteDataToPath(path);

    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        EXPECT_TRUE(openResult.Value()->EndOfData());
        this->TriggerCallComplete();
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, RefreshOnEndOfDataStreamCreatesError)
{
    DataStorePath path("will/be/written/to");

    this->WriteDataToPath(path);

    this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam openResult)
    {
        EXPECT_TRUE(openResult.Value()->EndOfData());
        openResult.Value()->Refresh([&](DataStoreReadCallbackParam readResult)
                                    {
                                        EXPECT_TRUE(readResult.HasError());
                                            EXPECT_STRING_EQUAL(readResult.GetError(), "Attempted to read past end of stream  (Path: 'will/be/written/to')");
                                        this->TriggerCallComplete();
                                    });
    });

    this->ExpectCallToCompleteInTime();
}

TYPED_TEST(DataStoreTests, OpenMultipleStreamsToSamePathCreatesError)
{
    DataStorePath path("some/path");

    this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam)
    {
        this->dataStore->OpenForRead(path, [&](DataStoreReadCallbackParam read)
        {
            EXPECT_TRUE(read.HasError());
                EXPECT_STRING_EQUAL(read.GetError(), "Path: 'some/path' is already open in another stream");

            this->dataStore->OpenForAppend(path, [&](DataStoreWriteCallbackParam append)
            {
                EXPECT_TRUE(append.HasError());
                    EXPECT_STRING_EQUAL(append.GetError(), "Path: 'some/path' is already open in another stream");

                this->dataStore->OpenForWrite(path, [&](DataStoreWriteCallbackParam secondWrite)
                {
                    EXPECT_TRUE(secondWrite.HasError());
                        EXPECT_STRING_EQUAL(secondWrite.GetError(), "Path: 'some/path' is already open in another stream");
                    this->TriggerCallComplete();
                });
            });
        });
    });

    this->ExpectCallToCompleteInTime();
}
#include "Test.h"

#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/IWritableDataStore.h"

using namespace Flourish;

class MockWriteDataStore : public IWritableDataStore
{
public:

    DataBuffer* bufferToWrite;

    void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) override
    {
    }

    void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override
    {
    }

    bool IsDir(const DataStorePath& path) const override
    {
        return false;
    }

    bool IsData(const DataStorePath& path) const override
    {
        return false;
    }

protected:
    void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override
    {
    }

private:
    void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) override
    {
        bufferToWrite = buffer;
    }
};


TEST(DataStoreWriteStreamTests, HasPath)
{
    DataStoreWriteStream stream(nullptr, DataStorePath("some/path"));

        EXPECT_EQUAL(stream.Path(), DataStorePath("some/path"));
}

TEST(DataStoreWriteStreamTests, WritingDataReducesSpaceLeft)
{
    DataStoreWriteStream stream(nullptr, DataStorePath("some/path"));

    auto prevAvailable = stream.Available();

    auto data = "some data";
    stream.Write(data, strlen(data));

        EXPECT_LESS_THAN(stream.Available(), prevAvailable);
}

TEST(DataStoreWriteStreamTests, FlushWritesDataToStore)
{
    MockWriteDataStore dataStore;
    DataStoreWriteStream stream(&dataStore, DataStorePath("some/path"));

    auto data = "some data";
    stream.Write(data, strlen(data));

    stream.Flush([](Error<DataStoreWriteStream*>)
                 {});

        ASSERT_NOT_EQUAL(dataStore.bufferToWrite, nullptr);
        EXPECT_STRING_EQUAL(data, static_cast<const char*>(dataStore.bufferToWrite->Data()));
}
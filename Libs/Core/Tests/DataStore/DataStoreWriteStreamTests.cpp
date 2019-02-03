#include "Test.h"

#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/IWritableDataStore.h"

using namespace Flourish;

class MockWriteDataStore : public IWritableDataStore
{
public:
    DataBuffer* bufferToWrite;
    DataStoreWriteStream* lastClosedStream;

    MockWriteDataStore()
        : bufferToWrite(nullptr)
        , lastClosedStream(nullptr)
    {

    }

    bool Exists(const DataStorePath& path) const override
    {
		FL_UNUSED(path)
        return false;
    }

    void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) override
    {
		FL_UNUSED(path)
		FL_UNUSED(callback)
    }

    void Close(DataStoreReadStream* stream) override
    {
		FL_UNUSED(stream)
    }

    void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override
    {
		FL_UNUSED(path)
		FL_UNUSED(callback)
    }

    void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) override
    {
		FL_UNUSED(path)
		FL_UNUSED(callback)
    }

    void Close(DataStoreWriteStream* stream) override
    {
        lastClosedStream = stream;
    }

    bool IsDir(const DataStorePath& path) const override
    {
		FL_UNUSED(path)
        return false;
    }

    bool IsData(const DataStorePath& path) const override
    {
		FL_UNUSED(path)
        return false;
    }

    void Enumerate(const DataStorePath& path, std::vector<DataStorePath>& entries) const override
    {
		FL_UNUSED(path)
		FL_UNUSED(entries)
    }

protected:
    void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override
    {
		FL_UNUSED(stream)
		FL_UNUSED(buffer)
		FL_UNUSED(callback)
    }

private:
    void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) override
    {
		FL_UNUSED(stream)
		FL_UNUSED(callback)

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

    stream.Flush([](DataStoreWriteCallbackParam) {});

        ASSERT_NOT_EQUAL(dataStore.bufferToWrite, nullptr);
        EXPECT_STRING_EQUAL(data, static_cast<const char*>(dataStore.bufferToWrite->ReadData()));
}

TEST(DataStoreWriteStreamTests, ClosesStreamOnDestroy)
{
    MockWriteDataStore dataStore;
    {
        DataStoreWriteStream stream(&dataStore, DataStorePath("some/path"));
    }
        EXPECT_NOT_EQUAL(dataStore.lastClosedStream, nullptr);
}

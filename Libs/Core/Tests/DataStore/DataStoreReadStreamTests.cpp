#include "Test.h"

#include "DataStore/DataStorePath.h"
#include "DataStore/DataStoreReadStream.h"
#include "DataStore/IReadableDataStore.h"

using namespace Flourish;

class MockReadDataStore : public IReadableDataStore
{
public:
    DataBuffer* bufferToFill;
    DataStoreReadStream* lastClosedStream;

    MockReadDataStore()
        : bufferToFill(nullptr)
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
		FL_UNUSED(callback)

        bufferToFill = buffer;
    }

};

TEST(DataStoreReadStreamTests, HasPath)
{
    DataStoreReadStream stream(nullptr, DataStorePath("some/path"), DataBuffer(0));

        EXPECT_EQUAL(DataStorePath("some/path"), stream.Path());
}

TEST(DataStoreReadStreamTests, CanReadFromInitialData)
{
    auto data = "some data";
	size_t dataLen = strlen(data) + 1; //Include null terminator

    DataBuffer buffer(dataLen);
    buffer.Write(data, dataLen);
    DataStoreReadStream stream(nullptr, DataStorePath("some/path"), buffer);

        EXPECT_EQUAL(dataLen, stream.Available());
        EXPECT_STRING_EQUAL(data, static_cast<const char*>(stream.Data()));
}


TEST(DataStoreReadStreamTests, RequestsMoreDataOnRefresh)
{
    DataBuffer buffer(20);
    MockReadDataStore dataStore;
    DataStoreReadStream stream(&dataStore, DataStorePath("some/path"), buffer);

    stream.Refresh([](DataStoreReadCallbackParam result){});

        EXPECT_NOT_EQUAL(nullptr, dataStore.bufferToFill);
}

TEST(DataStoreReadStreamTests, ConsumeMovesReadHead)
{
    auto data = "some data";
	size_t dataLen = strlen(data) + 1; //Include null terminator

    DataBuffer buffer(dataLen);
    buffer.Write(data, dataLen);
    DataStoreReadStream stream(nullptr, DataStorePath("some/path"), buffer);

    stream.Consume(strlen("some "));

    EXPECT_EQUAL(strlen("data") + 1, stream.Available());
    EXPECT_STRING_EQUAL("data", static_cast<const char*>(stream.Data()));
}

TEST(DataStoreReadStreamTests, ClosesStreamOnDestroy)
{
    MockReadDataStore dataStore;
    {
        DataStoreReadStream stream(&dataStore, DataStorePath("some/path"), DataBuffer(32));
    }
        EXPECT_NOT_EQUAL(dataStore.lastClosedStream, nullptr);
}

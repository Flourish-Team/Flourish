#include "Test.h"

#include "DataStore/DataStorePath.h"
#include "DataStore/DataStoreReadStream.h"
#include "DataStore/IReadableDataStore.h"

using namespace Flourish;

class MockReadDataStore : public IReadableDataStore
{
public:
    DataBuffer* bufferToFill;

    MockReadDataStore()
        : bufferToFill(nullptr)
    {

    }


    void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) override
    {

    }

protected:
    void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override
    {
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
    DataBuffer buffer(strlen(data));
    buffer.Write(data, strlen(data));
    DataStoreReadStream stream(nullptr, DataStorePath("some/path"), buffer);

        EXPECT_EQUAL(strlen(data), stream.Available());
        EXPECT_STRING_EQUAL(data, static_cast<const char*>(stream.Data()));
}


TEST(DataStoreReadStreamTests, RequestsMoreDataOnRefresh)
{
    DataBuffer buffer(20);
    MockReadDataStore dataStore;
    DataStoreReadStream stream(&dataStore, DataStorePath("some/path"), buffer);

    stream.Refresh([](Error<DataStoreReadStream*> result){});

        EXPECT_NOT_EQUAL(nullptr, dataStore.bufferToFill);
}

TEST(DataStoreReadStreamTests, ConsumeMovesReadHead)
{
    auto data = "some data";
    DataBuffer buffer(strlen(data));
    buffer.Write(data, strlen(data));
    DataStoreReadStream stream(nullptr, DataStorePath("some/path"), buffer);

    stream.Consume(strlen("some "));

    EXPECT_EQUAL(strlen("data"), stream.Available());
    EXPECT_STRING_EQUAL("data", static_cast<const char*>(stream.Data()));
}

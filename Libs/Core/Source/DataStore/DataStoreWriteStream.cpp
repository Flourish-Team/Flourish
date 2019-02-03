#include <utility>
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/IWritableDataStore.h"

namespace Flourish
{
    DataStoreWriteStream::DataStoreWriteStream(IWritableDataStore* dataStore, DataStorePath path)
        : _dataStore(dataStore)
        , _path(std::move(path))
        , _buffer(1024)
    {
    }

    DataStoreWriteStream::~DataStoreWriteStream()
    {
        if(_dataStore != nullptr)
        {
            _dataStore->Close(this);
        }
    }

    size_t DataStoreWriteStream::Available() const
    {
        return _buffer.SpaceLeftToWrite();
    }

    void DataStoreWriteStream::Write(const void* data, size_t dataSize)
    {
        _buffer.Write(data, dataSize);
    }

    void DataStoreWriteStream::Flush(DataStoreWriteCallback result)
    {
        _dataStore->EnqueueWrite(this, &_buffer, std::move(result));
    }

    DataStorePath DataStoreWriteStream::Path() const
    {
        return _path;
    }
}

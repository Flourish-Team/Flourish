#include "DataStore/DataStoreWriteStream.h"

#include "DataStore/IWritableDataStore.h"

namespace Flourish
{
    DataStoreWriteStream::DataStoreWriteStream(IWritableDataStore* dataStore, const DataStorePath& path)
        : _dataStore(dataStore)
        , _path(path)
        , _buffer(1024)
    {
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
        _dataStore->EnqueueWrite(this, &_buffer, result);
    }

    DataStorePath DataStoreWriteStream::Path() const
    {
        return _path;
    }
}

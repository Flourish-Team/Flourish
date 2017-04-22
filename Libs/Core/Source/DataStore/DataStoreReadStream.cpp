#include "DataStore/DataStoreReadStream.h"

#include "DataStore/IWritableDataStore.h"

namespace Flourish
{
    DataStoreReadStream::DataStoreReadStream(IReadableDataStore* dataStore, const DataStorePath& path, const DataBuffer& initialData)
        : _dataStore(dataStore)
        , _path(path)
        , _buffer(initialData)
    {
    }

    DataStoreReadStream::~DataStoreReadStream()
    {
        if(_dataStore != nullptr)
        {
            _dataStore->Close(this);
        }
    }

    size_t DataStoreReadStream::Available() const
    {
        return _buffer.DataAvailableToRead();
    }

    const void* DataStoreReadStream::Data() const
    {
        return _buffer.Data();
    }

    void DataStoreReadStream::Consume(size_t bytes)
    {
        _buffer.MarkAsRead(bytes);
    }

    void DataStoreReadStream::Refresh(DataStoreReadCallback callback)
    {
        _dataStore->EnqueueRead(this, &_buffer, callback);
    }

    DataStorePath DataStoreReadStream::Path() const
    {
        return _path;
    }

    bool DataStoreReadStream::EndOfData() const
    {
        return _buffer.DataAvailableToRead() < _buffer.Size();
    }
}

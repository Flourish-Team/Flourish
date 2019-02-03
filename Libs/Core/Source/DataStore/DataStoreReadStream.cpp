#include <utility>
#include "DataStore/DataStoreReadStream.h"

#include "DataStore/IWritableDataStore.h"

namespace Flourish
{
    DataStoreReadStream::DataStoreReadStream(IReadableDataStore* dataStore, DataStorePath path, DataBuffer initialData)
        : _dataStore(dataStore)
        , _path(std::move(path))
        , _buffer(std::move(initialData))
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
        return _buffer.ReadData();
    }

    void DataStoreReadStream::Consume(size_t bytes)
    {
        _buffer.MarkAsRead(bytes);
    }

    void DataStoreReadStream::Refresh(DataStoreReadCallback callback)
    {
        _dataStore->EnqueueRead(this, &_buffer, std::move(callback));
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

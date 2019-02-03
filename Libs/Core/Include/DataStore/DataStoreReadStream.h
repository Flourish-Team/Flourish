#pragma once

#include "DataStore/DataBuffer.h"
#include "DataStore/DataStoreCallbacks.h"
#include "DataStore/DataStorePath.h"

namespace Flourish
{
    class IReadableDataStore;

    class DataStoreReadStream
    {
    public:
        DataStoreReadStream(IReadableDataStore* dataStore, DataStorePath path, DataBuffer initialData);
        virtual ~DataStoreReadStream();

		DataStoreReadStream(DataStoreReadStream& other) = delete;
		DataStoreReadStream& operator=(const DataStoreReadStream&) = delete;

		DataStoreReadStream(DataStoreReadStream&& other) = delete;
		DataStoreReadStream& operator=(DataStoreReadStream&&) = delete;

        size_t Available() const;
        const void* Data() const;
        void Consume(size_t bytes);
        void Refresh(DataStoreReadCallback callback);
        DataStorePath Path() const;
        bool EndOfData() const;

    private:
        IReadableDataStore* _dataStore;
        DataStorePath _path;
        DataBuffer _buffer;
    };
}

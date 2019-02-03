#pragma once

#include "DataStore/DataBuffer.h"
#include "DataStore/DataStoreCallbacks.h"
#include "DataStore/DataStorePath.h"

namespace Flourish
{
    class IWritableDataStore;

    class DataStoreWriteStream
    {
    public:
        DataStoreWriteStream(IWritableDataStore* dataStore, DataStorePath path);
        virtual ~DataStoreWriteStream();

		DataStoreWriteStream(DataStoreWriteStream& other) = delete;
		DataStoreWriteStream& operator=(const DataStoreWriteStream&) = delete;

		DataStoreWriteStream(DataStoreWriteStream&& other) = delete;
		DataStoreWriteStream& operator=(DataStoreWriteStream&&) = delete;

        size_t Available() const;
        virtual void Write(const void* data, size_t dataSize);
        void Flush(DataStoreWriteCallback result);
        DataStorePath Path() const;

    private:
        IWritableDataStore* _dataStore;
        DataStorePath _path;
        DataBuffer _buffer;
    };
}

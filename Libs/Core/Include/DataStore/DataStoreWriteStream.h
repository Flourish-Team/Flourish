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
        DataStoreWriteStream(IWritableDataStore* dataStore, const DataStorePath& path);

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

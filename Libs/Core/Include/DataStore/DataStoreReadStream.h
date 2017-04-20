#pragma once

#include <stdint.h>

#include "DataStore/DataBuffer.h"
#include "DataStore/DataStoreCallbacks.h"
#include "DataStore/DataStorePath.h"

namespace Flourish
{
    class IReadableDataStore;

    class DataStoreReadStream
    {
    public:
        DataStoreReadStream(IReadableDataStore* dataStore, const DataStorePath& path, const DataBuffer& initialData);

        size_t Available() const;
        const void* Data() const;
        void Consume(size_t bytes);
        void Refresh(DataStoreReadCallback callback);
        DataStorePath Path() const;

    private:
        IReadableDataStore* _dataStore;
        DataStorePath _path;
        DataBuffer _buffer;
    };
}

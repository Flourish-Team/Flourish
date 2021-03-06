#pragma once

#include <functional>
#include <vector>
#include "DataStore/DataBuffer.h"
#include "DataStore/DataStorePath.h"
#include "DataStore/DataStoreCallbacks.h"

namespace Flourish
{
    class DataStoreReadStream;

    class IReadableDataStore
    {
    public:
		IReadableDataStore() = default;
        virtual ~IReadableDataStore() = default;

		IReadableDataStore(IReadableDataStore& other) = delete;
		IReadableDataStore& operator=(const IReadableDataStore&) = delete;

		IReadableDataStore(IReadableDataStore&& other) = delete;
		IReadableDataStore& operator=(IReadableDataStore&&) = delete;

        virtual bool Exists(const DataStorePath& path) const = 0;
        virtual void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) = 0;
        virtual void Close(DataStoreReadStream* stream) =  0;
        virtual bool IsDir(const DataStorePath& path) const = 0;
        virtual bool IsData(const DataStorePath& path) const = 0;
        virtual void Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const = 0;

    protected:
        virtual void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) = 0;

        friend class DataStoreReadStream;
    };
}

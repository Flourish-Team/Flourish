#pragma once

#include "DataStore/IReadableDataStore.h"
#include "DataStore/DataStoreCallbacks.h"

namespace Flourish
{
    class IWritableDataStore : public IReadableDataStore
    {
    public:
		using IReadableDataStore::Close;

		IWritableDataStore() = default;
        virtual ~IWritableDataStore() = default;

		IWritableDataStore(IWritableDataStore& other) = delete;
		IWritableDataStore& operator=(const IWritableDataStore&) = delete;

		IWritableDataStore(IWritableDataStore&& other) = delete;
		IWritableDataStore& operator=(IWritableDataStore&&) = delete;

		virtual void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) = 0;
        virtual void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) = 0;
        virtual void Close(DataStoreWriteStream* stream) = 0;

    private:
        virtual void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) = 0;

        friend class DataStoreWriteStream;
    };
}

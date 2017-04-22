#pragma once

#include "IWritableDataStore.h"

namespace Flourish
{
    class LocalFileDataStore : public IWritableDataStore
    {
    public:
        LocalFileDataStore(const char* root);

        bool Exists(const DataStorePath& path) const override;
        void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) override;
        void Close(DataStoreReadStream* stream) override;
        bool IsDir(const DataStorePath& path) const override;
        bool IsData(const DataStorePath& path) const override;
        void Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const override;
        void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void Close(DataStoreWriteStream* stream) override;

    private:
        void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override;
        void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) override;
    };
}
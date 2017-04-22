#pragma once

#include "DataStore/IWritableDataStore.h"
#include "DataStore/DataStorePath.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/DataStoreReadStream.h"

#include <map>

namespace Flourish
{
    class LocalFileDataStore : public IWritableDataStore
    {
    public:
        LocalFileDataStore(const char* root);
        ~LocalFileDataStore();

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

        class OpenFile
        {
        public:
            OpenFile(FILE* file, std::shared_ptr<DataStoreReadStream> stream);
            OpenFile(FILE* file, std::shared_ptr<DataStoreWriteStream> stream);

            const std::shared_ptr<DataStoreWriteStream> GetCurrentWriteStream() const;
            const std::shared_ptr<DataStoreReadStream> GetCurrentReadStream() const;

        private:
            FILE* _file;
            std::weak_ptr<DataStoreWriteStream> _currentWriteStream;
            std::weak_ptr<DataStoreReadStream> _currentReadStream;
        };

        typedef std::map<DataStorePath, OpenFile*> OpenFileMap;
        OpenFileMap _pathToOpenFile;
        const char* root;
    };
}
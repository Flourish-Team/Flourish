#pragma once

#include <map>
#include <vector>

#include "DataStore/IWritableDataStore.h"
#include "DataStore/DataStorePath.h"

namespace Flourish
{
    class MemoryDataStore : public IWritableDataStore
    {
    public:
        virtual ~MemoryDataStore();

        bool Exists(const DataStorePath& path) const;
        void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback) override;
        void Close(DataStoreReadStream* stream) override;
        void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void Close(DataStoreWriteStream* stream) override;
        bool IsDir(const DataStorePath& path) const override;
        bool IsData(const DataStorePath& path) const override;
        void Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const override;

    private:
        void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) override;
        void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override;
        void CreateDirTree(const DataStorePath& path);

        class Record
        {
        public:
            static Record* Dir();
            static Record* Data();

            void ResetReadHead();
            void Append(DataBuffer* buffer);
            void Fill(DataBuffer* buffer);
            void Clear();
            void SetCurrentStream(std::shared_ptr<DataStoreWriteStream> stream);
            void SetCurrentStream(std::shared_ptr<DataStoreReadStream> stream);
            const std::shared_ptr<DataStoreWriteStream> GetCurrentWriteStream() const;
            const std::shared_ptr<DataStoreReadStream> GetCurrentReadStream() const;
            void ClearCurrentStream();
            bool HasCurrentStream();
            bool IsDir();

        private:
            Record(bool isDir);

            bool _isDir;
            std::vector<uint8_t> _data;
            size_t _readHead;
            std::weak_ptr<DataStoreWriteStream> _currentWriteStream;
            std::weak_ptr<DataStoreReadStream> _currentReadStream;
        };

        typedef std::map<DataStorePath, Record*> RecordMap;
        RecordMap _pathToRecord;
    };
}

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
        void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) override;
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

            bool IsDir();
        private:
            Record(bool isDir);

            bool _isDir;
            std::vector<uint8_t> _data;
            size_t _readHead;
        };

        typedef std::map<DataStorePath, Record*> RecordMap;
        RecordMap _pathToRecord;
    };
}

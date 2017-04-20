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
        void OpenForRead(const DataStorePath& path, DataStoreReadCallback callback);
        void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback);

    private:
        void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback);
        void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback);

        class Record
        {
        public:
            static Record* Dir();
            static Record* Data();

            void Append(DataBuffer* buffer);
            void Fill(DataBuffer* buffer);

        private:
            Record(bool isDir);

            bool _isDir;
            std::vector<uint8_t> _data;
        };

        typedef std::map<DataStorePath, Record*> RecordMap;
        RecordMap _pathToRecord;
    };
}
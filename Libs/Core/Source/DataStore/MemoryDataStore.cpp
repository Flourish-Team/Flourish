#include "DataStore/MemoryDataStore.h"

#include "DataStore/DataStoreReadStream.h"
#include "DataStore/DataStoreWriteStream.h"

namespace Flourish
{
    bool operator<(const DataStorePath& lhs, const DataStorePath& rhs)
    {
        return lhs.AsString() < rhs.AsString();
    }

    MemoryDataStore::~MemoryDataStore()
    {
        for (auto iter : _pathToRecord)
        {
            delete iter.second;
        }
    }

    bool MemoryDataStore::Exists(const DataStorePath& path) const
    {
        return _pathToRecord.find(path) != _pathToRecord.end();
    }

    void MemoryDataStore::OpenForRead(const DataStorePath& path, DataStoreReadCallback callback)
    {
        auto recordIter = _pathToRecord.find(path);
        if (recordIter == _pathToRecord.end())
        {
            std::string errorMessage = "Path '";
            errorMessage.append(path.AsString());
            errorMessage.append("' does not exist in data store");
            callback(Error<DataStoreReadStream*>::Failure(errorMessage.c_str()));
            return;
        }

        DataBuffer buffer(1024);
        recordIter->second->Fill(&buffer);
        auto stream = new DataStoreReadStream(this, path, buffer);
        callback(Error<DataStoreReadStream*>::Successful(stream));
    }

    void MemoryDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        CreateDirTree(path);
        _pathToRecord.insert(std::make_pair(path, Record::Data()));
        auto stream = new DataStoreWriteStream(this, path);
        callback(Error<DataStoreWriteStream*>::Successful(stream));
    }

    void MemoryDataStore::CreateDirTree(const DataStorePath& path)
    {
        auto dirPath = path;
        do
        {
            dirPath = dirPath.GetDirectory();
            if (!Exists(dirPath))
            {
                _pathToRecord.insert(std::make_pair(dirPath, Record::Dir()));
            }
        } while (dirPath != DataStorePath(""));
    }

    bool MemoryDataStore::IsDir(const DataStorePath& path) const
    {
        auto recordIter = _pathToRecord.find(path);
        if (recordIter == _pathToRecord.end())
        {
            return false;
        }
        return recordIter->second->IsDir();
    }

    bool MemoryDataStore::IsData(const DataStorePath& path) const
    {
        auto recordIter = _pathToRecord.find(path);
        if (recordIter == _pathToRecord.end())
        {
            return false;
        }
        return !recordIter->second->IsDir();
    }

    void MemoryDataStore::Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const
    {
        for(auto& entry : _pathToRecord)
        {
            const auto entryPath = entry.first;
            if(entryPath.GetDirectory() == dirPath)
            {
                entries.push_back(entryPath);
            }
        }
    }

    void MemoryDataStore::EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback)
    {
        auto record = _pathToRecord.find(stream->Path())->second;
        record->Append(buffer);
        buffer->Clear();
        callback(Error<DataStoreWriteStream*>::Successful(stream));
    }

    void MemoryDataStore::EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
    {
        auto record = _pathToRecord.find(stream->Path())->second;
        record->Fill(buffer);
        callback(Error<DataStoreReadStream*>::Successful(stream));
    }

    MemoryDataStore::Record* MemoryDataStore::Record::Dir()
    {
        return new MemoryDataStore::Record(true);
    }

    MemoryDataStore::Record* MemoryDataStore::Record::Data()
    {
        return new MemoryDataStore::Record(false);
    }

    void MemoryDataStore::Record::Append(DataBuffer* buffer)
    {
        _data.reserve(_data.size() + buffer->DataAvailableToRead());
        auto bufferData = static_cast<const uint8_t*>(buffer->Data());
        for (auto byteIdx = 0; byteIdx < buffer->DataAvailableToRead(); byteIdx++)
        {
            _data.push_back(bufferData[byteIdx]);
        }
    }

    void MemoryDataStore::Record::Fill(DataBuffer* buffer)
    {
        buffer->Clear();
        buffer->Write(_data.data(), std::min(buffer->SpaceLeftToWrite(), _data.size()));
    }

    MemoryDataStore::Record::Record(bool isDir)
        : _isDir(isDir)
        , _data()
    {
    }

    bool MemoryDataStore::Record::IsDir()
    {
        return _isDir;
    }
}

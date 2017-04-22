#include "DataStore/MemoryDataStore.h"

#include "DataStore/DataStoreReadStream.h"
#include "DataStore/DataStoreWriteStream.h"

namespace Flourish
{
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
            callback(DataStoreReadCallbackParam::Failure(errorMessage.c_str()));
            return;
        }

        DataBuffer buffer(1024);
        auto record = recordIter->second;
        if(record->HasCurrentStream())
        {
            std::string error("Path: '");
            error.append(path.AsString());
            error.append("' is already open in another stream");
            callback(DataStoreReadCallbackParam::Failure(error.c_str()));
            return;
        }
        record->ResetReadHead();
        record->Fill(&buffer);
        auto stream = std::shared_ptr<DataStoreReadStream>(new DataStoreReadStream(this, path, buffer));
        record->SetCurrentStream(stream);
        callback(DataStoreReadCallbackParam::Successful(stream));
    }

    void MemoryDataStore::Close(DataStoreReadStream* stream)
    {
        _pathToRecord[stream->Path()]->ClearCurrentStream();
    }

    void MemoryDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        CreateDirTree(path);
        auto iterAndInserted = _pathToRecord.insert({path, Record::Data()});
        if (!iterAndInserted.second)
        {
            // Element already exists
            auto record = _pathToRecord[path];
            record->Clear();
            if(record->HasCurrentStream())
            {
                std::string error("Path: '");
                error.append(path.AsString());
                error.append("' is already open in another stream");
                callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
                return;
            }
        }
        const auto stream = std::shared_ptr<DataStoreWriteStream>(new DataStoreWriteStream(this, path));
        _pathToRecord[path]->SetCurrentStream(stream);
        callback(DataStoreWriteCallbackParam::Successful(stream));
    }

    void MemoryDataStore::OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        CreateDirTree(path);
        _pathToRecord.insert({path, Record::Data()});
        auto record = _pathToRecord[path];
        if(record->HasCurrentStream())
        {
            std::string error("Path: '");
            error.append(path.AsString());
            error.append("' is already open in another stream");
            callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
            return;
        }
        const auto stream = std::make_shared<DataStoreWriteStream>(this, path);
        record->SetCurrentStream(stream);
        callback(DataStoreWriteCallbackParam::Successful(stream));
    }

    void MemoryDataStore::Close(DataStoreWriteStream* stream)
    {
        _pathToRecord[stream->Path()]->ClearCurrentStream();
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
        for (auto& entry : _pathToRecord)
        {
            const auto entryPath = entry.first;
            if (entryPath.GetDirectory() == dirPath)
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
        callback(DataStoreWriteCallbackParam::Successful(record->GetCurrentWriteStream()));
    }

    void MemoryDataStore::EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
    {
        if(stream->EndOfData())
        {
            std::string error("Attempted to read past end of stream  (Path: '");
            error.append(stream->Path().AsString());
            error.append("')");
            callback(DataStoreReadCallbackParam::Failure(error.c_str()));
            return;
        }
        auto record = _pathToRecord.find(stream->Path())->second;
        record->Fill(buffer);
        callback(DataStoreReadCallbackParam::Successful(record->GetCurrentReadStream()));
    }

    MemoryDataStore::Record* MemoryDataStore::Record::Dir()
    {
        return new MemoryDataStore::Record(true);
    }

    MemoryDataStore::Record* MemoryDataStore::Record::Data()
    {
        return new MemoryDataStore::Record(false);
    }

    void MemoryDataStore::Record::ResetReadHead()
    {
        _readHead = 0;
    }

    void MemoryDataStore::Record::Append(DataBuffer* buffer)
    {
        _data.reserve(_data.size() + buffer->DataAvailableToRead());
        auto bufferData = static_cast<const uint8_t*>(buffer->ReadData());
        for (auto byteIdx = 0; byteIdx < buffer->DataAvailableToRead(); byteIdx++)
        {
            _data.push_back(bufferData[byteIdx]);
        }
    }

    void MemoryDataStore::Record::Fill(DataBuffer* buffer)
    {
        buffer->Clear();
        const auto numBytesToRead = std::min(buffer->SpaceLeftToWrite(), _data.size() - _readHead);
        buffer->Write(&_data.data()[_readHead], numBytesToRead);
        _readHead += numBytesToRead;
    }

    void MemoryDataStore::Record::Clear()
    {
        _data.clear();
    }

    void MemoryDataStore::Record::ClearCurrentStream()
    {
        _currentReadStream.reset();
        _currentWriteStream.reset();
    }

    bool MemoryDataStore::Record::HasCurrentStream()
    {
        return _currentReadStream.use_count() > 0 || _currentWriteStream.use_count() > 0;
    }

    bool MemoryDataStore::Record::IsDir()
    {
        return _isDir;
    }

    MemoryDataStore::Record::Record(bool isDir)
        : _isDir(isDir)
        , _data()
        , _readHead(0)
        , _currentWriteStream()
        , _currentReadStream()
    {
    }

    void MemoryDataStore::Record::SetCurrentStream(std::shared_ptr<DataStoreWriteStream> stream)
    {
        _currentWriteStream = stream;
    }

    void MemoryDataStore::Record::SetCurrentStream(std::shared_ptr<DataStoreReadStream> stream)
    {
        _currentReadStream = stream;
    }

    const std::shared_ptr<DataStoreWriteStream> MemoryDataStore::Record::GetCurrentWriteStream() const
    {
        return _currentWriteStream.lock();
    }

    const std::shared_ptr<DataStoreReadStream> MemoryDataStore::Record::GetCurrentReadStream() const
    {
        return _currentReadStream.lock();
    }
}

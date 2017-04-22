#include "DataStore/LocalFileDataStore.h"

#include "DataStore/FileSystem.h"

namespace Flourish
{
    LocalFileDataStore::LocalFileDataStore(const char* root)
        : _root(root)
    {
        FileSystem::CreateDirectoryTree(root);
        auto lastChar = root[_root.length() - 1];
        if(lastChar != '\\' && lastChar != '/')
        {
            _root.append("/");
        }
    }

    LocalFileDataStore::~LocalFileDataStore()
    {
        for (auto iter : _pathToOpenFile)
        {
            delete iter.second;
        }
    }

    bool LocalFileDataStore::Exists(const DataStorePath& path) const
    {
        auto pathAsCStr = path.AsString().c_str();
        return FileSystem::FileExists(pathAsCStr) || FileSystem::DirExists(pathAsCStr);
    }

    void LocalFileDataStore::OpenForRead(const DataStorePath& path, DataStoreReadCallback callback)
    {
        if (_pathToOpenFile.find(path) == _pathToOpenFile.end())
        {
            DataBuffer buffer(1024);
            auto file = FileSystem::OpenRead(GetFullPath(path).c_str());
            FileSystem::Read(static_cast<uint8_t*>(buffer.WriteData()), buffer.Size(), file);
            const auto stream = std::make_shared<DataStoreReadStream>(this, path, buffer);
            _pathToOpenFile.insert({path, new OpenFile(file, stream)});
            callback(DataStoreReadCallbackParam::Successful(stream));
        }
    }

    void LocalFileDataStore::Close(DataStoreReadStream* stream)
    {
        auto iter = _pathToOpenFile.find(stream->Path());
        delete iter->second;
        _pathToOpenFile.erase(iter);
    }

    bool LocalFileDataStore::IsDir(const DataStorePath& path) const
    {
        return FileSystem::DirExists(path.AsString().c_str());
    }

    bool LocalFileDataStore::IsData(const DataStorePath& path) const
    {
        return FileSystem::FileExists(path.AsString().c_str());
    }

    void LocalFileDataStore::Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const
    {

    }

    void LocalFileDataStore::EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
    {
        auto openFile = _pathToOpenFile[stream->Path()];
        openFile->Fill(buffer);
        callback(DataStoreReadCallbackParam::Successful(openFile->GetCurrentReadStream()));
    }

    void LocalFileDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        auto file = FileSystem::OpenWrite(GetFullPath(path).c_str());
        auto stream = std::make_shared<DataStoreWriteStream>(this, path);
        _pathToOpenFile.insert({path, new OpenFile(file, stream)});
        callback(DataStoreWriteCallbackParam::Successful(stream));
    }

    void LocalFileDataStore::OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        auto file = FileSystem::OpenAppend(GetFullPath(path).c_str());
        auto stream = std::make_shared<DataStoreWriteStream>(this, path);
        _pathToOpenFile.insert({path, new OpenFile(file, stream)});
        callback(DataStoreWriteCallbackParam::Successful(stream));
    }

    void LocalFileDataStore::Close(DataStoreWriteStream* stream)
    {
        auto iter = _pathToOpenFile.find(stream->Path());
        delete iter->second;
        _pathToOpenFile.erase(iter);
    }

    void LocalFileDataStore::EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback)
    {
        auto openFile = _pathToOpenFile[stream->Path()];
        openFile->Append(buffer);
        callback(DataStoreWriteCallbackParam::Successful(openFile->GetCurrentWriteStream()));
    }

    std::string LocalFileDataStore::GetFullPath(const DataStorePath& path)
    {
        auto fullPath = std::string(_root);
        return fullPath.append(path.AsString());
    }

    LocalFileDataStore::OpenFile::OpenFile(FILE* file, std::shared_ptr<DataStoreReadStream> stream)
        : _file(file)
        , _currentWriteStream()
        , _currentReadStream(stream)
    {
    }

    LocalFileDataStore::OpenFile::OpenFile(FILE* file, std::shared_ptr<DataStoreWriteStream> stream)
        : _file(file)
        , _currentWriteStream(stream)
        , _currentReadStream()
    {
    }

    LocalFileDataStore::OpenFile::~OpenFile()
    {
        FileSystem::Close(_file);
    }

    void LocalFileDataStore::OpenFile::Append(DataBuffer* buffer)
    {
        FileSystem::Write(static_cast<const uint8_t*>(buffer->ReadData()), buffer->DataAvailableToRead(), _file);
    }

    void LocalFileDataStore::OpenFile::Fill(DataBuffer* buffer)
    {
        FileSystem::Read(static_cast<uint8_t*>(buffer->WriteData()), buffer->SpaceLeftToWrite(), _file);
    }

    const std::shared_ptr<DataStoreWriteStream> LocalFileDataStore::OpenFile::GetCurrentWriteStream() const
    {
        return _currentWriteStream.lock();
    }

    const std::shared_ptr<DataStoreReadStream> LocalFileDataStore::OpenFile::GetCurrentReadStream() const
    {
        return _currentReadStream.lock();
    }
}

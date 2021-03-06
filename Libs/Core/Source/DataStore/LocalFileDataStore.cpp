#include "DataStore/LocalFileDataStore.h"

#include "FileSystem/FileSystem.h"

namespace Flourish
{
    LocalFileDataStore::LocalFileDataStore(const char* root)
        : _root(root)
    {
        FileSystem::CreateDirectoryTree(root);
        auto lastChar = root[_root.length() - 1];
        if (lastChar != '\\' && lastChar != '/')
        {
            _root.append("/");
        }
    }

    LocalFileDataStore::~LocalFileDataStore()
    {
        for (const auto& iter : _pathToOpenFile)
        {
            delete iter.second;
        }
    }

    bool LocalFileDataStore::Exists(const DataStorePath& path) const
    {
		auto fullPath = GetFullPath(path);
        auto pathAsCStr = fullPath.c_str();
        return FileSystem::FileExists(pathAsCStr) || FileSystem::DirExists(pathAsCStr);
    }

    void LocalFileDataStore::OpenForRead(const DataStorePath& path, DataStoreReadCallback callback)
    {
        if (_pathToOpenFile.find(path) == _pathToOpenFile.end())
        {
            DataBuffer buffer(1024);
            auto file = FileSystem::OpenRead(GetFullPath(path).c_str());
            if (file == nullptr)
            {
                std::string errorMessage = "Path '";
                errorMessage.append(path.AsString());
                errorMessage.append("' does not exist in data store");
                callback(DataStoreReadCallbackParam::Failure(errorMessage.c_str()));
                return;
            }
            FileSystem::Read(static_cast<uint8_t*>(buffer.WriteData()), buffer.Size(), file);
            const auto stream = std::make_shared<DataStoreReadStream>(this, path, buffer);
            _pathToOpenFile.insert({path, new OpenFile(file, stream)});
            callback(DataStoreReadCallbackParam::Successful(stream));
        }
        else
        {
            std::string error("Path: '");
            error.append(path.AsString());
            error.append("' is already open in another stream");
            callback(DataStoreReadCallbackParam::Failure(error.c_str()));
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
        return FileSystem::DirExists(GetFullPath(path).c_str());
    }

    bool LocalFileDataStore::IsData(const DataStorePath& path) const
    {
        return FileSystem::FileExists(GetFullPath(path).c_str());
    }

    void LocalFileDataStore::Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const
    {
        std::vector<std::string> stringEntries;
        FileSystem::EnumerateDirectory(GetFullPath(dirPath).c_str(), stringEntries);
        for (auto& stringEntry : stringEntries)
        {
            auto fullPath = dirPath.AsString();
            fullPath.append("/");
            fullPath.append(stringEntry);
            entries.emplace_back(DataStorePath(fullPath));
        }
    }

    void LocalFileDataStore::EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
    {
        if (stream->EndOfData())
        {
            std::string error("Attempted to read past end of stream  (Path: '");
            error.append(stream->Path().AsString());
            error.append("')");
            callback(DataStoreReadCallbackParam::Failure(error.c_str()));
            return;
        }
        auto openFile = _pathToOpenFile[stream->Path()];
        openFile->Fill(buffer);
        callback(DataStoreReadCallbackParam::Successful(openFile->GetCurrentReadStream()));
    }

    void LocalFileDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        if(_pathToOpenFile.find(path) != _pathToOpenFile.end())
        {
            std::string error("Path: '");
            error.append(path.AsString());
            error.append("' is already open in another stream");
            callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
            return;
        }
        auto fullPathToDir = GetFullPath(path.GetDirectory());
        FileSystem::CreateDirectoryTree(fullPathToDir.c_str());
        auto file = FileSystem::OpenWrite(GetFullPath(path).c_str());
        auto stream = std::make_shared<DataStoreWriteStream>(this, path);
        _pathToOpenFile.insert({path, new OpenFile(file, stream)});
        callback(DataStoreWriteCallbackParam::Successful(stream));
    }

    void LocalFileDataStore::OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        if(_pathToOpenFile.find(path) != _pathToOpenFile.end())
        {
            std::string error("Path: '");
            error.append(path.AsString());
            error.append("' is already open in another stream");
            callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
            return;
        }
        auto fullPathToDir = GetFullPath(path.GetDirectory());
        FileSystem::CreateDirectoryTree(fullPathToDir.c_str());
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
        buffer->Clear();
        callback(DataStoreWriteCallbackParam::Successful(openFile->GetCurrentWriteStream()));
    }

    std::string LocalFileDataStore::GetFullPath(const DataStorePath& path) const
    {
        auto fullPath = std::string(_root);
        return fullPath.append(path.AsString());
    }

    LocalFileDataStore::OpenFile::OpenFile(FILE* file, const std::shared_ptr<DataStoreReadStream>& stream)
        : _file(file)
        , _currentReadStream(stream)
    {
    }

    LocalFileDataStore::OpenFile::OpenFile(FILE* file, const std::shared_ptr<DataStoreWriteStream>& stream)
        : _file(file)
        , _currentWriteStream(stream)
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

    std::shared_ptr<DataStoreWriteStream> LocalFileDataStore::OpenFile::GetCurrentWriteStream() const
    {
        return _currentWriteStream.lock();
    }

    std::shared_ptr<DataStoreReadStream> LocalFileDataStore::OpenFile::GetCurrentReadStream() const
    {
        return _currentReadStream.lock();
    }
}

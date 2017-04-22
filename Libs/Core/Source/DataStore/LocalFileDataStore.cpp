#include "DataStore/LocalFileDataStore.h"

#include "DataStore/FileSystem.h"

namespace Flourish
{
    LocalFileDataStore::LocalFileDataStore(const char* root)
        : root(root)
    {
        FileSystem::CreateDirectoryTree(root);
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
            /*DataBuffer buffer(1024);
            auto file = FileSystem::OpenRead(path.AsString().c_str());
            FileSystem::Read(buffer.Write())
            auto stream = new DataStoreReadStream(this, path, buffer);
            _pathToOpenFile.insert({path, new OpenFile()})*/
        }
    }

    void LocalFileDataStore::Close(DataStoreReadStream* stream)
    {

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

    }

    void LocalFileDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {

    }

    void LocalFileDataStore::OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback)
    {

    }

    void LocalFileDataStore::Close(DataStoreWriteStream* stream)
    {

    }

    void LocalFileDataStore::EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback)
    {

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

    const std::shared_ptr<DataStoreWriteStream> LocalFileDataStore::OpenFile::GetCurrentWriteStream() const
    {
        return _currentWriteStream.lock();
    }

    const std::shared_ptr<DataStoreReadStream> LocalFileDataStore::OpenFile::GetCurrentReadStream() const
    {
        return _currentReadStream.lock();
    }
}

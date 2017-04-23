#include <mutex>
#include "DataStore/LocalFileDataStore.h"

#include "DataStore/FileSystem.h"

namespace Flourish
{
    LocalFileDataStore::LocalFileDataStore(const char* root)
        : _pathToOpenFile()
        , _root(root)
        , _operationQueue()
        , _operationThread(&LocalFileDataStore::RunOperations, this)
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
        _operationQueue.Stop();
        _operationThread.join();
        for (auto iter : _pathToOpenFile)
        {
            delete iter.second;
        }
    }

    bool LocalFileDataStore::Exists(const DataStorePath& path) const
    {
        auto pathAsCStr = GetFullPath(path).c_str();
        return FileSystem::FileExists(pathAsCStr) || FileSystem::DirExists(pathAsCStr);
    }

    void LocalFileDataStore::OpenForRead(const DataStorePath& path, DataStoreReadCallback callback)
    {
        DataStorePath localPath(path);
        _operationQueue.Push(new OpenForReadOperation(this, path, callback));
    }

    void LocalFileDataStore::Close(DataStoreReadStream* stream)
    {
        _operationQueue.Push(new CloseReadOperation(this, stream));
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
            entries.push_back(DataStorePath(fullPath));
        }
    }

    void LocalFileDataStore::EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
    {
        _operationQueue.Push(new ReadOperation(this, stream, buffer, callback));
    }

    void LocalFileDataStore::OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        _operationQueue.Push(new OpenForWriteOperation(this, path, callback));
    }

    void LocalFileDataStore::OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback)
    {
        DataStorePath localPath(path);
        _operationQueue.Push(new OpenForAppendOperation(this, path, callback));
    }

    void LocalFileDataStore::Close(DataStoreWriteStream* stream)
    {
        _operationQueue.Push(new CloseWriteOperation(this, stream));
    }

    void LocalFileDataStore::EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback)
    {
        _operationQueue.Push(new WriteOperation(this, stream, buffer, callback));
    }

    std::string LocalFileDataStore::GetFullPath(const DataStorePath& path) const
    {
        auto fullPath(_root);
        fullPath.append(path.AsString());
        return fullPath;
    }

    void LocalFileDataStore::RunOperations()
    {
        while (!_operationQueue.IsFinishing())
        {
            _operationQueue.RunOneOperation();
        }
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
        return _currentWriteStream;
    }

    const std::shared_ptr<DataStoreReadStream> LocalFileDataStore::OpenFile::GetCurrentReadStream() const
    {
        return _currentReadStream;
    }

    LocalFileDataStore::OperationQueue::OperationQueue()
        : _operations()
        , _mutex()
        , _condition()
        , _finishing(false)
    {
        printf("Starting\n");
    }

    void LocalFileDataStore::OperationQueue::Push(IOperation* operation)
    {
        while (true)
        {
            std::unique_lock<std::mutex> locker(_mutex);
            printf("Pushed operation\n");
            _operations.push(operation);
            locker.unlock();
            _condition.notify_all();
            return;
        }
    }

    void LocalFileDataStore::OperationQueue::RunOneOperation()
    {
        while (true)
        {
            std::unique_lock<std::mutex> locker(_mutex);
            printf("Waiting for operation\n");
            if (_operations.size() <= 0)
            {
                _condition.wait(locker);
            }
            printf("Came out of wait\n");
            if (_finishing)
            {
                printf("Exiting because finishing\n");
                return;
            }
            if (_operations.size() <= 0)
            {
                printf("Continueing because no operations\n");
                continue;
            }
            printf("Running operation\n");
            auto operation = _operations.back();
            _operations.pop();
            locker.unlock();
            operation->Execute();
            delete operation;
        }
    }

    void LocalFileDataStore::OperationQueue::Stop()
    {
        printf("Stopping\n");
        _finishing = true;
        _condition.notify_all();
    }

    bool LocalFileDataStore::OperationQueue::IsFinishing()
    {
        return _finishing;
    }

}

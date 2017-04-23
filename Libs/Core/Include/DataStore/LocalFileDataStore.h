#pragma once

#include "DataStore/DataStorePath.h"
#include "DataStore/DataStoreReadStream.h"
#include "DataStore/DataStoreWriteStream.h"
#include "DataStore/IWritableDataStore.h"
#include "DataStore/FileSystem.h"

#include <functional>
#include <map>
#include <queue>
#include <thread>
#include <future>


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
        void OpenForWrite(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void Enumerate(const DataStorePath& dirPath, std::vector<DataStorePath>& entries) const override;
        void OpenForAppend(const DataStorePath& path, DataStoreWriteCallback callback) override;
        void Close(DataStoreWriteStream* stream) override;

    private:
        void EnqueueRead(DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback) override;
        void EnqueueWrite(DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback) override;
        std::string GetFullPath(const DataStorePath& path) const;
        void RunOperations();

        class OpenFile
        {
        public:
            OpenFile(FILE* file, std::shared_ptr<DataStoreReadStream> stream);
            OpenFile(FILE* file, std::shared_ptr<DataStoreWriteStream> stream);
            virtual ~OpenFile();

            const std::shared_ptr<DataStoreWriteStream> GetCurrentWriteStream() const;
            const std::shared_ptr<DataStoreReadStream> GetCurrentReadStream() const;

            void Append(DataBuffer* buffer);
            void Fill(DataBuffer* buffer);

        private:
            FILE* _file;
            std::shared_ptr<DataStoreWriteStream> _currentWriteStream;
            std::shared_ptr<DataStoreReadStream> _currentReadStream;
        };

        class IOperation
        {
        public:
            virtual ~IOperation()
            {
            }

            virtual void Execute() = 0;
        };

        class OpenForReadOperation : public IOperation
        {
        public:
            OpenForReadOperation(LocalFileDataStore* dataStore, const DataStorePath& path, DataStoreReadCallback callback)
                : _dataStore(dataStore)
                , _localPath(path)
                , _callback(callback)
            {

            }

            void Execute() override
            {
                printf("Open for read: %s\n", _localPath.AsString().c_str());
                if (_dataStore->_pathToOpenFile.find(_localPath) ==
                    _dataStore->_pathToOpenFile.end())
                {
                    DataBuffer buffer(1024);
                    auto file = FileSystem::OpenRead(_dataStore->GetFullPath(_localPath).c_str());
                    if (file == nullptr)
                    {
                        std::string errorMessage = "Path '";
                        errorMessage.append(_localPath.AsString());
                        errorMessage.append("' does not exist in data store");
                        _callback(DataStoreReadCallbackParam::Failure(errorMessage.c_str()));
                        return;
                    }
                    FileSystem::Read(static_cast<uint8_t*>(buffer.WriteData()), buffer.Size(), file);
                    const auto stream = std::make_shared<DataStoreReadStream>(_dataStore, _localPath, buffer);
                    _dataStore->_pathToOpenFile.insert({_localPath, new OpenFile(file, stream)});
                    _callback(DataStoreReadCallbackParam::Successful(stream));
                }
                else
                {
                    std::string error("Path: '");
                    error.append(_localPath.AsString());
                    error.append("' is already open in another stream");
                    _callback(DataStoreReadCallbackParam::Failure(error.c_str()));
                }
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStorePath _localPath;
            DataStoreReadCallback _callback;
        };

        class ReadOperation : public IOperation
        {
        public:
            ReadOperation(LocalFileDataStore* dataStore, DataStoreReadStream* stream, DataBuffer* buffer, DataStoreReadCallback callback)
                : _dataStore(dataStore)
                , _stream(stream)
                , _buffer(buffer)
                , _callback(callback)
            {
            }

            void Execute() override
            {
                auto openFile = _dataStore->_pathToOpenFile[_stream->Path()];
                printf("Enqueue read: %s\n", _stream->Path().AsString().c_str());
                if (_stream->EndOfData())
                {
                    std::string error("Attempted to read past end of stream  (Path: '");
                    error.append(_stream->Path().AsString());
                    error.append("')");
                    _callback(DataStoreReadCallbackParam::Failure(error.c_str()));
                    return;
                }
                openFile->Fill(_buffer);
                _callback(DataStoreReadCallbackParam::Successful(openFile->GetCurrentReadStream()));
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStoreReadStream* _stream;
            DataBuffer* _buffer;
            DataStoreReadCallback _callback;
        };

        class CloseReadOperation : public IOperation
        {
        public:
            CloseReadOperation(LocalFileDataStore* dataStore, DataStoreReadStream* stream)
                : _dataStore(dataStore)
                , _stream(stream)
            {
            }

            void Execute() override
            {
                printf("Close read stream: %s\n", _stream->Path().AsString().c_str());
                auto iter = _dataStore->_pathToOpenFile.find(_stream->Path());
                delete iter->second;
                _dataStore->_pathToOpenFile.erase(iter);
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStoreReadStream* _stream;
        };

        class OpenForWriteOperation : public IOperation
        {
        public:
            OpenForWriteOperation(LocalFileDataStore* dataStore, const DataStorePath& path, DataStoreWriteCallback callback)
                : _dataStore(dataStore)
                , _localPath(path)
                , _callback(callback)
            {

            }

            void Execute() override
            {
                printf("Open for write: %s\n", _localPath.AsString().c_str());
                if (_dataStore->_pathToOpenFile.find(_localPath) != _dataStore->_pathToOpenFile.end())
                {
                    std::string error("Path: '");
                    error.append(_localPath.AsString());
                    error.append("' is already open in another stream");
                    _callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
                    return;
                }
                auto fullPathToDir = _dataStore->GetFullPath(_localPath.GetDirectory());
                FileSystem::CreateDirectoryTree(fullPathToDir.c_str());
                auto file = FileSystem::OpenWrite(_dataStore->GetFullPath(_localPath).c_str());
                auto stream = std::make_shared<DataStoreWriteStream>(_dataStore, _localPath);
                _dataStore->_pathToOpenFile.insert({_localPath, new OpenFile(file, stream)});
                _callback(DataStoreWriteCallbackParam::Successful(stream));
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStorePath _localPath;
            DataStoreWriteCallback _callback;
        };

        class OpenForAppendOperation : public IOperation
        {
        public:
            OpenForAppendOperation(LocalFileDataStore* dataStore, const DataStorePath& path, DataStoreWriteCallback callback)
                : _dataStore(dataStore)
                , _localPath(path)
                , _callback(callback)
            {

            }

            void Execute() override
            {
                printf("Open for append: %s\n", _localPath.AsString().c_str());
                if (_dataStore->_pathToOpenFile.find(_localPath) != _dataStore->_pathToOpenFile.end())
                {
                    std::string error("Path: '");
                    error.append(_localPath.AsString());
                    error.append("' is already open in another stream");
                    _callback(DataStoreWriteCallbackParam::Failure(error.c_str()));
                    return;
                }
                auto fullPathToDir = _dataStore->GetFullPath(_localPath.GetDirectory());
                FileSystem::CreateDirectoryTree(fullPathToDir.c_str());
                auto file = FileSystem::OpenAppend(_dataStore->GetFullPath(_localPath).c_str());
                auto stream = std::make_shared<DataStoreWriteStream>(_dataStore, _localPath);
                _dataStore->_pathToOpenFile.insert({_localPath, new OpenFile(file, stream)});
                _callback(DataStoreWriteCallbackParam::Successful(stream));
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStorePath _localPath;
            DataStoreWriteCallback _callback;
        };

        class WriteOperation : public IOperation
        {
        public:
            WriteOperation(LocalFileDataStore* dataStore, DataStoreWriteStream* stream, DataBuffer* buffer, DataStoreWriteCallback callback)
                : _dataStore(dataStore)
                , _stream(stream)
                , _buffer(buffer)
                , _callback(callback)
            {
            }

            void Execute() override
            {
                printf("Enqueue write: %s\n", _stream->Path().AsString().c_str());
                auto openFile = _dataStore->_pathToOpenFile[_stream->Path()];
                openFile->Append(_buffer);
                _buffer->Clear();
                _callback(DataStoreWriteCallbackParam::Successful(openFile->GetCurrentWriteStream()));
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStoreWriteStream* _stream;
            DataBuffer* _buffer;
            DataStoreWriteCallback _callback;
        };

        class CloseWriteOperation : public IOperation
        {
        public:
            CloseWriteOperation(LocalFileDataStore* dataStore, DataStoreWriteStream* stream)
                : _dataStore(dataStore)
                , _stream(stream)
            {
            }

            void Execute() override
            {
                printf("Close write stream: %s\n", _stream->Path().AsString().c_str());
                auto iter = _dataStore->_pathToOpenFile.find(_stream->Path());
                delete iter->second;
                _dataStore->_pathToOpenFile.erase(iter);
            }

        private:
            LocalFileDataStore* _dataStore;
            DataStoreWriteStream* _stream;
        };

        class OperationQueue
        {
        public:
            OperationQueue();
            void Push(IOperation* operation);
            void RunOneOperation();
            void Stop();
            bool IsFinishing();
        private:
            std::queue<IOperation*> _operations;
            std::mutex _mutex;
            std::condition_variable _condition;
            std::atomic_bool _finishing;
        };

        typedef std::map<DataStorePath, OpenFile*> OpenFileMap;
        OpenFileMap _pathToOpenFile;
        std::string _root;
        OperationQueue _operationQueue;
        std::thread _operationThread;
    };
}
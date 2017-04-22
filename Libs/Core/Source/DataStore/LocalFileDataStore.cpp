#include "DataStore/LocalFileDataStore.h"

Flourish::LocalFileDataStore::LocalFileDataStore(const char* root)
{

}

bool Flourish::LocalFileDataStore::Exists(const Flourish::DataStorePath& path) const
{
    return false;
}

void Flourish::LocalFileDataStore::OpenForRead(const Flourish::DataStorePath& path, Flourish::DataStoreReadCallback callback)
{

}

void Flourish::LocalFileDataStore::Close(DataStoreReadStream* stream)
{

}

bool Flourish::LocalFileDataStore::IsDir(const Flourish::DataStorePath& path) const
{
    return false;
}

bool Flourish::LocalFileDataStore::IsData(const Flourish::DataStorePath& path) const
{
    return false;
}

void Flourish::LocalFileDataStore::Enumerate(const Flourish::DataStorePath& dirPath, std::vector<Flourish::DataStorePath>& entries) const
{

}

void Flourish::LocalFileDataStore::EnqueueRead(DataStoreReadStream* stream, Flourish::DataBuffer* buffer, Flourish::DataStoreReadCallback callback)
{

}

void Flourish::LocalFileDataStore::OpenForWrite(const Flourish::DataStorePath& path, Flourish::DataStoreWriteCallback callback)
{

}

void Flourish::LocalFileDataStore::OpenForAppend(const Flourish::DataStorePath& path, Flourish::DataStoreWriteCallback callback)
{

}

void Flourish::LocalFileDataStore::Close(DataStoreWriteStream* stream)
{

}

void Flourish::LocalFileDataStore::EnqueueWrite(DataStoreWriteStream* stream, Flourish::DataBuffer* buffer, Flourish::DataStoreWriteCallback callback)
{

}

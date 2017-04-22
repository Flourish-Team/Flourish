#pragma once

#include <cstdio>
#include <cstdint>

namespace Flourish
{
    namespace FileSystem
    {
        bool CreateDirectoryTree(const char* path);
        bool DeleteDirectory(const char* path);
        FILE* OpenRead(const char* path);
        FILE* OpenWrite(const char* path);
        FILE* OpenAppend(const char* path);
        bool FileExists(const char* path);
        bool DirExists(const char* path);
        void Close(FILE* file);
        size_t Write(const uint8_t* data, size_t size, FILE* file);
        size_t Read(uint8_t* data, size_t size, FILE* file);
    }
}
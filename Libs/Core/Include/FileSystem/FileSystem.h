#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

namespace Flourish::FileSystem
{
	//Creates all directories along a file path
    bool CreateDirectoryTree(const char* path);

	//Delete target directory and all contents (including sub folders)
    bool DeleteDirectory(const char* path);

	//fills entries with the names of all files and directories in path
    void EnumerateDirectory(const char* path, std::vector<std::string>& entries);

	//Opens file at path for read
    FILE* OpenRead(const char* path);

	//Opens file at path for write
    FILE* OpenWrite(const char* path);

	//Opens file at path for append
    FILE* OpenAppend(const char* path);

	//Closes a file opened with any of the above file open functions
	void Close(FILE* file);

	//Returns true if a file exists at path
    bool FileExists(const char* path);

	//Returns true if a directory exists at path
    bool DirExists(const char* path);

	//write data to a opened file from buffer data, return value is the number of items written
    size_t Write(const uint8_t* data, size_t size, FILE* file);

	//reads data from a opened file into buffer data, return value is the number of items read
    size_t Read(uint8_t* data, size_t size, FILE* file);

}
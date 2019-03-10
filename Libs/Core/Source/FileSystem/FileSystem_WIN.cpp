#include "Platform/Platform.h"
#if FL_ENABLED(FL_PLATFORM_WINDOWS)

#include "FileSystem/FileSystem.h"

namespace Flourish::FileSystem
{
	void ReportFileError(const char* path, const char* functionName, int errorCode)
	{
		char* messageBuffer = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                             nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&messageBuffer), 0, nullptr);

		fprintf(stderr, "%s: %s error: %s\n", path, functionName, messageBuffer);
		LocalFree(messageBuffer);
	}

    bool CreateDirectoryTree(const char* path)
    {
		char dirPath[MAX_PATH];

	    strcpy_s(dirPath, path);
		int pathLen = strlen(dirPath);

		for(int i = 0; i < pathLen; ++i)
		{
			if(dirPath[i] == '/')
			{
				dirPath[i] = '\\';
			}
		}

		if(dirPath[pathLen- 1] != '\\')
		{
			strcat_s(dirPath,"\\");
		}

		char folder[MAX_PATH];
		memset(folder, 0, MAX_PATH * sizeof(char));


		const char* end = strchr(dirPath, '\\');

		while(end != nullptr)
		{
		    strncpy_s(folder, dirPath, end - dirPath + 1);
		    if(!CreateDirectoryA(folder, nullptr))
		    {
				DWORD error = GetLastError();
				if(error != ERROR_ALREADY_EXISTS)
				{
					ReportFileError(dirPath, "CreateDirectoryTree", error);
					return false;
				}
		    }
			Sleep(1);
		    end = strchr(++end, '\\');
		}

		return true;
    }

    bool DeleteDirectory(const char* path)
    {
	    char dirPath[MAX_PATH];
	    char fileName[MAX_PATH];

	    strcpy_s(dirPath, path);
	    strcat_s(dirPath,"\\*"); 

	    strcpy_s(fileName, path);
	    strcat_s(fileName,"\\");

		WIN32_FIND_DATAA findFileData;
	    HANDLE hFind = FindFirstFileA(dirPath, &findFileData); // find the first file

	    if(hFind == INVALID_HANDLE_VALUE) 
		{
	    	return false;
		}

	    strcpy_s(dirPath, fileName);
	        
	    bool bSearch = true;
	    while(bSearch) 
		{
	        if(FindNextFileA(hFind, &findFileData)) 
			{
				if(strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) 
				{
	            	continue;
				}
	            strcat_s(fileName,findFileData.cFileName);

	            if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				{
	                // we have found a directory, recurse
	                if(!DeleteDirectory(fileName)) 
					{ 
	                    FindClose(hFind); 
	                    return false;
	                }
	                RemoveDirectoryA(fileName);
	                strcpy_s(fileName,dirPath);
	            }
	            else
				{
	                if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					{
						//Clear the read only flag
						SetFileAttributesA(fileName, findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
					}

	                if(!DeleteFileA(fileName)) 
					{ 
	                    FindClose(hFind); 
	                    return false; 
	                }
	            	
	                strcpy_s(fileName,dirPath);
	            }
	        }
	        else 
			{
	            if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
	            {
		            bSearch = false;
	            }
	            else 
				{
	                // some error occured, close the handle and return FALSE
	                FindClose(hFind); 
	                return false;
	            }

	        }
	    }

	    FindClose(hFind);  // closing file handle
	 
	    return RemoveDirectoryA(path); // remove the empty directory
    }

    void EnumerateDirectory(const char* path, std::vector<std::string>& entries)
    {
		char dirPath[MAX_PATH];

	    strcpy_s(dirPath, path);
	    strcat_s(dirPath,"\\*"); 

		WIN32_FIND_DATAA findFileData;
	    HANDLE hFind = FindFirstFileA(dirPath, &findFileData); // find the first file

	    if(hFind == INVALID_HANDLE_VALUE) 
		{
	    	return;
		}
	        
	    bool bSearch = true;
	    while(bSearch) 
		{
	        if(FindNextFileA(hFind, &findFileData)) 
			{
				if(strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) 
				{
	            	continue;
				}

				entries.emplace_back(std::string(findFileData.cFileName));
	        }
	        else 
			{
	            bSearch = false;
	        }
	    }

	    FindClose(hFind);
    }

    FILE* OpenRead(const char* path)
    {
		FILE* fileHandle;

        if(fopen_s(&fileHandle, path, "rb") != 0)
        {
	        return nullptr;
        }

		return fileHandle;
    }

    FILE* OpenWrite(const char* path)
    {
		FILE* fileHandle;

        if(fopen_s(&fileHandle, path, "wb") != 0)
        {
	        return nullptr;
        }

		return fileHandle;
    }

    FILE* OpenAppend(const char* path)
    {
		FILE* fileHandle;

        if(fopen_s(&fileHandle, path, "ab") != 0)
        {
	        return nullptr;
        }

		return fileHandle;
    }

    bool FileExists(const char* path)
    {
		DWORD attributes = GetFileAttributesA(path);
		return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool DirExists(const char* path)
    {
        DWORD attributes = GetFileAttributesA(path);
		return attributes != INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY;
    }

    void Close(FILE* file)
    {
        fclose(file);
    }

    size_t Write(const uint8_t* data, size_t size, FILE* file)
    {
        return fwrite(data, sizeof(uint8_t), size, file);
    }

    size_t Read(uint8_t* data, size_t size, FILE* file)
    {
        return fread(data, sizeof(uint8_t), size, file);
    }
}

#endif

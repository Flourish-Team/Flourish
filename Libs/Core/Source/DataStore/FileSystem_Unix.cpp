#include "DataStore/FileSystem.h"

#include "Platform/Platform.h"

#if FL_ENABLED(FL_PLATFORM_UNIX)

#include <errno.h>
#include <fts.h>
#include <sys/stat.h>
#include <dirent.h>

namespace Flourish
{
    namespace FileSystem
    {
        bool CreateDirectoryTree(const char* path)
        {
            const size_t len = strlen(path);
            char _path[PATH_MAX];
            char* p;

            errno = 0;

            if (len > sizeof(_path) - 1)
            {
                errno = ENAMETOOLONG;
                return false;
            }
            strcpy(_path, path);

            for (p = _path + 1; *p; p++)
            {
                if (*p == '/')
                {
                    // Temporarily truncate
                    *p = '\0';

                    if (mkdir(_path, S_IRWXU) != 0)
                    {
                        if (errno != EEXIST)
                        {
                            return false;
                        }
                    }

                    *p = '/';
                }
            }

            if (mkdir(_path, S_IRWXU) != 0)
            {
                if (errno != EEXIST)
                {
                    return false;
                }
            }

            return true;
        }

        bool DeleteDirectory(const char* path)
        {
            // Taken from: http://stackoverflow.com/a/27808574/595270
            bool ret = true;
            FTS* ftsp = NULL;
            FTSENT* curr;

            // Cast needed (in C) because fts_open() takes a "char * const *", instead
            // of a "const char * const *", which is only allowed in C++. fts_open()
            // does not modify the argument.
            char* files[] = {(char*) path, NULL};

            // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
            //                in multithreaded programs
            // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
            //                of the specified directory
            // FTS_XDEV     - Don't cross filesystem boundaries
            ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
            if (!ftsp)
            {
                fprintf(stderr, "%s: fts_open failed: %s\n", path, strerror(errno));
                ret = false;
                goto finish;
            }

            while ((curr = fts_read(ftsp)))
            {
                switch (curr->fts_info)
                {
                    case FTS_NS:
                    case FTS_DNR:
                    case FTS_ERR:
                        fprintf(stderr, "%s: fts_read error: %s\n",
                                curr->fts_accpath, strerror(curr->fts_errno));
                        break;

                    case FTS_DC:
                    case FTS_DOT:
                    case FTS_NSOK:
                        // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
                        // passed to fts_open()
                        break;

                    case FTS_D:
                        // Do nothing. Need depth-first search, so directories are deleted
                        // in FTS_DP
                        break;

                    case FTS_DP:
                    case FTS_F:
                    case FTS_SL:
                    case FTS_SLNONE:
                    case FTS_DEFAULT:
                        if (remove(curr->fts_accpath) < 0)
                        {
                            fprintf(stderr, "%s: Failed to remove: %s\n",
                                    curr->fts_path, strerror(errno));
                            ret = false;
                        }
                        break;
                }
            }

            finish:
            if (ftsp)
            {
                fts_close(ftsp);
            }

            return ret;
        }

        void EnumerateDirectory(const char* path, std::vector<std::string>& entries)
        {
            auto dir = opendir(path);
            if (dir == nullptr)
            {
                return;
            }
            struct dirent* dirEntry;
            while ((dirEntry = readdir(dir)) != nullptr)
            {
                if (strcmp(dirEntry->d_name, ".") == 0 ||
                    strcmp(dirEntry->d_name, "..") == 0)
                {
                    continue;
                }
                entries.push_back(std::string(dirEntry->d_name));
            }
            closedir(dir);
        }

        FILE* OpenRead(const char* path)
        {
            return fopen(path, "rb");
        }

        FILE* OpenWrite(const char* path)
        {
            return fopen(path, "wb");
        }

        FILE* OpenAppend(const char* path)
        {
            return fopen(path, "ab");
        }

        bool FileExists(const char* path)
        {
            struct stat sb;
            return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
        }

        bool DirExists(const char* path)
        {
            struct stat sb;
            return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
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
}

#endif

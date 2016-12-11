#include "FileSystem/FilePath.h"

namespace Flourish
{
    FilePath::FilePath(const char* path)
    {
        // strlen works for UTF-8 encoded strings
        auto length = strlen(path);
        // We know the string will be at most 'length', it might be shorter
        _contentsAsString.resize(length);
        uint32_t destIdx = 0;
        for(uint32_t sourceIdx = 0; sourceIdx < length; sourceIdx++)
        {
            if(path[sourceIdx] == '\\')
            {
                _contentsAsString[destIdx] = '/';
            }
            else
            {
                _contentsAsString[destIdx] = path[sourceIdx];
            }
            destIdx++;
        }
    }
    
    FilePath::FilePath(const std::string& path)
        : FilePath(path.c_str())
    {
    }
    
    std::string FilePath::AsString() const
    {
        return _contentsAsString;
    }
    
    FilePath& FilePath::GetDirectory()
    {
        auto lastSlash = _contentsAsString.find_last_of('/');
        if(lastSlash != std::string::npos)
        {
            _contentsAsString.resize(lastSlash);
            return *this;
        }
        _contentsAsString.resize(0);
        return *this;
    }
    
    FilePath& FilePath::GetFileName()
    {
        return *this;
    }
}

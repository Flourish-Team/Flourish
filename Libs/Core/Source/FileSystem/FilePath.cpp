#include "FileSystem/FilePath.h"

namespace Flourish
{
    FilePath::FilePath(const char* path)
        : _contentsAsString(path)
    {
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
        auto lastSlash = GetLastDirSeperatorIdx();
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
        auto lastSlash = GetLastDirSeperatorIdx();
        if(lastSlash != std::string::npos)
        {
            _contentsAsString.erase(0, lastSlash + 1);
        }
        return *this;
    }
    
    FilePath& FilePath::GetFileNameWithoutExtension()
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if(lastSlash != std::string::npos)
        {
            _contentsAsString.erase(0, lastSlash + 1);
        }
        auto lastDot = _contentsAsString.find_last_of('.');
        if(lastDot != std::string::npos)
        {
            _contentsAsString.erase(lastDot);
        }
        return *this;
    }
    
    std::string::size_type FilePath::GetLastDirSeperatorIdx() const
    {
        if(_contentsAsString.empty())
        {
            return std::string::npos;
        }
        for(std::string::size_type charIdx = _contentsAsString.length() - 1; charIdx != 0; charIdx--)
        {
            if(_contentsAsString[charIdx] == '/' ||
               _contentsAsString[charIdx] == '\\')
            {
                return charIdx;
            }
        }
        
        return std::string::npos;
    }
}

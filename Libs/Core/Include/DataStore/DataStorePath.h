#pragma once

#include <string>

namespace Flourish
{
    // Path to an item in a data store.
    // Similar to a normal file-path but has these restrictions
    // - Always relative
    // - Always uses / as a seperator
    // - Cannot include '.' or '..'
    class DataStorePath
    {
    public:
        DataStorePath(const char* path);
        DataStorePath(const std::string& path);
        
        std::string AsString() const;
        DataStorePath& GetDirectory();
        DataStorePath& GetFileName();
        DataStorePath& GetFileNameWithoutExtension();
    
    private:
        std::string::size_type GetLastDirSeperatorIdx() const;
        void Validate() const;
        
        std::string _contentsAsString;
    };
}

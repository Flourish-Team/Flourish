#pragma once

#include <string>

namespace Flourish
{
    // A platform agnostic file path.
    // Uses / as a seperator (and will convert \\ automatically).
    // Assumes input is UTF-8 encoded
    class FilePath
    {
    public:
        FilePath(const char* path);
        FilePath(const std::string& path);
        
        std::string AsString() const;
        FilePath& GetDirectory();
        FilePath& GetFileName();
    
    private:
        std::string _contentsAsString;
    };
}

#include "DataStore/DataStorePath.h"

#include "Debug/Assert.h"

namespace Flourish
{
    DataStorePath::DataStorePath(const char* path)
            : _contentsAsString(path)
    {
        Validate();
    }

    bool DataStorePath::operator==(const DataStorePath& rhs) const
    {
        return _contentsAsString == rhs._contentsAsString;
    }

    bool DataStorePath::operator!=(const DataStorePath& rhs) const
    {
        return !(rhs == *this);
    }

    DataStorePath::DataStorePath(const std::string& path)
            : DataStorePath(path.c_str())
    {
    }

    std::string DataStorePath::AsString() const
    {
        return _contentsAsString;
    }

    DataStorePath& DataStorePath::GetDirectory()
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if (lastSlash != std::string::npos)
        {
            _contentsAsString.resize(lastSlash);
            return *this;
        }
        _contentsAsString.resize(0);
        return *this;
    }

    DataStorePath& DataStorePath::GetFileName()
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if (lastSlash != std::string::npos)
        {
            _contentsAsString.erase(0, lastSlash + 1);
        }
        return *this;
    }

    DataStorePath& DataStorePath::GetFileNameWithoutExtension()
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if (lastSlash != std::string::npos)
        {
            _contentsAsString.erase(0, lastSlash + 1);
        }
        auto lastDot = _contentsAsString.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            _contentsAsString.erase(lastDot);
        }
        return *this;
    }

    std::string::size_type DataStorePath::GetLastDirSeperatorIdx() const
    {
        return _contentsAsString.find_last_of('/');
    }

    void DataStorePath::Validate() const
    {
        if (_contentsAsString.length() <= 0)
        {
            // Empty path is valid, being the directory of the
            // root of the data store
            return;
        }

        FL_ASSERT_MSG(_contentsAsString[0] != '/', "Data store path must be relative");
        FL_ASSERT_MSG(_contentsAsString.find("..") == std::string::npos, "Data Store Path cannot contain ..");
        FL_ASSERT_MSG(_contentsAsString.find('\\') == std::string::npos, "Data Store Path cannot contain \\");
    }

}

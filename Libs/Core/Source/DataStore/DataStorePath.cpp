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

    DataStorePath DataStorePath::GetDirectory() const
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if (lastSlash != std::string::npos)
        {
            return DataStorePath(_contentsAsString.substr(0, lastSlash));
        }
        return DataStorePath("");
    }

    DataStorePath DataStorePath::GetFileName() const
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        if (lastSlash != std::string::npos)
        {
            return DataStorePath(_contentsAsString.substr(lastSlash + 1));
        }
        return DataStorePath(_contentsAsString);
    }

    DataStorePath DataStorePath::GetFileNameWithoutExtension() const
    {
        auto lastSlash = GetLastDirSeperatorIdx();
        auto strValue = _contentsAsString;
        if (lastSlash != std::string::npos)
        {
            strValue.erase(0, lastSlash + 1);
        }
        auto lastDot = strValue.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            strValue.erase(lastDot);
        }
        return DataStorePath(strValue);
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

    bool operator<(const DataStorePath& lhs, const DataStorePath& rhs)
    {
        return lhs.AsString() < rhs.AsString();
    }
}

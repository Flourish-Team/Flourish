#include "DataStore/DataBuffer.h"

namespace Flourish
{
    DataBuffer::DataBuffer(size_t size)
        : _data(new uint8_t[size])
        , _size(size)
        , _writeHead(0)
        , _readHead(0)
    {
    }

    DataBuffer::~DataBuffer() noexcept
    {
        delete[] _data;
    }

    DataBuffer::DataBuffer(const DataBuffer& other)
        : _data(new uint8_t[other._size])
        , _size(other._size)
        , _writeHead(other._writeHead)
        , _readHead(other._readHead)
    {
        memcpy(_data, other._data, _size);
    }

    DataBuffer::DataBuffer(DataBuffer&& other) noexcept
        : _data(other._data)
        , _size(other._size)
        , _writeHead(other._writeHead)
        , _readHead(other._readHead)
    {
        other._data = nullptr;
    }

    DataBuffer& DataBuffer::operator=(const DataBuffer& other)
    {
        DataBuffer tmp(other);
        *this = std::move(tmp);
        return *this;
    }

    DataBuffer& DataBuffer::operator=(DataBuffer&& other) noexcept
    {
        delete[] _data;
        _data = other._data;
        other._data = nullptr;
        _size = other._size;
        _readHead = other._readHead;
        _writeHead = other._writeHead;
        return *this;
    }

    void DataBuffer::Write(const void* dataToWrite, size_t sizeOfData)
    {
        FL_ASSERT_MSG(sizeOfData <= SpaceLeftToWrite(),
                      "Attempted to write %zu bytes to buffer that has only %zu bytes left", sizeOfData,
                      SpaceLeftToWrite());
        memcpy(&_data[_writeHead], dataToWrite, std::min(sizeOfData, SpaceLeftToWrite()));
        _writeHead += sizeOfData;
    }

    size_t DataBuffer::SpaceLeftToWrite() const
    {
        return _size - _writeHead;
    }

    const void* DataBuffer::Data() const
    {
        return &_data[_readHead];
    }

    size_t DataBuffer::DataAvailableToRead() const
    {
        return _writeHead - _readHead;
    }

    void DataBuffer::MarkAsRead(size_t data)
    {
        _readHead += data;
    }

    size_t DataBuffer::Size() const
    {
        return _size;
    }

    void DataBuffer::Clear()
    {
        _writeHead = 0;
        _readHead = 0;
    }
}

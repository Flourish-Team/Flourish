#pragma once

#include <algorithm>

#include "Debug/Assert.h"

namespace Flourish
{
    // Simple buffer that can be written to/read from
    class DataBuffer
    {
    public:
        DataBuffer(size_t size);
        virtual ~DataBuffer() noexcept;
        DataBuffer(const DataBuffer& other);
        DataBuffer(DataBuffer&& other) noexcept;
        DataBuffer& operator=(const DataBuffer& other);
        DataBuffer& operator=(DataBuffer&& other) noexcept;

        void Write(const void* dataToWrite, size_t sizeOfData);
        size_t SpaceLeftToWrite() const;
        const void* ReadData() const;
        void* WriteData();
        size_t DataAvailableToRead() const;
        void MarkAsRead(size_t data);
        void MarkAsWritten(size_t data);
        size_t Size() const;
        void Clear();

    private:
        uint8_t* _data;
        size_t _size;
        size_t _writeHead;
        size_t _readHead;
    };
}

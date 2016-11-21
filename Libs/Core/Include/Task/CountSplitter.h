#pragma once

#include <cstdint>

namespace Flourish
{
    template<typename DataType>
    class CountSplitter
    {
    public:
        explicit CountSplitter(uint32_t count)
            : _count(count)
        {
        }
        
        bool ShouldSplit(DataType*, uint32_t dataCount)
        {
            return dataCount > _count;
        }
        
    private:
        uint32_t _count;
    };
}

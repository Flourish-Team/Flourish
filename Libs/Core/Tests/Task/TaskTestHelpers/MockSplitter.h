#pragma once

namespace Flourish
{
    namespace TaskTestHelpers
    {
        template<typename DataType>
        class MockSplitter
        {
        public:
            MockSplitter()
            : _data(nullptr)
            , _count(0)
            {
            }
            
            bool ShouldSplit(DataType* data, uint32_t count)
            {
                _data = data;
                _count = count;
                return false;
            }
            
            DataType* _data;
            uint32_t _count;
        };
    }
}

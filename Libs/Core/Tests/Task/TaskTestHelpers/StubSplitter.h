#pragma once

namespace Flourish
{
    namespace TaskTestHelpers
    {
        template<typename DataType>
        class StubSplitter
        {
        public:
            StubSplitter(bool shouldSplit)
            : _shouldSplit(shouldSplit)
            {
                
            }
            
            bool ShouldSplit(DataType*, uint32_t)
            {
                return _shouldSplit;
            }
            
            bool _shouldSplit;
        };
    }
}

#pragma once

#include "Task/TaskTestHelpers/ReplaySplitter.h"

namespace Flourish
{
    namespace TaskTestHelpers
    {
        template<typename DataType, uint32_t count>
        class RecordReplaySplitter : public ReplaySplitter<DataType, count>
        {
        public:
            RecordReplaySplitter(bool shouldSplit[count])
            : ReplaySplitter<DataType, count>(shouldSplit)
            , _dataList()
            , _dataCountList()
            {
                for(uint32_t index = 0; index < count; index++)
                {
                    _dataList[index] = nullptr;
                    _dataCountList[index] = 0;
                }
            }
            
            bool ShouldSplit(DataType* data, uint32_t dataCount) override
            {
                _dataList[ReplaySplitter<DataType, count>::_currentResonse] = data;
                _dataCountList[ReplaySplitter<DataType, count>::_currentResonse] = dataCount;
                return ReplaySplitter<DataType, count>::ShouldSplit(data, dataCount);
            }
            
            DataType* _dataList[count];
            uint32_t _dataCountList[count];
        };
    }
}

#pragma once

#include "Test.h"

namespace Flourish
{
    namespace TaskTestHelpers
    {
        template<typename DataType, uint32_t count>
        class ReplaySplitter
        {
        public:
            ReplaySplitter(bool shouldSplit[count])
            : _shouldSplit()
            , _currentResonse(0)
            {
                for(uint32_t index = 0; index < count; index++)
                {
                    _shouldSplit[index] = shouldSplit[index];
                }
            }
            
            virtual bool ShouldSplit(DataType*, uint32_t)
            {
                if(_currentResonse >= count)
                {
                    return false;
                }
                auto response = _shouldSplit[_currentResonse];
                _currentResonse++;
                return response;
            }
            
            void AssertUsedAllExpectedResponses()
            {
                EXPECT_EQUAL(_currentResonse, count) << "Did not use all the expected responses";
            }
            
            bool _shouldSplit[count];
            uint32_t _currentResonse;
        };
    }
}

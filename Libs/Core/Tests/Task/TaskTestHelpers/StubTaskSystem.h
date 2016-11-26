#pragma once

#include "Task/Task.h"

namespace Flourish
{
    namespace TaskTestHelpers
    {
        class StubTaskSystem
        {
        public:
            template<typename Callable>
            WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
            {
                return WorkItem(WorkItemFunction(callable), data);
            }
            
            TaskId BeginAdd(WorkItem)
            {
                return TaskId(0);
            }
            
            void AddChild(TaskId, TaskId) {}
            
            void FinishAdd(TaskId){}
        };
    }
}

#pragma once

#include "Task/Task.h"

namespace Flourish
{
    namespace TaskTestHelpers
    {
        class MockTaskSystem
        {
        public:
            MockTaskSystem()
            : _taskAdded(false)
            {
            }
            
            template<typename Callable>
            WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
            {
                return WorkItem(WorkItemFunction(callable), data);
            }
            
            TaskId BeginAdd(WorkItem)
            {
                return TaskId(4);
            }
            
            void FinishAdd(TaskId id)
            {
                EXPECT_EQUAL(id, 4u) << "Incorrect TaskId was passed to FinishAdd";
                _taskAdded = true;
            }
            
            void AddChild(TaskId, TaskId) {}
            
            bool _taskAdded;
        };
    }
}

#pragma once

#include "Task/Task.h"

namespace Flourish
{
    namespace TaskTestHelpers
    {
        template<uint32_t expectedNumTasks>
        class RecordTaskSystem
        {
        public:
            RecordTaskSystem()
            : _numTasksAdded(0)
            , _parentTasks()
            , _nextTaskId(0)
            {
                for(uint32_t index = 0; index < expectedNumTasks; index++)
                {
                    _parentTasks[index] = -1;
                }
            }
            
            template<typename Callable>
            WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
            {
                return WorkItem(WorkItemFunction(callable), data);
            }
            
            TaskId BeginAdd(WorkItem)
            {
                return TaskId(_nextTaskId++);
            }
            
            void FinishAdd(TaskId)
            {
                _numTasksAdded++;
            }
            
            void AddChild(TaskId parent, TaskId child)
            {
                _parentTasks[child] = parent;
            }
            
            void AssertExpectedNumberOfTasksAdded()
            {
                EXPECT_EQUAL(_numTasksAdded, expectedNumTasks) << "Incorrect number of tasks added";
            }
            
            uint32_t _numTasksAdded;
            TaskId _parentTasks[expectedNumTasks];
            TaskId _nextTaskId;
        };
    }
}

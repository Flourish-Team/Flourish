#pragma once

#include <functional>
#include <utility>

#include "Task/Task.h"

namespace Flourish
{
    template<typename DataType, typename Splitter, typename TaskSystem>
    void ParallelForFunc(TaskId parentTask, DataType* data, int32_t dataCount, Splitter* splitter, std::function<void(DataType*,uint32_t)> workFunc, TaskSystem* taskSystem)
    {
        if(!splitter->ShouldSplit(data, dataCount))
        {
            auto taskId = taskSystem->BeginAdd(taskSystem->WorkItemWithTaskAllocator([=](void*){
                workFunc(data, dataCount);
            }));
            taskSystem->AddChild(parentTask, taskId);
            taskSystem->FinishAdd(taskId);
        }
        else
        {
            auto leftSize = dataCount / 2u;
            ParallelForFunc(parentTask, data, leftSize, splitter, workFunc, taskSystem);
            ParallelForFunc(parentTask, data + leftSize, dataCount - leftSize, splitter, workFunc, taskSystem);
        }
    }
    
    template<typename DataType, typename Splitter, typename TaskSystem = class TaskManager>
    class ParallelFor
    {
        typedef std::function<void(DataType*,uint32_t)> WorkFunc;
    public:
        ParallelFor(DataType* data, uint32_t dataCount, Splitter* splitter, WorkFunc workFunc, TaskSystem* taskSystem)
            : _data(data)
            , _dataCount(dataCount)
            , _splitter(splitter)
            , _workFunc(std::move(workFunc))
            , _taskSystem(taskSystem)
        {
        }
        
        TaskId Run()
        {
            auto taskId = _taskSystem->BeginAdd(WorkItem::Empty());
            ParallelForFunc(taskId, _data, _dataCount, _splitter, _workFunc, _taskSystem);
            _taskSystem->FinishAdd(taskId);
            return taskId;
        }
        
    private:
        DataType* _data;
        uint32_t _dataCount;
        Splitter* _splitter;
        WorkFunc _workFunc;
        TaskSystem* _taskSystem;
    };
}

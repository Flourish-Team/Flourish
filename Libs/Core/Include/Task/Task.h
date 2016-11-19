#pragma once

#include <atomic>
#include <cstdint>

namespace Flourish
{
    typedef void (*WorkItemFunction)(void*);
    typedef uint32_t TaskId;
    
    // A work item is a function and some data
    // it is assumed that the function knows what
    // to do with the void* data
    struct WorkItem
    {
    public:
        WorkItemFunction _function;
        void* _data;
        
        WorkItem(WorkItemFunction function, void* data)
        {
            _function = function;
            _data = data;
        }
        
        WorkItem(WorkItemFunction function)
            : WorkItem(function, nullptr)
        {
        }
        
        static WorkItem Empty()
        {
            return WorkItem(nullptr);
        }
        
        void operator()()
        {
            _function(_data);
        }
    };

    struct Task
    {
        TaskId _id;
        WorkItem _workItem;
        TaskId _parentId;
        bool _added;
        std::atomic_bool _complete;
        TaskId _dependency;
        uint8_t padding[24];
        
        Task()
            : Task(WorkItem::Empty())
        {
            
        }
        
        Task(const WorkItem& workItem)
            : _id(0)
            , _workItem(workItem)
            , _parentId(0)
            , _added(false)
            , _complete(false)
            , _dependency(0)
        {
            _workItem = workItem;
        }
    };
}

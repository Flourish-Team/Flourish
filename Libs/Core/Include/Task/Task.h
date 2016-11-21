#pragma once

#include <atomic>
#include <cstdint>
#include <functional>

namespace Flourish
{
    typedef std::function<void(void*)> WorkItemFunction;
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
            return WorkItem([](void*){});
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
        std::atomic_uint _openWorkItems;
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
            , _openWorkItems(1)
            , _dependency(0)
        {
            _workItem = workItem;
        }
    };
}

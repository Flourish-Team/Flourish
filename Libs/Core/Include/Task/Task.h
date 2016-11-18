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
        WorkItemFunction _function;
        void* _data;
        
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
    };
}

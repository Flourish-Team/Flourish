#pragma once

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
    
    // AT TODO:
    // Thread affinity
    struct Task
    {
        TaskId _id;
        WorkItem _workItem;
        TaskId _parentId;
        int32_t _openWorkItems;
        int32_t _priority;
        TaskId _dependency;
    };
}

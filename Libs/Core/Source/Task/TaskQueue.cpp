#include "Task/TaskQueue.h"

#include <stdatomic.h>

#include "Task/Task.h"

namespace Flourish
{
    // Mask used for cheap modulo operators. Only works if MaxumuymNumerOfTasks is a power of 2
    static const uint32_t Mask = TaskQueue::MaximumNumberOfTasks - 1u;
    
    TaskQueue::TaskQueue()
        : _top(0)
        , _bottom(0)
        , _tasks()
    {
    }
    
    // We assume that this method can only be called concurrently
    // with Steal.
    // So, Steal reads from _bottom and writes to _top
    // Push doesn't touch _top, and if Steal reads an incorrect
    // _bottom (e.g. before it has been updated at the end of the function)
    // it will just return nullptr and no harm has been done
    void TaskQueue::Push(Task* task)
    {
        long currentBottom = _bottom;
        _tasks[currentBottom & Mask] = task;
        _bottom = currentBottom + 1;
    }
    
    // We assume that this method can only be called concurrently
    // with Steal.
    Task* TaskQueue::Pop()
    {
        long newBottom = _bottom - 1;
        _bottom = newBottom;
        
        long currentTop = _top;
        if (currentTop <= newBottom)
        {
            // The queue has something in it
            auto task = _tasks[newBottom & Mask];
            if (currentTop != newBottom)
            {
                // More than one item in the queue, we don't have to worry about
                // conflicting with a steal
                return task;
            }
            
            // This is the last item in the queue
            if (!_top.compare_exchange_weak(currentTop, currentTop + 1))
            {
                // Steal got there first
                task = nullptr;
            }
            
            _bottom = currentTop + 1;
            return task;
        }
        // Queue was empty
        _bottom = currentTop;
        return nullptr;
    }
    
    // We assume this method can be called concurrently with either
    // Push, Pop, or another Steal
    Task* TaskQueue::Steal()
    {
        long currentTop = _top;
        long currentBottom = _bottom;
        if(currentTop < currentBottom)
        {
            // The queue has something in it
            auto task = _tasks[currentTop & Mask];
            if(!_top.compare_exchange_weak(currentTop, currentTop + 1))
            {
                // Something else (probably another Steal) took this item
                return nullptr;
            }
            return task;
        }
        // Queue is empty
        return nullptr;
    }
}

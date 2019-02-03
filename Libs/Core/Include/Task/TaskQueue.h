#pragma once

#include <atomic>
#include <cstdint>

namespace Flourish
{
    struct Task;
    
    // TaskQueue assumes that Push and Pop will only be called on the same
    // thread, (so Push and Pop cannot be called concurrently). However Steal
    // is assumed to be called on a different thread, so may be run concurrently
    // with either Pop, Push, or another Steal
    class TaskQueue
    {
    public:
        static const uint32_t MAXIMUM_NUMBER_OF_TASKS = 4096u;

		// Mask used for cheap modulo operators. Only works if MAXIMUM_NUMBER_OF_TASKS is a power of 2
		static const uint32_t MASK = TaskQueue::MAXIMUM_NUMBER_OF_TASKS - 1u;
        
        TaskQueue();
        void Push(Task* task);
        Task* Pop();
        Task* Steal();
        
    private:
        std::atomic_long _top;
        std::atomic_long _bottom;
        Task* _tasks[MAXIMUM_NUMBER_OF_TASKS];
    };
}

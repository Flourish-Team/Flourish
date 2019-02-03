#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Task/Task.h"
#include "Task/TaskThreadGate.h"

namespace Flourish
{
    class TaskQueue;
	class TaskManager
	{
	public:
        
        static const int32_t AutomaticallyDetectNumThreads = -1;
        static const uint32_t MaxConcurrentTasks = 4096;
        
        TaskManager(int32_t numThreads = TaskManager::AutomaticallyDetectNumThreads);
		~TaskManager();

		TaskManager(TaskManager& other) = delete;
		TaskManager& operator=(const TaskManager&) = delete;

		TaskManager(TaskManager&& other) = delete;
		TaskManager& operator=(TaskManager&&) = delete;

		TaskId BeginAdd(WorkItem workItem);
        TaskId AddDependentTasks(TaskId root, std::vector<TaskId> dependencies);
        void AddChild(TaskId parent, TaskId child);
        void FinishAdd(TaskId id);
        TaskId AddTaskWithNoChildrenOrDependencies(WorkItem workItem);
		void Wait(TaskId id);
        template<typename Callable>
        WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
        {
            // TODO: Make this use an allocator
            return WorkItem(WorkItemFunction(callable), data);
        }

	private:
        void CreateTasks();
		void CreateAndStartWorkerThreads();
		void WorkerThreadFunc(int32_t threadIdx);
        void WaitForTaskAndExecute();
        int32_t GetIdealNumThreads();
        Task* GetTaskToExecute();
        Task* GetTaskFromId(TaskId id);
        void FinishTask(Task* task);
        void CreateTaskQueueForCurrentThread(uint32_t threadIdx);
        
        std::atomic_uint _nextTaskId;
        Task* _allTasks;
        uint32_t _numThreads;
		std::thread* _workerThreads;
        TaskQueue** _taskQueues;
        static thread_local TaskQueue* _currentThreadTaskQueue;
        TaskThreadGate<std::condition_variable> _taskThreadGate;
        std::atomic_bool _exiting;
	};
}

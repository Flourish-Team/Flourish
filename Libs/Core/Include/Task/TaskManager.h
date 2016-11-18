#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>

#include "Task/Task.h"

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

		TaskId BeginAdd(WorkItem workItem);
        TaskId AddDependantTasks(TaskId root, std::vector<TaskId> dependancies);
        void AddChild(TaskId parent, TaskId child);
        void FinishAdd(TaskId id);
		void Wait(TaskId id);

	private:
        void CreateTasks();
		void CreateAndStartWorkerThreads();
		void WorkerThreadFunc(int32_t threadIdx);
        void WaitForTaskAndExecute(int32_t threadIdx);
        int32_t GetIdealNumThreads();
        Task* GetTaskToExecute(uint32_t currentThreadQueueIdx);
        Task* GetTaskFromId(TaskId id);
        void FinishTask(Task* task);
        
        std::atomic_uint _nextTaskId;
        Task* _allTasks;
        uint32_t _numThreads;
        std::atomic_uint _numQueuedTasks;
		std::thread* _workerThreads;
        TaskQueue** _taskQueues;
        std::mutex _waitForWorkMutex;
        std::condition_variable _workMaybeAvailable;
        std::atomic_bool _exiting;
	};
}

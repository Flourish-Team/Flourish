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

		TaskId BeginAdd(WorkItem workItem, TaskId dependsOn = 0);
		void FinishAdd(TaskId id);
        void AddChild(TaskId parent, TaskId child);
		void Wait(TaskId id);

	private:
        void CreateTasks();
		void CreateAndStartWorkerThreads();
		void WorkerThreadFunc(int32_t threadIdx);
        void WaitForTaskAndExecute(int32_t threadIdx);
        int32_t GetIdealNumThreads();
        // Attempt to get a task for the current thread to execute. Will first try
        // to get one from the current threads queue, if not, it'll try to steal one
        // if neither of those gets a task, returns nullptr. Also returns nullptr
        // if the task it does get has a dependancy that is not finished
        Task* GetTaskToExecute(uint32_t currentThreadQueueIdx);
        Task* GetTaskFromId(TaskId id);
        void FinishTask(Task* task);
        
        std::atomic_uint _nextTaskId;
        Task* _allTasks;
        int32_t _numThreads;
		std::thread* _workerThreads;
        TaskQueue** _taskQueues;
        std::mutex _waitForWorkMutex;
        std::condition_variable _workMaybeAvailable;
        std::atomic_bool _exiting;
	};
}

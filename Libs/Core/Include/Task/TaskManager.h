#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Task/Task.h"

namespace Flourish
{
	class TaskManager
	{
	public:
        
        static const int32_t AutomaticallyDetectNumThreads = -1;
        
        TaskManager(int32_t numThreads = TaskManager::AutomaticallyDetectNumThreads);
		~TaskManager();

		TaskId BeginAdd(WorkItem workItem, TaskId dependsOn = 0);
		void FinishAdd(TaskId id);
		void AddChild(TaskId parentId, TaskId childId);
		void Wait(TaskId id);

	private:
		void CreateAndStartWorkerThreads();
		void WorkerThreadFunc();
		bool TaskQueueHasItems() const;
		std::vector<Task>::iterator GetHighestPriorityTask();
		void DecrementOpenWorkItems(TaskId id);
		void NonThreadSafeDecrementOpenWorkItemsRecursive(TaskId id);
		bool TaskPending(TaskId id);
        int32_t GetIdealNumThreads();

		TaskId _nextId;
        int32_t _numThreads;
		std::thread* _workerThreads;
		std::vector<Task> _taskQueue;
		std::mutex _taskQueueMutex;
		std::condition_variable _workAddedToTaskQueue;
		std::vector<Task> _openTaskQueue;
		std::mutex _openTaskQueueMutex;
        std::atomic_bool _exiting;
	};
}

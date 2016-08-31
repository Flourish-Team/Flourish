#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Flourish
{
	typedef void (*TaskFunction)(void*);
	typedef uint32_t TaskId;

	// AT TODO: 
	// Thread affinity
	// We probably want something better than just a function pointer
	struct Task
	{
		TaskId _id;
		TaskFunction _function;
		TaskId _parentId;
		int32_t _openWorkItems;
		int32_t _priority;
		TaskId _dependency;
	};

	class TaskManager
	{
	public:
		TaskManager();
		~TaskManager();

		TaskId BeginAdd(TaskFunction task, TaskId dependsOn = 0);
		void FinishAdd(TaskId id);
		void AddChild(TaskId parentId, TaskId childId);

	private:
		void CreateAndStartWorkerThreads();
		void WorkerThreadFunc();
		bool TaskQueueHasItems();
		std::vector<Task>::iterator GetHighestPriorityTask();
		void DecrementOpenWorkItems(TaskId id);
		void NonThreadSafeDecrementOpenWorkItemsRecursive(TaskId id);

		TaskId _nextId;
		std::thread* _workerThreads;
		std::vector<Task> _taskQueue;
		std::mutex _taskQueueMutex;
		std::condition_variable _workAddedToTaskQueue;
		std::vector<Task> _openTaskQueue;
		std::mutex _openTaskQueueMutex;
	};
}
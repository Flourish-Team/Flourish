#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

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

	class TaskManager
	{
	public:
		TaskManager();
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

		TaskId _nextId;
		std::thread* _workerThreads;
		std::vector<Task> _taskQueue;
		std::mutex _taskQueueMutex;
		std::condition_variable _workAddedToTaskQueue;
		std::vector<Task> _openTaskQueue;
		std::mutex _openTaskQueueMutex;
	};
}
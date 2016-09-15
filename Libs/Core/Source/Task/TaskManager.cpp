#include "Task/TaskManager.h"

#include <Windows.h>

namespace Flourish
{
	TaskManager::TaskManager()
		: _nextId(1)
		, _workerThreads(new std::thread[5])
		, _taskQueue()
		, _taskQueueMutex()
		, _workAddedToTaskQueue()
		, _openTaskQueue()
		, _openTaskQueueMutex()
	{
		CreateAndStartWorkerThreads();
	}

	TaskManager::~TaskManager()
	{
		delete[] _workerThreads;
	}

	TaskId TaskManager::BeginAdd(WorkItem workItem, TaskId dependsOn /* = 0*/)
	{
		std::lock_guard<std::mutex>openTaskLock(_openTaskQueueMutex);
		Task task = {};
		task._id = _nextId++;
		task._workItem = workItem;
		task._dependency = dependsOn;
		// Setting open work items to 2 prevents race conditions
		// when adding children
		// Because even if the taskFunction for this task is complete
		// it will still have an openWorkItems of 1 until
		// FinishAdd is called
		task._openWorkItems = 2;
		_openTaskQueue.push_back(task);
		std::lock_guard<std::mutex>taskQueue(_taskQueueMutex);
		_taskQueue.push_back(task);
		return task._id;
	}

	void TaskManager::FinishAdd(TaskId id)
	{
		DecrementOpenWorkItems(id);
		_workAddedToTaskQueue.notify_one();
	}

	void TaskManager::AddChild(TaskId parentId, TaskId childId)
	{
		std::lock_guard<std::mutex>lock(_openTaskQueueMutex);
		for (auto openTaskIter = _openTaskQueue.begin(); openTaskIter != _openTaskQueue.end(); ++openTaskIter)
		{
			if (openTaskIter->_id == childId)
			{
				openTaskIter->_parentId = parentId;
				if (openTaskIter->_openWorkItems <= 0)
				{
					// Child task is already complete
					// Don't bother telling the parent it has a new work item
					return;
				}
			}
		}
		for (auto openTaskIter = _openTaskQueue.begin(); openTaskIter != _openTaskQueue.end(); ++openTaskIter)
		{
			if (openTaskIter->_id == parentId)
			{
				openTaskIter->_openWorkItems++;
				return;
			}
		}
	}

	void TaskManager::Wait(TaskId id)
	{
		while (TaskPending(id))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	void TaskManager::CreateAndStartWorkerThreads()
	{
		for (int32_t threadIdx = 0; threadIdx < 5; threadIdx++)
		{
			_workerThreads[threadIdx] = std::thread(&TaskManager::WorkerThreadFunc, this);
		}
	}

	void TaskManager::WorkerThreadFunc()
	{
		for (;;)
		{
			std::unique_lock<std::mutex> lock(_taskQueueMutex);
			_workAddedToTaskQueue.wait(lock);
			auto taskIter = GetHighestPriorityTask();			
			if (taskIter == _taskQueue.end())
			{
				// If we get here, we either had a spurious wake-up (https://en.wikipedia.org/wiki/Spurious_wakeup)
				// or all the queued tasks still have a dependency, in which case we'll hang out until FinishAdd is called
				// either way, we just unlock and loop back to waiting on the condition_variable
				lock.unlock();
				continue;
			}
			auto task = *taskIter;
			_taskQueue.erase(taskIter);
			lock.unlock();
			task._workItem();
			DecrementOpenWorkItems(task._id);
		}
	}

	bool TaskManager::TaskQueueHasItems()
	{
		return _taskQueue.size() > 0;
	}

	std::vector<Task>::iterator TaskManager::GetHighestPriorityTask()
	{
		auto highestPriorityTask = _taskQueue.end();
		auto highestPriority = INT32_MIN;
		for (auto taskQueueIter = _taskQueue.begin(); taskQueueIter != _taskQueue.end(); ++taskQueueIter)
		{
			if (taskQueueIter->_dependency != 0)
			{
				continue; // This task has a non-complete dependency, we can't start it
			}

			if (taskQueueIter->_priority > highestPriority)
			{
				highestPriority = taskQueueIter->_priority;
				highestPriorityTask = taskQueueIter;
			}
		}
		return highestPriorityTask;
	}

	void TaskManager::DecrementOpenWorkItems(TaskId id)
	{
		std::lock_guard<std::mutex>lock(_openTaskQueueMutex);
		NonThreadSafeDecrementOpenWorkItemsRecursive(id);
	}

	void TaskManager::NonThreadSafeDecrementOpenWorkItemsRecursive(TaskId id)
	{
		for (auto openTaskIter = _openTaskQueue.begin(); openTaskIter != _openTaskQueue.end(); ++openTaskIter)
		{
			if (openTaskIter->_id == id)
			{
				openTaskIter->_openWorkItems--;
				if (openTaskIter->_openWorkItems <= 0)
				{
					if (openTaskIter->_parentId != 0)
					{
						NonThreadSafeDecrementOpenWorkItemsRecursive(openTaskIter->_parentId);
					}
					for (auto dependantTaskIter = _openTaskQueue.begin(); dependantTaskIter != _openTaskQueue.end(); ++dependantTaskIter)
					{
						if (dependantTaskIter->_dependency == id)
						{
							dependantTaskIter->_dependency = 0;
						}
					}
					_openTaskQueue.erase(openTaskIter);
					return;
				}
			}
		}
	}

	bool TaskManager::TaskPending(TaskId id)
	{
		std::unique_lock<std::mutex> lock(_openTaskQueueMutex);
		for (auto& task : _openTaskQueue)
		{
			if (task._id == id)
			{
				return true;
			}
		}

		return false;
	}
}
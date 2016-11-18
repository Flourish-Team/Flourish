#include "Task/TaskManager.h"

#include <cassert>

#include "Task/TaskQueue.h"
#include "Platform/Os.h"

namespace Flourish
{
    struct RootAndDependancies
    {
        RootAndDependancies(TaskManager* taskManager, TaskId root, std::vector<TaskId> dependanices)
        {
            _taskManager = taskManager;
            _root = root;
            _dependancies = dependanices;
        }
        
        TaskManager* _taskManager;
        TaskId _root;
        std::vector<TaskId> _dependancies;
    };
    
    void WaitForRootThenAddDependancies(void* data)
    {
        auto rootAndDependancies = (RootAndDependancies*)data;
        rootAndDependancies->_taskManager->Wait(rootAndDependancies->_root);
        for(auto& dependancy : rootAndDependancies->_dependancies)
        {
            rootAndDependancies->_taskManager->FinishAdd(dependancy);
        }
        delete rootAndDependancies;
    }
    
	TaskManager::TaskManager(int32_t numThreads)
		: _nextTaskId(1)
        , _allTasks(nullptr)
        , _numThreads(numThreads)
        , _numQueuedTasks(0)
		, _workerThreads(nullptr)
        , _taskQueues(nullptr)
        , _waitForWorkMutex()
		, _workMaybeAvailable()
        , _exiting(false)
	{
        CreateTasks();
		CreateAndStartWorkerThreads();
	}

	TaskManager::~TaskManager()
	{
        _exiting = true;
        _workMaybeAvailable.notify_all();
        for (uint32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
        {
            _workerThreads[threadIdx].join();
        }
		delete[] _workerThreads;
	}

	TaskId TaskManager::BeginAdd(WorkItem workItem)
	{
        TaskId taskId = _nextTaskId++;
        auto task = GetTaskFromId(taskId);
		task->_id = taskId;
		task->_workItem = workItem;
        task->_added = false;
        task->_complete = false;
		return task->_id;
	}
    
    TaskId TaskManager::AddDependantTasks(TaskId root, std::vector<TaskId> dependancies)
    {
        WorkItem waitForRootThenAddDependancies;
        waitForRootThenAddDependancies._data = new RootAndDependancies(this, root, dependancies);
        waitForRootThenAddDependancies._function = &WaitForRootThenAddDependancies;
        auto wrapperTaskId = BeginAdd(waitForRootThenAddDependancies);
        FinishAdd(wrapperTaskId);
        return wrapperTaskId;
    }
    
    void TaskManager::AddChild(TaskId parent, TaskId child)
    {
        auto childTask = GetTaskFromId(child);
        if(childTask == nullptr)
        {
            return;
        }
        childTask->_parentId = parent;
    }
    
    void TaskManager::FinishAdd(TaskId id)
    {
        auto task = GetTaskFromId(id);
        if(task != nullptr)
        {
            // The first task queue is the one for this non-worker thread
            assert(!task->_added);
            task->_added = true;
            _taskQueues[0]->Push(task);
            _numQueuedTasks++;
            _workMaybeAvailable.notify_one();
        }
    }

	void TaskManager::Wait(TaskId id)
	{
        auto task = GetTaskFromId(id);
        if(task == nullptr)
        {
            return;
        }
		while (!task->_complete)
		{
            WaitForTaskAndExecute(0);
		}
	}
    
    void TaskManager::CreateTasks()
    {
        _allTasks = new Task[MaxConcurrentTasks];
    }

	void TaskManager::CreateAndStartWorkerThreads()
	{
        if((int32_t)_numThreads == TaskManager::AutomaticallyDetectNumThreads)
        {
            _numThreads = GetIdealNumThreads();
        }
        _workerThreads = new std::thread[_numThreads];
        _taskQueues = new TaskQueue*[_numThreads + 1]; // The current, non-worker thread also gets a queue
        for (uint32_t queueIdx = 0; queueIdx < _numThreads + 1; queueIdx++)
        {
            _taskQueues[queueIdx] = new TaskQueue();
        }
		for (uint32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
		{
			_workerThreads[threadIdx] = std::thread(&TaskManager::WorkerThreadFunc, this, threadIdx + 1);
        }
	}

	void TaskManager::WorkerThreadFunc(int32_t threadIdx)
	{
		while(!_exiting)
		{
            WaitForTaskAndExecute(threadIdx);
		}
	}
    
    void TaskManager::WaitForTaskAndExecute(int32_t threadIdx)
    {
        auto task = GetTaskToExecute(threadIdx);
        if(task == nullptr)
        {
            if(_numQueuedTasks > 0)
            {
                // Task was added whilst we were looking for one, back to the top!
                return;
            }
            // Wait for a new task to be added
            std::unique_lock<std::mutex> lock(_waitForWorkMutex);
            _workMaybeAvailable.wait(lock);
            lock.unlock();
            return;
        }
        _numQueuedTasks--;
        task->_workItem();
        FinishTask(task);
    }
    
    int32_t TaskManager::GetIdealNumThreads()
    {
        // TODO: Actually work this out from something sensible
        return 5;
    }
    
    Task* TaskManager::GetTaskToExecute(uint32_t currentThreadQueueIdx)
    {
        auto ourQueue = _taskQueues[currentThreadQueueIdx];
        auto task = ourQueue->Pop();
        if(task != nullptr)
        {
            return task;
        }
        // Try stealing from one of the other queues
        for (uint32_t queueIdx = 0; queueIdx < _numThreads + 1; queueIdx++)
        {
            if(queueIdx == currentThreadQueueIdx)
            {
                continue;
            }
            auto stolenTask = _taskQueues[queueIdx]->Steal();
            if(stolenTask != nullptr)
            {
                return stolenTask;
            }
        }
        return nullptr;
    }
    
    Task* TaskManager::GetTaskFromId(TaskId id)
    {
        if(id == 0)
        {
            return nullptr;
        }
        // Id's start at 1, so ID 1 is at index 0
        auto index = (id - 1) % MaxConcurrentTasks;
        return &_allTasks[index];
    }
    
    void TaskManager::FinishTask(Task* task)
    {
        task->_complete = true;
        auto parentTask = GetTaskFromId(task->_parentId);
        if(parentTask != nullptr)
        {
            FinishTask(parentTask);
        }
        // We might have just finished the task
        // another one was waiting on
        _workMaybeAvailable.notify_one();
    }
}

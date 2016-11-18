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
        for (int32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
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
		task->_openWorkItems = 1;
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
            _taskQueues[0]->Push(task);
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
		while (task->_openWorkItems > 0)
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
        if(_numThreads == TaskManager::AutomaticallyDetectNumThreads)
        {
            _numThreads = GetIdealNumThreads();
        }
        _workerThreads = new std::thread[_numThreads];
        _taskQueues = new TaskQueue*[_numThreads + 1]; // The current, non-worker thread also gets a queue
        for (int32_t queueIdx = 0; queueIdx < _numThreads + 1; queueIdx++)
        {
            _taskQueues[queueIdx] = new TaskQueue();
        }
		for (int32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
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
            // Wait for a new task to be added
            std::unique_lock<std::mutex> lock(_waitForWorkMutex);
            _workMaybeAvailable.wait_for(lock, std::chrono::milliseconds(10));
            lock.unlock();
            return;
        }
        
        task->_workItem();
        FinishTask(task);
    }
    
    int32_t TaskManager::GetIdealNumThreads()
    {
        // TODO: Actually work this out from something sensible
        return 5;
    }
    
    // TODO: This method of picking a queue could do with improving
    // the random approach could result in no tasks being done if
    // there aren't many
    Task* TaskManager::GetTaskToExecute(uint32_t currentThreadQueueIdx)
    {
        auto ourQueue = _taskQueues[currentThreadQueueIdx];
        auto task = ourQueue->Pop();
        if(task == nullptr)
        {
            // If we're a worker thread, try stealing from
            // the task queue from the main thread first
            if(currentThreadQueueIdx != 0)
            {
                task = _taskQueues[0]->Steal();
            }
            if(task == nullptr)
            {
                uint32_t randomIdx = rand() % (_numThreads + 1);
                if(randomIdx != currentThreadQueueIdx)
                {
                    task = _taskQueues[randomIdx]->Steal();
                }
                if(task == nullptr)
                {
                    return nullptr;
                }
            }
        }
        auto dependancy = GetTaskFromId(task->_dependency);
        if(dependancy != nullptr &&
           dependancy->_openWorkItems > 0)
        {
            // The task depends on another that isn't done yet
            // so stick it at the back of our queue
            _taskQueues[currentThreadQueueIdx]->Push(task);
            return nullptr;
        }
        return task;
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
        task->_openWorkItems--;
        if(task->_openWorkItems == 0)
        {
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
}

#include "Task/TaskManager.h"

#include <cassert>

#include "Task/TaskQueue.h"

namespace Flourish
{
    struct RootAndDependencies
    {
        RootAndDependencies(TaskManager* taskManager, TaskId root, std::vector<TaskId> dependencies)
        {
            _taskManager = taskManager;
            _root = root;
            _dependencies = std::move(dependencies);
        }
        
        TaskManager* _taskManager;
        TaskId _root;
        std::vector<TaskId> _dependencies;
    };
    
    void WaitForRootThenAddDependencies(void* data)
    {
        auto rootAndDependencies = static_cast<RootAndDependencies*>(data);
        rootAndDependencies->_taskManager->Wait(rootAndDependencies->_root);
        for(auto& dependancy : rootAndDependencies->_dependencies)
        {
            rootAndDependencies->_taskManager->FinishAdd(dependancy);
        }
        delete rootAndDependencies;
    }
    
    thread_local TaskQueue* TaskManager::_currentThreadTaskQueue = nullptr;
    
	TaskManager::TaskManager(int32_t numThreads)
		: _nextTaskId(1)
        , _allTasks(nullptr)
        , _numThreads(numThreads)
		, _workerThreads(nullptr)
        , _taskQueues(nullptr)
        , _exiting(false)
	{
        CreateTasks();
		CreateAndStartWorkerThreads();
	}

	TaskManager::~TaskManager()
	{
        _exiting = true;
        _taskThreadGate.OpenPermenentlyAndNotifyAll();
        for (uint32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
        {
            _workerThreads[threadIdx].join();
        }
		delete[] _workerThreads;
        delete _currentThreadTaskQueue;
        delete[] _taskQueues;
	}

	TaskId TaskManager::BeginAdd(WorkItem workItem)
	{
        TaskId taskId = _nextTaskId++;
        auto task = GetTaskFromId(taskId);
		task->_id = taskId;
		task->_workItem = std::move(workItem);
        task->_added = false;
        task->_openWorkItems = 1;
		return task->_id;
	}
    
    TaskId TaskManager::AddDependentTasks(TaskId root, std::vector<TaskId> dependencies)
    {
        WorkItem waitForRootThenAddDependencies(&WaitForRootThenAddDependencies, new RootAndDependencies(this, root, std::move(dependencies)));
        auto wrapperTaskId = BeginAdd(waitForRootThenAddDependencies);
        FinishAdd(wrapperTaskId);
        return wrapperTaskId;
    }
    
    void TaskManager::AddChild(TaskId parent, TaskId child)
    {
        auto parentTask = GetTaskFromId(parent);
        if(parentTask == nullptr)
        {
            return;
        }
        auto childTask = GetTaskFromId(child);
        if(childTask == nullptr)
        {
            return;
        }
        childTask->_parentId = parent;
        parentTask->_openWorkItems++;
    }
    
    void TaskManager::FinishAdd(TaskId id)
    {
        auto task = GetTaskFromId(id);
        if(task != nullptr)
        {
            assert(!task->_added); // If this is hit a task was added twice
            task->_added = true;
            _currentThreadTaskQueue->Push(task);
            _taskThreadGate.OpenAndNotifyOne();
        }
    }
    
    TaskId TaskManager::AddTaskWithNoChildrenOrDependencies(Flourish::WorkItem workItem)
    {
        auto taskId = BeginAdd(std::move(workItem));
        FinishAdd(taskId);
        return taskId;
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
            WaitForTaskAndExecute(std::chrono::milliseconds(100));
		}
	}
    
    void TaskManager::CreateTasks()
    {
        _allTasks = new Task[MaxConcurrentTasks];
    }

	void TaskManager::CreateAndStartWorkerThreads()
	{
        if(static_cast<int32_t>(_numThreads) == TaskManager::AutomaticallyDetectNumThreads)
        {
            _numThreads = GetIdealNumThreads();
        }
        _workerThreads = new std::thread[_numThreads];
        _taskQueues = new TaskQueue*[_numThreads + 1]; // The current, non-worker thread also gets a queue
        for(uint32_t queueIdx = 0; queueIdx < _numThreads + 1; queueIdx++)
        {
            _taskQueues[queueIdx] = nullptr;
        }
        
        CreateTaskQueueForCurrentThread(0);
        
		for (uint32_t threadIdx = 0; threadIdx < _numThreads; threadIdx++)
		{
			_workerThreads[threadIdx] = std::thread(&TaskManager::WorkerThreadFunc, this, threadIdx + 1);
        }
	}

	void TaskManager::WorkerThreadFunc(int32_t threadIdx)
	{
        CreateTaskQueueForCurrentThread(threadIdx);
		while(!_exiting)
		{
            WaitForTaskAndExecute();
		}
        delete _currentThreadTaskQueue;
	}
    
	void TaskManager::WaitForTaskAndExecute(std::chrono::milliseconds waitDuration)
    {
        auto task = GetTaskToExecute();
        if(task == nullptr)
        {
            // Wait for a more work
            _taskThreadGate.Wait(waitDuration);
            return;
        }
        task->_workItem();
        FinishTask(task);
    }
    
    int32_t TaskManager::GetIdealNumThreads()
    {
        auto numConcurrentThreads = std::thread::hardware_concurrency();
        assert(numConcurrentThreads != 0); // If you hit this, it can't be detected
        return numConcurrentThreads - 1;
    }
    
    Task* TaskManager::GetTaskToExecute()
    {
        auto task = _currentThreadTaskQueue->Pop();
        if(task != nullptr)
        {
            return task;
        }
        // Try stealing from one of the other queues
        // This will actually check the current thread queue again, but that's okay
        for (uint32_t queueIdx = 0; queueIdx < _numThreads + 1; queueIdx++)
        {
            auto queueToStealFrom = _taskQueues[queueIdx];
            if(queueToStealFrom == nullptr)
            {
                continue;
            }
            auto stolenTask = queueToStealFrom->Steal();
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
        task->_openWorkItems--;
        if(task->_openWorkItems <= 0)
        {
        auto parentTask = GetTaskFromId(task->_parentId);
            if(parentTask != nullptr)
            {
                FinishTask(parentTask);
            }
            // We might have just finished the task
            // another one was waiting on
            // We notify all here in-case notify_one
            // picks a thread that isn't the one that's waiting
            _taskThreadGate.OpenAndNotifyAll();
        }
    }
    
    void TaskManager::CreateTaskQueueForCurrentThread(uint32_t threadIdx)
    {
        _currentThreadTaskQueue = new TaskQueue();
        _taskQueues[threadIdx] = _currentThreadTaskQueue;
    }
}

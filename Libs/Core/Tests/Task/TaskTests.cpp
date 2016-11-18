#include "Test.h"
#include "Task/TaskManager.h"

using namespace Flourish;


TEST(TaskManagerTests, TaskRuns)
{
	bool taskHasRun = false;

	TaskManager _taskManager;
	auto waitWorkItem = WorkItem();
	waitWorkItem._data = &taskHasRun;
	waitWorkItem._function = [](void* data) {
		*((bool*)data) = true;
	};

	auto taskId = _taskManager.BeginAdd(waitWorkItem);
	_taskManager.FinishAdd(taskId);

	_taskManager.Wait(taskId);

	ASSERT_TRUE(taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, WaitingWillRunTask)
{
    bool taskHasRun = false;
    
    TaskManager _taskManager(0);
    auto waitWorkItem = WorkItem();
    waitWorkItem._data = &taskHasRun;
    waitWorkItem._function = [](void* data) {
        *((bool*)data) = true;
    };
    
    auto taskId = _taskManager.BeginAdd(waitWorkItem);
    _taskManager.FinishAdd(taskId);
    
    _taskManager.Wait(taskId);
    
    ASSERT_TRUE(taskHasRun) << "Task did not run";
}

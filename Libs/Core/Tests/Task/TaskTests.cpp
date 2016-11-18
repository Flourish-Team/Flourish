#include "Test.h"
#include "Task/TaskManager.h"

using namespace Flourish;


TEST(TaskManagerTests, TaskRuns)
{
	bool taskHasRun = false;

	TaskManager taskManager;
	auto waitWorkItem = WorkItem();
	waitWorkItem._data = &taskHasRun;
	waitWorkItem._function = [](void* data) {
		*((bool*)data) = true;
	};

	auto taskId = taskManager.BeginAdd(waitWorkItem);
	taskManager.FinishAdd(taskId);

	taskManager.Wait(taskId);

	ASSERT_TRUE(taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, WaitingWillRunTask)
{
    bool taskHasRun = false;
    
    TaskManager taskManager(0);
    auto waitWorkItem = WorkItem();
    waitWorkItem._data = &taskHasRun;
    waitWorkItem._function = [](void* data) {
        *((bool*)data) = true;
    };
    
    auto taskId = taskManager.BeginAdd(waitWorkItem);
    taskManager.FinishAdd(taskId);
    
    taskManager.Wait(taskId);
    
    ASSERT_TRUE(taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, DependancyMustFinishFirst)
{
    TaskManager taskManager(0);
    int32_t taskRunSatus = -1;
    
    auto workItemA = WorkItem();
    workItemA._data = &taskRunSatus;
    workItemA._function = [](void* data) {
        auto& status = *((int32_t*)data);
        ASSERT_EQUAL(status, -1) << "Status was not expected value during task A";
        status = 1;
    };
    
    auto workItemB = WorkItem();
    workItemB._data = &taskRunSatus;
    workItemB._function = [](void* data) {
        auto& status = *((int32_t*)data);
        ASSERT_EQUAL(status, 1) << "Status was not expected value during task B";
        status = 2;
    };
    
    auto taskA = taskManager.BeginAdd(workItemA);
    auto taskB = taskManager.BeginAdd(workItemB);
    auto dependancies = std::vector<TaskId>();
    dependancies.push_back(taskB);
    taskManager.AddDependantTasks(taskA, dependancies);
    taskManager.FinishAdd(taskA);
    
    taskManager.Wait(taskB);
    
    ASSERT_EQUAL(taskRunSatus, 2);
}

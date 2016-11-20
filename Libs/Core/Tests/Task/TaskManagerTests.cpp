#include "Test.h"
#include "Task/TaskManager.h"

using namespace Flourish;

TEST(TaskManagerTests, TaskRuns)
{
	TaskManager taskManager;
    bool taskHasRun;
    auto workItem = taskManager.WorkItemWithTaskAllocator([&](void*){
        taskHasRun = true;
    });
    
	auto taskId = taskManager.AddTaskWithNoChildrenOrDependencies(workItem);

	taskManager.Wait(taskId);

	ASSERT_TRUE(taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, WaitingWillRunTask)
{
    TaskManager taskManager(0);
    bool taskHasRun;
    auto workItem = taskManager.WorkItemWithTaskAllocator([&](void*){
        taskHasRun = true;
    });
    
    auto taskId = taskManager.AddTaskWithNoChildrenOrDependencies(workItem);
    
    taskManager.Wait(taskId);
    
    ASSERT_TRUE(taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, DependancyMustFinishFirst)
{
    TaskManager taskManager(0);
    int32_t taskRunSatus = -1;
    
    auto workItemA = taskManager.WorkItemWithTaskAllocator([&](void*) {
        ASSERT_EQUAL(taskRunSatus, -1) << "Status was not expected value during task A";
        taskRunSatus = 1;
    });
    
    auto workItemB = taskManager.WorkItemWithTaskAllocator([&](void*) {
        ASSERT_EQUAL(taskRunSatus, 1) << "Status was not expected value during task B";
        taskRunSatus = 2;
    });
    
    auto taskA = taskManager.BeginAdd(workItemA);
    auto taskB = taskManager.BeginAdd(workItemB);
    auto dependencies = std::vector<TaskId>();
    dependencies.push_back(taskB);
    taskManager.AddDependentTasks(taskA, dependencies);
    taskManager.FinishAdd(taskA);
    
    taskManager.Wait(taskB);
    
    ASSERT_EQUAL(taskRunSatus, 2);
}

TEST(TaskManagerTests, ParentDoesNotFinishUntilChildFinishes)
{
    TaskManager taskManager(0);
    std::atomic_uint numChildrenFinished(0);
    
    auto child1 = taskManager.WorkItemWithTaskAllocator([&](void*){
        numChildrenFinished++;
    });
    auto child2 = taskManager.WorkItemWithTaskAllocator([&](void*){
        numChildrenFinished++;
    });
    auto child3 = taskManager.WorkItemWithTaskAllocator([&](void*){
        numChildrenFinished++;
    });
    
    auto parent = taskManager.WorkItemWithTaskAllocator([](void*) {});
    
    auto parentId = taskManager.BeginAdd(parent);
    auto child1Id = taskManager.BeginAdd(child1);
    auto child2Id = taskManager.BeginAdd(child2);
    auto child3Id = taskManager.BeginAdd(child3);
    taskManager.AddChild(parentId, child1Id);
    taskManager.AddChild(parentId, child2Id);
    taskManager.AddChild(parentId, child3Id);
    
    taskManager.FinishAdd(child3Id);
    taskManager.FinishAdd(child2Id);
    taskManager.FinishAdd(child1Id);
    taskManager.FinishAdd(parentId);
    
    taskManager.Wait(parentId);
    
    ASSERT_EQUAL(numChildrenFinished, 3u) << "Not all children finished before parent";
}

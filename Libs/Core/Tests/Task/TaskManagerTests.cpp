#include "Test.h"
#include "Task/TaskManager.h"

using namespace Flourish;

struct SetBoolWhenRunWorkItem : WorkItem
{
    std::atomic_bool _taskHasRun;
    
    SetBoolWhenRunWorkItem()
        : WorkItem([](void* data){ *((std::atomic_bool*)data) = true; }, &_taskHasRun)
    {
        
    }
    
};


TEST(TaskManagerTests, TaskRuns)
{
	TaskManager taskManager;
    SetBoolWhenRunWorkItem workItem;
    
	auto taskId = taskManager.AddTaskWithNoChildrenOrDependencies(workItem);

	taskManager.Wait(taskId);

	ASSERT_TRUE(workItem._taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, WaitingWillRunTask)
{
    TaskManager taskManager(0);
    SetBoolWhenRunWorkItem workItem;
    
    auto taskId = taskManager.AddTaskWithNoChildrenOrDependencies(workItem);
    
    taskManager.Wait(taskId);
    
    ASSERT_TRUE(workItem._taskHasRun) << "Task did not run";
}

TEST(TaskManagerTests, DependancyMustFinishFirst)
{
    TaskManager taskManager(0);
    int32_t taskRunSatus = -1;
    
    WorkItem workItemA([](void* data) {
        auto& status = *((int32_t*)data);
        ASSERT_EQUAL(status, -1) << "Status was not expected value during task A";
        status = 1;
    }, &taskRunSatus);
    
    WorkItem workItemB([](void* data) {
        auto& status = *((int32_t*)data);
        ASSERT_EQUAL(status, 1) << "Status was not expected value during task B";
        status = 2;
    }, &taskRunSatus);
    
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
    
    WorkItem child1([](void* data){
        (*(std::atomic_uint*)data)++;
    }, &numChildrenFinished);
    WorkItem child2([](void* data){
        (*(std::atomic_uint*)data)++;
    }, &numChildrenFinished);
    WorkItem child3([](void* data){
        (*(std::atomic_uint*)data)++;
    }, &numChildrenFinished);
    
    WorkItem parent([](void*) {});
    
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

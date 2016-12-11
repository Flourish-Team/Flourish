#include "Test.h"
#include "Task/TaskQueue.h"
#include "Task/Task.h"

using namespace Flourish;

TEST(TaskQueueTests, PopReturnsNullIfEmpty)
{
    TaskQueue queue;
    auto result = queue.Pop();
    EXPECT_EQUAL(result, nullptr);
}

TEST(TaskQueueTests, StealReturnsNullIfEmpty)
{
    TaskQueue queue;
    auto result = queue.Steal();
    EXPECT_EQUAL(result, nullptr);
}

TEST(TaskQueueTests, PopReturnsLastTaskAdded)
{
    TaskQueue queue;
    Task taskA;
    Task taskB;
    
    queue.Push(&taskA);
    queue.Push(&taskB);
    
    auto result = queue.Pop();
    EXPECT_EQUAL(result, &taskB);
}

TEST(TaskQueueTests, StealReturnsFirstTaskAdded)
{
    TaskQueue queue;
    Task taskA;
    Task taskB;
    
    queue.Push(&taskA);
    queue.Push(&taskB);
    
    auto result = queue.Steal();
    EXPECT_EQUAL(result, &taskA);
}

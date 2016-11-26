#include "Test.h"

#include "Task/CountSplitter.h"
#include "Task/ParallelFor.h"
#include "Task/TaskManager.h"

#include "Task/TaskTestHelpers/MockSplitter.h"
#include "Task/TaskTestHelpers/MockTaskSystem.h"
#include "Task/TaskTestHelpers/RecordReplaySplitter.h"
#include "Task/TaskTestHelpers/RecordTaskSystem.h"
#include "Task/TaskTestHelpers/ReplaySplitter.h"
#include "Task/TaskTestHelpers/StubSplitter.h"
#include "Task/TaskTestHelpers/StubTaskSystem.h"

using namespace Flourish;
using namespace Flourish::TaskTestHelpers;

template<typename DataType, typename Splitter, typename TaskSystem>
ParallelFor<DataType, Splitter, TaskSystem> MakeParallelFor(DataType* data, uint32_t dataCount, Splitter* splitter, TaskSystem* taskSystem)
{
    return ParallelFor<DataType, Splitter, TaskSystem>(data, dataCount, splitter, [](DataType*,uint32_t){}, taskSystem);
}

TEST(ParallelForTests, ShouldAskSplitterIfDataShouldBeSplit)
{
    const uint32_t dummyDataSize = 20;
    int dummyData[dummyDataSize];
    MockSplitter<int> mockSplitter;
    StubTaskSystem stubTaskSystem;
    
    auto parallelFor = MakeParallelFor(dummyData, dummyDataSize, &mockSplitter, &stubTaskSystem);
    
    parallelFor.Run();
    
    ASSERT_EQUAL(dummyData, mockSplitter._data) << "Data passed to splitter was incorrect";
    ASSERT_EQUAL(dummyDataSize, mockSplitter._count) << "Count passed to splitter was incorrect";
}

TEST(ParallelForTests, ShouldAddTaskIfSplitReturnsFalse)
{
    const uint32_t dummyDataSize = 20;
    int dummyData[dummyDataSize];
    StubSplitter<int> stubSplitter(false);
    MockTaskSystem mockTaskSystem;
    
    auto parallelFor = MakeParallelFor(dummyData, dummyDataSize, &stubSplitter, &mockTaskSystem);
    
    parallelFor.Run();
    
    ASSERT_TRUE(mockTaskSystem._taskAdded) << "Task was not added";
}

TEST(ParallelForTests, ShouldSplitIntoSubtasksIfSplitReturnsTrue)
{
    const uint32_t dummyDataSize = 20;
    int dummyData[dummyDataSize];
    bool shouldSplit[] = {
        true,
        false,
        false
    };
    ReplaySplitter<int, 3> replaySplitter(shouldSplit);
    RecordTaskSystem<3> recordTaskSystem;
    
    auto parallelFor = MakeParallelFor(dummyData, dummyDataSize, &replaySplitter, &recordTaskSystem);
    
    parallelFor.Run();
    
    replaySplitter.AssertUsedAllExpectedResponses();
    recordTaskSystem.AssertExpectedNumberOfTasksAdded();
    ASSERT_EQUAL(recordTaskSystem._parentTasks[1], TaskId(0)) << "First subtask was not parented correclty";
    ASSERT_EQUAL(recordTaskSystem._parentTasks[2], TaskId(0)) << "Second subtask was not parented correclty";
}

TEST(ParallelForTests, ShouldSplitDataAndCount)
{
    const uint32_t dummyDataSize = 19;
    int dummyData[dummyDataSize];
    bool shouldSplit[] = {
        true,
        false,
        false
    };
    RecordReplaySplitter<int, 3> recordReplaySplitter(shouldSplit);
    StubTaskSystem stubTaskSystem;
    
    auto parallelFor = MakeParallelFor(dummyData, dummyDataSize, &recordReplaySplitter, &stubTaskSystem);
    
    parallelFor.Run();
    
    ASSERT_EQUAL(recordReplaySplitter._dataList[0], dummyData) << "First split did not have correct data";
    ASSERT_EQUAL(recordReplaySplitter._dataCountList[0], dummyDataSize) << "First split did not have correct data count";
    
    ASSERT_EQUAL(recordReplaySplitter._dataList[1], dummyData) << "Second split did not have correct data";
    ASSERT_EQUAL(recordReplaySplitter._dataCountList[1], dummyDataSize / 2u) << "Second split did not have correct data count";
    
    auto expectedOffset = dummyDataSize / 2u;
    auto expectedSize = dummyDataSize - expectedOffset;
    ASSERT_EQUAL(recordReplaySplitter._dataList[2], &dummyData[expectedOffset]) << "Third split did not have correct data";
    ASSERT_EQUAL(recordReplaySplitter._dataCountList[2], expectedSize) << "Third split did not have correct data count";
}

TEST(ParallelForTests, RunsAllTasks)
{
    const uint32_t dummyDataSize = 100;
    int dummyData[dummyDataSize];
    CountSplitter<int> countSplitter(10);
    TaskManager taskManager;
    for(uint32_t index = 0; index < dummyDataSize; index++)
    {
        dummyData[index] = index;
    }
    
    auto parallelFor = ParallelFor<int, CountSplitter<int>>(dummyData, dummyDataSize, &countSplitter, [&](int* data, uint32_t dataCount){
        for(uint32_t index = 0; index < dataCount; index++)
        {
            data[index] *= 2;
        }
    }, &taskManager);
    
    auto taskId = parallelFor.Run();
    taskManager.Wait(taskId);
    
    for(uint32_t index = 0; index < dummyDataSize; index++)
    {
        ASSERT_EQUAL(dummyData[index], ((int)index * 2)) << "Data for index " << index << " was incorrect";
    }
}

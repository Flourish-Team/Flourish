#include "Test.h"

#include "Task/ParallelFor.h"

using namespace Flourish;

template<typename DataType>
class MockSplitter
{
public:
    MockSplitter()
        : _data(nullptr)
        , _count(0)
    {
    }
    
    bool ShouldSplit(DataType* data, uint32_t count)
    {
        _data = data;
        _count = count;
        return false;
    }
    
    DataType* _data;
    uint32_t _count;
};

template<typename DataType>
class StubSplitter
{
public:
    StubSplitter(bool shouldSplit)
        : _shouldSplit(shouldSplit)
    {
        
    }
    
    bool ShouldSplit(DataType*, uint32_t)
    {
        return _shouldSplit;
    }
    
    bool _shouldSplit;
};

template<typename DataType, uint32_t count>
class ReplaySplitter
{
public:
    ReplaySplitter(bool shouldSplit[count])
        : _shouldSplit()
        , _currentResonse(0)
    {
        for(uint32_t index = 0; index < count; index++)
        {
            _shouldSplit[index] = shouldSplit[index];
        }
    }
    
    virtual bool ShouldSplit(DataType*, uint32_t)
    {
        if(_currentResonse >= count)
        {
            return false;
        }
        auto response = _shouldSplit[_currentResonse];
        _currentResonse++;
        return response;
    }
    
    void AssertUsedAllExpectedResponses()
    {
        ASSERT_EQUAL(_currentResonse, count) << "Did not use all the expected responses";
    }
    
    bool _shouldSplit[count];
    uint32_t _currentResonse;
};

template<typename DataType, uint32_t count>
class RecordReplaySplitter : public ReplaySplitter<DataType, count>
{
public:
    RecordReplaySplitter(bool shouldSplit[count])
        : ReplaySplitter<DataType, count>(shouldSplit)
        , _dataList()
        , _dataCountList()
    {
        for(uint32_t index = 0; index < count; index++)
        {
            _dataList[index] = nullptr;
            _dataCountList[index] = 0;
        }
    }
    
    bool ShouldSplit(DataType* data, uint32_t dataCount) override
    {
        _dataList[ReplaySplitter<DataType, count>::_currentResonse] = data;
        _dataCountList[ReplaySplitter<DataType, count>::_currentResonse] = dataCount;
        return ReplaySplitter<DataType, count>::ShouldSplit(data, dataCount);
    }
    
    DataType* _dataList[count];
    uint32_t _dataCountList[count];
};

class StubTaskSystem
{
public:
    template<typename Callable>
    WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
    {
        return WorkItem(WorkItemFunction(callable), data);
    }
    
    TaskId BeginAdd(WorkItem)
    {
        return TaskId(0);
    }
    
    void AddChild(TaskId, TaskId) {}
    
    void FinishAdd(TaskId){}
};

class MockTaskSystem
{
public:
    MockTaskSystem()
        : _taskAdded(false)
    {
    }
    
    template<typename Callable>
    WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
    {
        return WorkItem(WorkItemFunction(callable), data);
    }
    
    TaskId BeginAdd(WorkItem)
    {
        return TaskId(4);
    }
    
    void FinishAdd(TaskId id)
    {
        ASSERT_EQUAL(id, 4u) << "Incorrect TaskId was passed to FinishAdd";
        _taskAdded = true;
    }
    
    void AddChild(TaskId, TaskId) {}
    
    bool _taskAdded;
};

template<uint32_t expectedNumTasks>
class RecordTaskSystem
{
public:
    RecordTaskSystem()
        : _numTasksAdded(0)
        , _parentTasks()
        , _nextTaskId(0)
    {
        for(uint32_t index = 0; index < expectedNumTasks; index++)
        {
            _parentTasks[index] = -1;
        }
    }
    
    template<typename Callable>
    WorkItem WorkItemWithTaskAllocator(Callable callable, void* data = nullptr)
    {
        return WorkItem(WorkItemFunction(callable), data);
    }
    
    TaskId BeginAdd(WorkItem)
    {
        return TaskId(_nextTaskId++);
    }
    
    void FinishAdd(TaskId)
    {
        _numTasksAdded++;
    }
    
    void AddChild(TaskId parent, TaskId child)
    {
        _parentTasks[child] = parent;
    }
    
    void AssertExpectedNumberOfTasksAdded()
    {
        ASSERT_EQUAL(_numTasksAdded, expectedNumTasks) << "Incorrect number of tasks added";
    }
    
    uint32_t _numTasksAdded;
    TaskId _parentTasks[expectedNumTasks];
    TaskId _nextTaskId;
};

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

TEST(ParallelForTests, ShouldSplitCorrectly)
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

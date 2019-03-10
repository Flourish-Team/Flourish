#include "Test.h"

#include "Task/TaskThreadGate.h"

using namespace Flourish;

class MockWaitMechanism
{
public:
    template<class Predicate>
    void wait(std::unique_lock<std::mutex>&, Predicate)
    {
        _hasWaited = true;
    }

	template<class Rep, class Period, class Predicate>
	void wait_for(std::unique_lock<std::mutex>&, const std::chrono::duration<Rep, Period>&, Predicate)
	{
		_hasWaited = true;
	}
    
    void notify_one()
    {
        _notifiedOne = true;
    }
    
    void notify_all()
    {
        _notifiedAll = true;
    }
    
    bool _hasWaited;
    bool _notifiedOne;
    bool _notifiedAll;
};

TEST(TaskThreadGate, WaitsWhenNotOpen)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.Wait();
    
    EXPECT_TRUE(gate.GetWaitMechanism()->_hasWaited) << "Gate should not be open, should have waited";
}

TEST(TaskThreadGate, OpenAndNotifyOneNotifiesOne)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenAndNotifyOne();
    
    EXPECT_TRUE(gate.GetWaitMechanism()->_notifiedOne) << "Gate should have notified one";
}

TEST(TaskThreadGate, OpenAndNotifyAllNotifiesAll)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenAndNotifyAll();
    
    EXPECT_TRUE(gate.GetWaitMechanism()->_notifiedAll) << "Gate should have notified all";
}

TEST(TaskThreadGate, OpenPermenentlyAndNotifyAllNotifiesAll)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenPermenentlyAndNotifyAll();
    
    EXPECT_TRUE(gate.GetWaitMechanism()->_notifiedAll) << "Gate should have notified all";
}

TEST(TaskThreadGate, WaitAfterOpenAndNotifyOneShouldNotWait)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenAndNotifyOne();
    
    gate.Wait();
    
    EXPECT_FALSE(gate.GetWaitMechanism()->_hasWaited) << "Gate should not have waited";
}

TEST(TaskThreadGate, WaitAfterOpenAndNotifyAllShouldNotWait)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenAndNotifyAll();
    
    gate.Wait();
    
    EXPECT_FALSE(gate.GetWaitMechanism()->_hasWaited) << "Gate should not have waited";
}

TEST(TaskThreadGate, WaitClosesGate)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenAndNotifyOne();
    
    gate.Wait();
    gate.GetWaitMechanism()->_hasWaited = false;
    
    gate.Wait();
    
    EXPECT_TRUE(gate.GetWaitMechanism()->_hasWaited) << "Gate should have closed after the first wait, and waited on the second";
}

TEST(TaskThreadGate, WaitShouldNotCloseGateIfOpenPermenently)
{
    TaskThreadGate<MockWaitMechanism> gate;
    
    gate.OpenPermenentlyAndNotifyAll();
    
    gate.Wait();
    gate.GetWaitMechanism()->_hasWaited = false;
    
    gate.Wait();
    
    EXPECT_FALSE(gate.GetWaitMechanism()->_hasWaited) << "Gate should not closed after the first wait because it should be open permenently";
}

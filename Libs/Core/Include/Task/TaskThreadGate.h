#pragma once

#include <mutex>
#include <condition_variable>

namespace Flourish
{
    template<typename WaitMechanism = std::condition_variable>
    class TaskThreadGate
    {
    public:
        TaskThreadGate()
        : _open(false)
        , _openPermenently(false)
        , _waitMechanism()
        {
            
        }
        
        void OpenAndNotifyOne()
        {
            OpenAndNotify(false);
        }
        
        void OpenAndNotifyAll()
        {
            OpenAndNotify(true);
        }
        
        void OpenPermenentlyAndNotifyAll()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _openPermenently = true;
            _open = true;
            _waitMechanism.notify_all();
        }
        
        void Wait(std::chrono::milliseconds waitDuration = std::chrono::milliseconds::zero())
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if(!_open)
            {
				if(waitDuration > std::chrono::milliseconds::zero())
				{
					_waitMechanism.wait_for(lock, waitDuration, [&] { return _open; });
				}
				else
				{
					_waitMechanism.wait(lock, [&] { return _open; });
				}
            }
            if(!_openPermenently)
            {
                _open = false;
            }
        }
        
#if defined(FL_RUNNING_TESTS)
        WaitMechanism* GetWaitMechanism()
        {
            return &_waitMechanism;
        }
#endif
        
    private:
        void OpenAndNotify(bool notifyAll)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _open = true;
            if(notifyAll)
            {
                _waitMechanism.notify_all();
            }
            else
            {
                _waitMechanism.notify_one();
            }
        }
        
        bool _open;
        bool _openPermenently;
        std::mutex _mutex;
        WaitMechanism _waitMechanism;
    };
}

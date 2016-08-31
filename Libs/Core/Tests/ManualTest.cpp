#include "Test.h"
#include "Task/TaskManager.h"

using namespace Flourish;

TEST(TaskManagerTests, ManualTest)
{
	// This is a bug
	// If a dependancy finishes before the other task is added
	// it will never be run
	// Also we really need a 'wait'
	TaskManager _taskManager;

	auto firstId = _taskManager.BeginAdd([](auto p) { std::cout << "First Task!"; });
	auto secondId = _taskManager.BeginAdd([](auto p) { std::cout << "second Task!"; }, firstId);
	_taskManager.FinishAdd(secondId);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	_taskManager.FinishAdd(firstId);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	int foo = 0;
}
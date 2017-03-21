#pragma once

#include "Platform/Platform.h"
#include "Macro/MacroUtils.h"
#include "Debug/SourceInfo.h"

//Simple runtime abd static asserts
//Asserts can be forced on or off using FL_FORCE_ASSERTS_ON or FL_FORCE_ASSERTS_OFF, otherwise they are active 
//on debug builds only

//Asserts that the condition is true
#define FL_ASSERT(condition) \
	_IMPL_FL_ASSERT(condition, "")

//Assers the condition is true, outputing the printf style message to the Assert handler
#define FL_ASSERT_MSG(condition, message, ...) \
	_IMPL_FL_ASSERT(condition, message, ##__VA_ARGS__)

//Assert that always fails if called
#define FL_ASSERT_ALWAYS() \
	_IMPL_FL_ASSERT(false, ("Always Fails"))

//Assert that always fails if called, outputting the printf style message to the Assert handler
#define FL_ASSERT_ALWAYS_MSG(message, ...) \
	_IMPL_FL_ASSERT(false, message,  ##__VA_ARGS__)

//Compile time assert
#define FL_STATIC_ASSERT(condition) \
	_IMPL_FL_STATIC_ASSERT(condition, "Static Assert Failed: '" FL_STRINGIFY(condition) "'")

//Compile time assert with message
#define FL_STATIC_ASSERT_MSG(condition, message) \
	_IMPL_FL_STATIC_ASSERT(condition, "Static Assert Failed: '" FL_STRINGIFY(condition) "' - '" message "'")


namespace Flourish { namespace Debug
{
	//Returned by the assert handler to specify how the assert should be handled
	enum class AssertResult
	{
		Ignore,			//Ignore this Assert once,
		IgnoreAlways,	//Ingore this assert for the rest of the run
		IgnoreAll,		//Ignore all asserts for the rest of the run
		Break,			//Trigger a breakpoint
		Abort,			//Abort the app
	};

	// typedef representing a funtion for handling asserts.
	typedef AssertResult (*AssertHandler)(
		SourceInfo sourceInfo,
		const char* condition,
		const char* message,
		const char* callstack);

	//Set the assert handler, set to null to use the default handler
	void SetAssertHandler(AssertHandler handler);

	//Returns the current assert handler, if null then we are using the default handler
	AssertHandler GetAssertHandler();

	//Set if all asserts should be ignored
	void SetIgnoreAllAsserts(bool value);

	//Gets if all asserts should be ignored
	bool ShouldIgnoreAllAsserts();
}}

//Include inline impimentation
#include "Internal/Assert.inl"

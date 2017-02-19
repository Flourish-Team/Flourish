#pragma once

//TODO: Add more levels (e.g Final, opt debug etc) with descriptions
//Build Macros
//Define the following Macros in the preprocessor defines to set up the build enviroment
//
// FL_BUILD_CONFIG_DEBUG	- Debug Build
// FL_BUILD_CONFIG_RELEASE	- Release Build

#define FL_DEBUG	FL_OFF
#define FL_RELEASE	FL_OFF

// Discover FL_PLATFORM_*
#if defined(FL_BUILD_CONFIG_DEBUG)
	#undef FL_DEBUG
	#define FL_DEBUG FL_ON
	
//Make sure _DEBUG is also defined
	#if !defined(_DEBUG)
		#define _DEBUG
	#endif

#elif defined(FL_BUILD_CONFIG_RELEASE)
	#undef FL_RELEASE
	#define FL_RELEASE FL_ON

	//Make sure NDEBUG is also defined
	#if !defined(NDEBUG)
		#define NDEBUG
	#endif
#else
	#error No Build config define has been set!
#endif
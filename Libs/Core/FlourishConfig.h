#pragma once
#include "Include/Macro/MacroUtils.h"
#include "Include/Macro/FeatureSwitches.h"
#include "Include/Platform/BuildConfig.h"

//-------------------------------------------------------------------------------
// Flourish Config File
//
// This file contains all the feature switches used in the engine. They can
// all be controled through this file. However the settings can also be 
// Override if needed.
//
// To edit settings you can either edit this file, or define 
// FL_CONFIG_OVERRIDE_HEADER in the preprocessor definitions to point to a 
// header file that will be included before undefined feature switches are set up

#ifdef FL_CONFIG_OVERRIDE_HEADER
	#include FL_CONFIG_OVERRIDE_HEADER
#endif

//-------------------------------------------------------------------------------
// Flourish Version Number
//
// Defines the major and minor version of the engine. Minor number should be defined
// as a 2 digit entry with leading zeros if needed e.g 04

#ifndef FL_VERSION_MAJOR
	#define FL_VERSION_MAJOR 00
#endif

#ifndef FL_VERSION_MINOR
	#define FL_VERSION_MINOR 01
#endif

//Formats the version number as the string "MAJOR.MINOR"
#ifndef FL_VERSION_STRING
	#define FL_VERSION_STRING \
		FL_STRINGIFY(FL_JOIN(FL_JOIN(FL_VERSION_MAJOR,.),FL_VERSION_MINOR))
#endif

//-------------------------------------------------------------------------------
// Asserts (FL_ASSERT_ENABLED)
// 
// Setting FL_ASSERT_ENABLED to FL_ON will enable run-time asserts in the engine.
//
// By default this is enabled for Debug builds

#if !defined(FL_ASSERT_ENABLED)
	#if FL_ENABLED(FL_DEBUG)
		#undef FL_ASSERT_ENABLED
		#define FL_ASSERT_ENABLED FL_ON
	#else
		#undef FL_ASSERT_ENABLED
		#define FL_ASSERT_ENABLED FL_OFF
	#endif
#endif

//-------------------------------------------------------------------------------
// Debug Printf (FL_DEBUG_PRINTF_ENABLED)
// 
// Setting FL_DEBUG_PRINTF_ENABLED to FL_ON will enable the FL_DEBUG_PRINTF macro,
// allowing debug printing to stdout and related debugging outputs. Otherwise calls to this
// will be compiled out
//
// By default this is enabled for Debug builds


#if !defined(FL_DEBUG_PRINTF_ENABLED)
	#if FL_ENABLED(FL_DEBUG)
		#undef FL_DEBUG_PRINTF_ENABLED
		#define FL_DEBUG_PRINTF_ENABLED FL_ON
	#else
		#undef FL_DEBUG_PRINTF_ENABLED
		#define FL_DEBUG_PRINTF_ENABLED FL_OFF
	#endif
#endif

//-------------------------------------------------------------------------------
// Stack Tracing (FL_STACK_TRACE_ENABLED)
// 
// Setting FL_STACK_TRACE_ENABLED to FL_ON will enable stack tracing via 
// Flourish::Debug::GetStackTrace(). If set to FL_OFF this will compile out to an empty
// function call.
//
// By default this is enabled for Debug builds

#if !defined(FL_STACK_TRACE_ENABLED)
	#if FL_ENABLED(FL_DEBUG)
		#undef FL_STACK_TRACE_ENABLED
		#define FL_STACK_TRACE_ENABLED FL_ON
	#else
		#undef FL_STACK_TRACE_ENABLED
		#define FL_STACK_TRACE_ENABLED FL_OFF
	#endif
#endif

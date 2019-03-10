#pragma once

// Useful macro things
// Sources:
// https://blog.molecular-matters.com/2011/07/12/a-plethora-of-macros/
// https://github.com/bkaradzic/bx/blob/master/include/bx/macros.h
// https://chromium.googlesource.com/chromium/src/base/+/master/macros.h

// Joins two tokens, even when the tokens are macros themselves
#define FL_JOIN(x,y) \
	_IMPL_FL_JOIN_A(x,y)

// Stringify some token, Even works on macros.
#define FL_STRINGIFY(str) \
	_IMPL_FL_STRINGIFY(str)

// Can be used on a variable to prevent "Unused Variable" Warnings.
#define FL_UNUSED(var) \
	_IMPL_FL_UNUSED(var)

// Put this in the declarations for a class to be uncopyable.
#define DISALLOW_COPY(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName& operator=(const TypeName&) = delete

// Put this in the declarations for a class to be unmovable. 
#define DISALLOW_MOVE(TypeName) \
	TypeName(const TypeName&&) = delete; \
	TypeName& operator=(const TypeName&&) = delete


// Put this in the declarations for a class to be uncopyable and unmovable.
#define DISALLOW_COPY_AND_MOVE(TypeName) \
  DISALLOW_COPY(TypeName);                 \
  DISALLOW_MOVE(TypeName)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
// This is especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                           \
  DISALLOW_COPY_AND_MOVE(TypeName)




//Implimentation helpers
#define _IMPL_FL_JOIN_A(x,y) _IMPL_FL_JOIN_B(x,y)
#define _IMPL_FL_JOIN_B(x,y) x##y


#define _IMPL_FL_STRINGIFY(x) #x


template <typename T> void UnusedParamater(T &&) { }
#define _IMPL_FL_UNUSED(var) UnusedParamater(var);
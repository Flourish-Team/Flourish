#pragma once

// Useful macro things
// Sources:
// https://blog.molecular-matters.com/2011/07/12/a-plethora-of-macros/
// https://github.com/bkaradzic/bx/blob/master/include/bx/macros.h

// Joins two tokens, even when the tokens are macros themselves
#define FL_JOIN(x,y) \
	_IMPL_FL_JOIN_A(x,y)

// Stringify some token, Even works on macros.
#define FL_STRINGIFY(str) \
	_IMPL_FL_STRINGIFY(str)

// Can be used on a variable to prevent "Unused Variable" Warnings.
#define FL_UNUSED(var) \
	_IMPL_FL_UNUSED(var)



//Implimentation helpers
#define _IMPL_FL_JOIN_A(x,y) _IMPL_FL_JOIN_B(x,y)
#define _IMPL_FL_JOIN_B(x,y) x##y


#define _IMPL_FL_STRINGIFY(x) #x


template <typename T> void UnusedParamater(T &&) { }
#define _IMPL_FL_UNUSED(var) UnusedParamater(var);
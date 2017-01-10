#pragma once

// Useful macro things
// Sources:
// https://blog.molecular-matters.com/2011/07/12/a-plethora-of-macros/
// https://github.com/bkaradzic/bx/blob/master/include/bx/macros.h

// Joins two tokens, even when the tokens are macros themselves
#define FL_JOIN(x,y) FL_JOIN_HELPER_A(x,y)
#define FL_JOIN_HELPER_A(x,y) FL_JOIN_HELPER_B(x,y)
#define FL_JOIN_HELPER_B(x,y) x##y

// Stringify some token, Even works on macros.
#define FL_STRINGIFY(str) FL_STRINGIFY_HELPER(str)
#define FL_STRINGIFY_HELPER(x) #x

// Can be used on a variable to prevent "Unused Variable" Warnings
#define FL_UNUSED(var) do { (void)(true ? (void)0 : ( (void)(var) ) ); } while (0)


// FL_VA_NUM_ARGS() lets you retrieve the number of arguments handed to a variable-argument macro
// unfortunately, VS 2010 still has a compiler bug which treats a __VA_ARGS__ argument as being one single parameter, hense the complicated workaround below
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details

#define FL_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#define FL_VA_NUM_ARGS_REVERSE_SEQUENCE            10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#define FL_LEFT_PARENTHESIS (
#define FL_RIGHT_PARENTHESIS )
#define FL_VA_NUM_ARGS(...)                        FL_VA_NUM_ARGS_HELPER FL_LEFT_PARENTHESIS __VA_ARGS__, FL_VA_NUM_ARGS_REVERSE_SEQUENCE FL_RIGHT_PARENTHESIS

// FL_PASS_VA passes __VA_ARGS__ as multiple parameters to another macro, working around the above-mentioned bug
#define FL_PASS_VA(...)                            FL_LEFT_PARENTHESIS __VA_ARGS__ FL_RIGHT_PARENTHESIS
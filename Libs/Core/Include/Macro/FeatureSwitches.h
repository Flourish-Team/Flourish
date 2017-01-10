#pragma once

#include "MacroUtils.h"

// These macros allow you to enable and disable features of the engine safely. Any other value for the feature degine other then FL_ON or FL_OFF will 
// trigger a compile error
// e.g
//
// #define FL_SOME_DEBUG_FEATURE FL_ON
//
// #if FL_ENABLED(FL_SOME_DEBUG_FEATURE)
//    ...Do someting if enabled
// #endif
//
// #if FL_DISABLED(FL_SOME_DEBUG_FEATURE)
//    ...Do someting if disabled
// #endif

#define FL_ON =
#define FL_OFF !

#define FL_ENABLED(x) (1 FL_JOIN(x,=) 1)

#define FL_DISABLED(x) (1 FL_JOIN(x,=) 2)

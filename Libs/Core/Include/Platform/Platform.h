#pragma once

#define FL_PLATFORM_WINDOWS false
#define FL_PLATFORM_OSX     false

#if defined(_WIN32) || defined(_WIN64)
    #undef FL_PLATFORM_WINDOWS
    #define FL_PLATFORM_WINDOWS true
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
    #undef FL_PLATFORM_OSX
    #define FL_PLATFORM_OSX true
#endif

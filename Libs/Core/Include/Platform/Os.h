#pragma once

#include "Platform/Platform.h"

#if FL_PLATFORM_WINDOWS
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

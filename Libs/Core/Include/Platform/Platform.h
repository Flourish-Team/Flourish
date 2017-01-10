#pragma once

#include "Macro/FeatureSwitches.h"

//Platform Macros
#define FL_PLATFORM_WINDOWS FL_OFF
#define FL_PLATFORM_OSX     FL_OFF
#define FL_PLATFORM_LINUX   FL_OFF

// Bit OSes
#define FL_ARCHITECTURE_32BIT FL_OFF
#define FL_ARCHITECTURE_64BIT FL_OFF

// Compilers
#define FL_COMPILER_MSVC	FL_OFF
#define FL_COMPILER_GCC		FL_OFF
#define FL_COMPILER_CLANG	FL_OFF

// Architectures
#define FL_CPU_ARCH_ARM		FL_OFF
#define FL_CPU_ARCH_MIPS	FL_OFF
#define FL_CPU_ARCH_PPC		FL_OFF
#define FL_CPU_ARCH_RISCV	FL_OFF
#define FL_CPU_ARCH_X86		FL_OFF	

// Endianess
#define FL_ENDIAN_LITTLE	FL_OFF
#define FL_ENDIAN_BIG		FL_OFF

//Sources
//https://github.com/bkaradzic/bx/blob/c74fa71b2a60e5a46cdc2e051571b163f9b89e1a/include/bx/platform.h
//https://sourceforge.net/p/predef/wiki/Home/


// Discover FL_PLATFORM_*
#if defined(_WIN32) || defined(_WIN64)
    #undef FL_PLATFORM_WINDOWS
    #define FL_PLATFORM_WINDOWS FL_ON
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
    #undef FL_PLATFORM_OSX
    #define FL_PLATFORM_OSX FL_ON
#else
	#error Unable to determine the platform for FL_PLATFORM_*
#endif


// Discover FL_COMPILER_*
#if defined(_MSC_VER)
	#undef  FL_COMPILER_MSVC
	#define FL_COMPILER_MSVC FL_ON
#elif defined(__clang__)
	#undef  FL_COMPILER_CLANG
	#define FL_COMPILER_CLANG FL_ON
#elif defined(__GNUC__)
	#undef  FL_COMPILER_GCC
	#define FL_COMPILER_GCC FL_ON
#else
	#error Unable to determine the compiler for FL_COMPILER_*
#endif

// Discover FL_CPU_ARCH_*
#if defined(__arm__)     || \
	defined(__aarch64__) || \
	defined(_M_ARM)
		#undef  FL_CPU_ARCH_ARM
		#define FL_CPU_ARCH_ARM FL_ON
#elif defined(__MIPSEL__)     || \
	  defined(__mips_isa_rev) || \
	  defined(__mips64)
		#undef  FL_CPU_ARCH_MIPS
		#define FL_CPU_ARCH_MIPS FL_ON
#elif defined(_M_PPC)        || \
	  defined(__powerpc__)   || \
	  defined(__powerpc64__)
		#undef  FL_CPU_ARCH_PPC
		#define FL_CPU_ARCH_PPC FL_ON
#elif defined(__riscv)   || \
	  defined(__riscv__) || \
	  defined(RISCVEL)
		#undef  FL_CPU_ARCH_RISCV
		#define FL_CPU_ARCH_RISCV FL_ON
#elif defined(_M_IX86)    || \
	  defined(_M_X64)     || \
	  defined(__i386__)   || \
	  defined(__x86_64__)
		#undef  FL_CPU_ARCH_X86
		#define FL_CPU_ARCH_X86 FL_ON
#else
	#error Unable to determine the compiler for FL_CPU_ARCH_*
#endif //

// Discover FL_ARCHITECTURE_*
#if defined(__x86_64__)    || \
	defined(_M_X64)        || \
	defined(__aarch64__)   || \
	defined(__64BIT__)     || \
	defined(__mips64)      || \
	defined(__powerpc64__) || \
	defined(__ppc64__)     || \
	defined(__LP64__)
		#undef  FL_ARCHITECTURE_64BIT
		#define FL_ARCHITECTURE_64BIT FL_ON
#else
		#undef  FL_ARCHITECTURE_32BIT
		#define FL_ARCHITECTURE_32BIT FL_ON
#endif

// Discover FL_ENDIAN_*
#if FL_ENABLED(FL_CPU_ARCH_PPC)
	#undef  FL_ENDIAN_BIG
	#define FL_ENDIAN_BIG FL_ON
#else
	#undef  FL_ENDIAN_LITTLE
	#define FL_ENDIAN_LITTLE FL_ON
#endif 

#include "Os.h"

#ifndef VCE_VCE2CONFIG_H
#define VCE_VCE2CONFIG_H

/*
 * project internal preprocessor symbol definitions and
 * platform dependent dirty hacks. 
 * vce2 core code should ride on this abstraction.
 *
 */

//#ifndef _GLIBCXX_FULLY_DYNAMIC_STRING
//#define _GLIBCXX_FULLY_DYNAMIC_STRING
//#endif

#if defined(WIN32) || defined(WIN64)
# define VCE_OS_WINDOWS
#elif defined(linux) || defined(__APPLE__) || defined(sun) || defined(__CYGWIN__)
# define VCE_OS_UNIX
# if defined(sun)
#  define VCE_OS_SOLARIS
# elif defined(__APPLE__)
#  define VCE_OS_APPLE
# elif defined(__CYGWIN__)
#  define VCE_OS_CYGWIN
# endif
# ifdef linux
#  define VCE_OS_LINUX
# endif
#elif defined(__MWERKS__) && defined(SDK_ARM9)
# define VCE_OS_NITRO
# include <string.h> // not <cstring>; to omit std:: prefix for stdc functions
# define VCE_ARCH_ARM
#elif defined(RVL)
# define VCE_OS_REVOLUTION
#elif defined(_XBOX)
# define VCE_OS_XBOX
#elif defined(__PPU__)
# define VCE_OS_PS3
#elif defined(__psp__)
# define VCE_OS_PSP
# define VCE_ARCH_MIPS
#else
#error "VCE: unknown platform"
#endif

/* __XXX_ENDIAN__ comes from darwin gcc */
#if defined(__BIG_ENDIAN__) || defined(VCE_OS_REVOLUTION) || defined(VCE_OS_XBOX)
# define VCE_BIG_ENDIAN
#else
# define VCE_LITTLE_ENDIAN
#endif

#if defined(VCE_OS_WINDOWS) || defined(VCE_OS_LINUX)
// XXX: we should enable it other platforms.
# define VCE_HAS_STREAM_EXT
#endif

// VCE2 for embedded.
#if defined(VCE_OS_NITRO) || defined(VCE_OS_REVOLUTION) || defined(VCE_OS_XBOX) || defined(VCE_OS_PS3) || defined(VCE_OS_PSP) || defined(VCE_OS_APPLE)
# define VCE_EMBEDDED_EDITION
#endif

#endif//VCE_VCE2CONFIG_H

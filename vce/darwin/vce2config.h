
#ifndef VCE_VCE2CONFIG_H
#define VCE_VCE2CONFIG_H

/*
 * project internal preprocessor symbol definitions and
 * platform dependent dirty hacks. 
 * vce2 core code should ride on this abstraction.
 *
 */

#if defined(WIN32) || defined(WIN64)
# define VCE_OS_WINDOWS
#elif defined(linux) || defined(__APPLE__) || defined(sun) || defined(__CYGWIN__)
# define VCE_OS_UNIX
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
#else
#error "VCE: unknown platform"
#endif

/* __XXX_ENDIAN__ comes from darwin gcc */
#if defined(__BIG_ENDIAN__) || defined(VCE_OS_REVOLUTION) || defined(VCE_OS_XBOX)
# define VCE_BIG_ENDIAN
#else
# define VCE_LITTLE_ENDIAN
#endif

#endif//VCE_VCE2CONFIG_H

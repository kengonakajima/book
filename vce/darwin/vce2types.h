
#ifndef VCE_VCE2TYPES_H
#define VCE_VCE2TYPES_H

#include "vce2config.h"

namespace vce
{
#if defined(VCE_OS_WINDOWS) || defined(VCE_OS_XBOX)

    typedef unsigned __int64 VUint64;
    typedef __int64 VSint64;

#elif defined(VCE_OS_UNIX) || defined(VCE_OS_PS3) || defined(VCE_OS_PSP)

    typedef unsigned long long VUint64;
    typedef signed long long VSint64;

#elif defined(VCE_OS_NITRO) || defined(VCE_OS_REVOLUTION)

	typedef unsigned long long int VUint64;
	typedef signed long long int VSint64;

#endif

    typedef unsigned int VUint32;
    typedef int VSint32;
    typedef unsigned short VUint16;
    typedef short VSint16;
    typedef unsigned char VUint8;
    typedef char VSint8;

    typedef char utf8;

	typedef bool VBOOL;

/*
 * buffer qualifier:
 * large buffer on stack is not allowed on tiny architectures
 * CAUTION:
 *   VCE_BUF_Q makes VCE2 non-reentrant, so we should minimize the usage.
 */
#if defined(VCE_OS_NITRO) || defined(VCE_OS_REVOLUTION) || defined(VCE_OS_PSP)
# define VCE_BUF_Q static
#else
# define VCE_BUF_Q
#endif

}

#endif//VCE_VCE2TYPES_H

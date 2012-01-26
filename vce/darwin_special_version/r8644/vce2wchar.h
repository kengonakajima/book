
#ifndef VCE_VCE2WCHAR_H
#define VCE_VCE2WCHAR_H
#include "vce2config.h"
#include <string>
#include <vector>
namespace vce
{
#if defined(VCE_OS_WINDOWS)||defined(VCE_OS_XBOX)
    typedef wchar_t wchar;
	typedef std::wstring vwstring;
#else
    typedef unsigned short wchar;
	typedef std::basic_string<vce::wchar> vwstring;
#endif
}
#endif

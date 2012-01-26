
#ifndef VCE_VCE2UTIL_H
#define VCE_VCE2UTIL_H

#include "vce2.h"

#include <ostream>

inline std::ostream&operator <<(std::ostream&o,vce::VCE_EXCEPTION type)
{
	o << vce::ExceptionToString(type);
	return o;
}

inline std::ostream&operator<<(std::ostream&o,vce::VCE_CLOSEREASON type)
{
	o << vce::CloseReasonToString(type);
	return o;
}

#ifndef VCE_EMBEDDED_EDITION
inline std::ostream&operator<<(std::ostream&o,vce::Codec::DIRECTCONTACT_PROGRESS type)
{
	switch(type)
	{
	case vce::Codec::DIRECTCONTACT_PROGRESS_COMPLETE:
		o<<"DIRECTCONTACT_PROGRESS_COMPLETE";
		break;
	case vce::Codec::DIRECTCONTACT_PROGRESS_SEND:
		o<<"DIRECTCONTACT_PROGRESS_SEND";
		break;
	case vce::Codec::DIRECTCONTACT_PROGRESS_RESEND:
		o<<"DIRECTCONTACT_PROGRESS_RESEND";
		break;
	case vce::Codec::DIRECTCONTACT_PROGRESS_TIMEOUT:
		o<<"DIRECTCONTACT_PROGRESS_TIMEOUT";
		break;
	default:
		o<<"unknown";
		break;
	}
	return o;
}
#endif

#endif//VCE_VCE2UTIL_H

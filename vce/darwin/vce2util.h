
#ifndef VCE_VCE2UTIL_H
#define VCE_VCE2UTIL_H

#include "vce2.h"

#include <ostream>

inline std::ostream&operator <<(std::ostream&o,vce::VCE_EXCEPTION type)
{
	switch(type)
	{
	case vce::VCE_EXCEPT_UNKNOWN:
		o<<"VCE_EXCEPT_UNKNOWN";
		break;
	case vce::VCE_EXCEPT_CREATESOCKETFAIL:
		o<<"VCE_EXCEPT_CREATESOCKETFAIL";
		break;
	case vce::VCE_EXCEPT_BINDFAIL:
		o<<"VCE_EXCEPT_BINDFAIL";
		break;
	case vce::VCE_EXCEPT_LISTENINGFAIL:
		o<<"VCE_EXCEPT_LISTENINGFAIL";
		break;
	case vce::VCE_EXCEPT_NONBLOCKING:
		o<<"VCE_EXCEPT_NONBLOCKING";
		break;
	case vce::VCE_EXCEPT_CONNECTFAIL:
		o<<"VCE_EXCEPT_CONNECTFAIL";
		break;
	case vce::VCE_EXCEPT_CONNECTTIMEOUT:
		o<<"VCE_EXCEPT_CONNECTTIMEOUT";
		break;
	case vce::VCE_EXCEPT_CONNECTREFUSED:
		o<<"VCE_EXCEPT_CONNECTREFUSED";
		break;
	case vce::VCE_EXCEPT_NAMERESOLUTIONFAIL:
		o<<"VCE_EXCEPT_NAMERESOLUTIONFAIL";
		break;
	case vce::VCE_EXCEPT_KEYEXCHANGEFAIL:
		o<<"VCE_EXCEPT_KEYEXCHANGEFAIL";
		break;
	case vce::VCE_EXCEPT_RECVBUFFER_FULL:
		o<<"VCE_EXCEPT_RECVBUFFER_FULL";
		break;
	case vce::VCE_EXCEPT_SENDBUFFER_FULL:
		o<<"VCE_EXCEPT_SENDBUFFER_FULL";
		break;
	case vce::VCE_EXCEPT_SIGPIPE:
		o<<"VCE_EXCEPT_SIGPIPE";
		break;
	case vce::VCE_EXCEPT_INVALIDDATA:
		o<<"VCE_EXCEPT_INVALIDDATA";
		break;
	case vce::VCE_EXCEPT_TIMEOUT:
		o<<"VCE_EXCEPT_TIMEOUT";
		break;
	case vce::VCE_EXCEPT_TOOMANYFILES:
		o<<"VCE_EXCEPT_TOOMANYFILES";
		break;
	case vce::VCE_EXCEPT_ALREADYLISTEN:
		o<<"VCE_EXCEPT_ALREADYLISTEN";
		break;
	case vce::VCE_EXCEPT_CONNECTIONRESET:
		o<<"VCE_EXCEPT_CONNECTIONRESET";
		break;
	case vce::VCE_EXCEPT_OVERFLOWSENDQUEUE:
		o<<"VCE_EXCEPT_OVERFLOWSENDQUEUE";
		break;
	case vce::VCE_EXCEPT_OVERFLOWSENDBUFFER:
		o<<"VCE_EXCEPT_OVERFLOWSENDBUFFER";
		break;
	default:
		o<<"unknown";
		break;
	}
	return o;
}

inline std::ostream&operator<<(std::ostream&o,vce::VCE_CLOSEREASON type)
{
	switch(type)
	{
	case vce::VCE_CLOSE_UNKNOWN:
		o<<"VCE_CLOSE_UNKNOWN";
		break;
	case vce::VCE_CLOSE_LOCAL:
		o<<"VCE_CLOSE_LOCAL";
		break;
	case vce::VCE_CLOSE_REMOTE:
		o<<"VCE_CLOSE_REMOTE";
		break;
	case vce::VCE_CLOSE_RECVBUFFERFULL:
		o<<"VCE_CLOSE_RECVBUFFERFULL";
		break;
	case vce::VCE_CLOSE_RECVERROR:
		o<<"VCE_CLOSE_RECVERROR";
		break;
	case vce::VCE_CLOSE_INVALIDDATA:
		o<<"VCE_CLOSE_INVALIDDATA";
		break;
	default:
		o<<"unknown";
		break;
	}
	return o;
}

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

#endif//VCE_VCE2UTIL_H

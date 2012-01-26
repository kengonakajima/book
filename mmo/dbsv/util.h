#ifndef _UTIL_H_
#define _UTIL_H_

void * MySelectAlgorithmCallback(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata);

#define ARRAYLEN(x) ( sizeof(x)/sizeof(x[0]))
#endif

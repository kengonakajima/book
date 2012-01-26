#ifndef _UTIL_H_
#define _UTIL_H_

void * MySelectAlgorithmCallback(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata);
std::string makeHashString( std::string s);
std::size_t hashval( std::string s);

#define ARRAYLEN(x) ( sizeof(x)/sizeof(x[0]))

#define minvalue(a,b)  (((a) < (b)) ? (a) : (b))
#define maxvalue(a,b)  (((a) > (b)) ? (a) : (b))
#define absvalue(a) (((a) < (0)) ? (-(a)) : (a))

#endif

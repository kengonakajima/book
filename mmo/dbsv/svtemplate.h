#ifndef _DBSVMAIN_H_
#define _DBSVMAIN_H_

#include "dbproto.h"
#include <iostream>


class DBServer : public db_proto_server
{

 public:
    DBServer(){ log=&std::cerr;}

	// vce funcs
	void Attached(){}
	void Connected(){
        log=&std::cerr;
        std::cerr << "accepted a client" << std::endl;
    }
	void Closed(vce::VCE_CLOSEREASON type){}
	void Detached(){}
	void Exception( vce::VCE_EXCEPTION type ){}
	size_t Recv(const vce::VUint8 *p,size_t sz);

    void recv_ping( vce::VUint64 timestamp );
	void recv_getNewID(vce::VUint32 num);


    // simple putter funcs
    //{% for t in db.tables %}
    // table {{t.name}}
    void recv_put_{{t.name}}( vce::VUint32 sessionID, {{t.struct_name}} data );

    // simple getter funcs
    //  {% for f in t.fields %}
    //    {% if f.index or f.primary %}
    void recv_get_{{t.name}}_by_{{f.name}}( vce::VUint32 sessionID, {{f.type_in_cpp}} {{f.name}} );

    void recv_get_{{t.name}}_ranking_by_{{f.name}}( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    //    {% endif %}    
    //    {% if f.conditional %}
    void recv_put_{{t.name}}_if_{{f.name}}( vce::VUint32 sessionID, {{t.struct_name}} data, {{f.type_in_cpp}} {{f.name}}_test );
    //    {% endif %}
    //  {% endfor %}
    //{% endfor %}
};




#endif

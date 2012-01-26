#ifndef _DBCLMAIN_H_
#define _DBCLMAIN_H_

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>

#include "dbproto.h"

class IDPool
{
    // Last in Last out.
    std::size_t stackPos; // 新しいIDを何個貯めてるか
    std::size_t stackMax; // 最大数
    vce::VUint64 *stack;
 public:
    IDPool( std::size_t sz ){
        stack = (vce::VUint64*)malloc( sz * sizeof(vce::VUint64));
        assert(stack);
        memset(stack, 0,  sz*sizeof(vce::VUint64));
        stackPos=0;
        stackMax = sz;
    }
    
    // 追加する。 追加できたらtrue
    bool add( vce::VUint64 val ){
        if( stackPos == stackMax ){
            return false;
        }
        stack[stackPos] = val;
        stackPos++;
        return true;
    }
    // 取得して在庫を減らす。 無かったらfalse
    vce::VUint64 get(){
        assert( stackPos > 0 );
        vce::VUint64 val = stack[stackPos-1];
        stack[stackPos-1]=0;
        stackPos--;
        std::cerr << "id.h: get() returns " << val << std::endl;
        return val;
    }
    vce::VUint64 getLeft(){
        return stackPos;
    }
    
};





class DBClient : public db_proto_client
{
    // network flags
    bool idQuerying;


    
 public:
    vce::VUint64 lastPingSentAt;
    vce::VUint64 lastPingRecvFromServer;

    vce::VUint64 getLatency(){
        return lastPingSentAt - lastPingRecvFromServer;
    }
    DBClient(){
        idQuerying = false;
        lastPingSentAt = 0;
        lastPingRecvFromServer = 0;
    };

    // work funcs
    void setIDQuerying( bool flag ){  idQuerying = flag; }
    bool getIDQuerying(){ return idQuerying; }

    
    // extend
    void Connected(){};
    void Closed(vce::VCE_CLOSEREASON reason ){
        std::cerr << "dbsv died? reason:" << reason << std::endl;
        assert(0);        
    };
    void recv_ping( vce::VUint64 timestamp );
	void recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt);

    
    // {% for t in db.tables %}
    void recv_put_{{t.name}}_result( vce::VUint32 sessionID, ResultCode result, {{t.struct_name}} data );    
    //   {% for f in t.fields %}
    //     {% if f.index or f.primary %}
    void recv_get_{{t.name}}_by_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} *data, vce::VUint32 data_qt );
    void recv_get_{{t.name}}_ranking_by_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} *data, vce::VUint32 data_qt );    
    //     {% endif %}
    //     {% if f.conditional %}
    void recv_put_{{t.name}}_if_{{f.name}}_result( vce::VUint32 sessionID, ResultCode result, const {{t.struct_name}} data );
    //     {% endif %}
    //   {% endfor %}
    // {% endfor %}
};





#endif



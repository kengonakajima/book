#ifndef _NET_H_
#define _NET_H_

#include "vce2.h"
#include "jproto.h"
#include <iostream>
#include <assert.h>

class SyncValue
{



 private:
    static const int SYNCVALUES_MAX = 32; // 一度に同期できる変数の最大個数    
    static const int BUFLEN = 128;
    vce::VUint8 m_buf[BUFLEN]; // 大量に確保しないので固定でよろしい
    size_t m_offsets[SYNCVALUES_MAX]; // どのIDが、何バイト目から始まるのか。0なら、m_buf[0]スタート。サイズは、型の種類で決まる。それはaddしていくときに決定される。
    size_t m_sizes[SYNCVALUES_MAX]; // それぞれの変数のサイズ
    bool m_changes[SYNCVALUES_MAX]; // どのIDが変更されてるかの記録
    bool m_uses[SYNCVALUES_MAX]; // どのフィールドが使われているか
    size_t m_currentOffset;

    void reg( int index, size_t sz ){
        assert(index<SYNCVALUES_MAX);
        m_offsets[index] = m_currentOffset;
        m_sizes[index] = sz;
        m_uses[index] = true;
        m_currentOffset += sz;
        assert( sz < BUFLEN );
    }
    size_t ofs(int index){
        assert(index<SYNCVALUES_MAX);
        return m_offsets[index];
    }
        
 public:

    // 必要なデータをすべてゼロ初期化
    SyncValue() : m_currentOffset(0), m_offsets(), m_sizes(), m_changes(), m_uses() {

    }


    void registerCharType( int index ){
        reg(index, 1);
    }
    void registerIntType( int index ){
        reg(index, 4 );
    }
    void registerFloatType( int index ){
        reg(index, 4 );
    }

    void setChar( int index, char val ){
        char prev = ((char*)(m_buf+ofs(index)))[0];
        if( prev != val ){
            ((char*)(m_buf+ofs(index)))[0] = val;
            m_changes[index]=true;
        }
    }
    void setInt( int index, int val ){
        int prev = ((int*)(m_buf+ofs(index)))[0];
        if( prev != val ){
            ((int*)(m_buf+ofs(index)))[0] = val;
            m_changes[index]=true;
        }
    }
    void setFloat( int index, float val ){
        float prev = ((float*)(m_buf+ofs(index)))[0];
        if( prev != val ){
            ((float*)(m_buf+ofs(index)))[0] = val;
            m_changes[index]=true;
        }
    }
    char getChar( int index ){
        return ((char*)(m_buf+ofs(index)))[0];
    }
    int getInt( int index ){
        return ((int*)(m_buf+ofs(index)))[0];
    }
    float getFloat( int index ){
        return ((float*)(m_buf+ofs(index)))[0];
    }

    void clearChanges(){
        fillChanges(false);
    }
    void fillChanges( bool flag ){
        for(int i=0;i<SYNCVALUES_MAX;i++){
            if( m_uses[i] ){
                m_changes[i]=flag;
            }
        }
    }

    bool isChanged( int index ){
        assert( index < SYNCVALUES_MAX );
        return m_changes[index];
    }

    void getDiffBuff( vce::VUint8 *outbuf, size_t *outbuflen );
    void dumpDiffBuff(){
        vce::VUint8 buf[BUFLEN];
        size_t buflen;
        getDiffBuff( buf, &buflen  );
        dumpBuffer( buf, buflen );
    }
    void dumpBuffer( const vce::VUint8* todump, size_t len ){
        for(int i=0;i< len;i++){
            std::cerr << (int)(todump[i] ) << " ";
        }
        std::cerr << std::endl;
    }

    void readBuffer( const vce::VUint8 *inbuf, size_t inbuflen );
    
    static void selfTest();
    
};



class JServer : public j_proto_server
{
    
 public:
    JServer(){
    };

	// vce funcs
	void Attached(){};
	void Connected();
	void Closed(vce::VCE_CLOSEREASON type){};
	void Detached(){};
	void Exception( vce::VCE_EXCEPTION type ){};
	size_t Recv(const vce::VUint8 *p,size_t sz){
        return j_proto_server::Recv(p,sz);

    };



    virtual void recv_sync(vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data,vce::VUint32 data_qt);
	virtual void recv_delete(vce::VUint32 guestid, vce::VUint32 id);
	virtual void recv_ping(vce::VUint64 guestclock);
	virtual void recv_getid(vce::VUint16 num);
};


class JClient : public j_proto_client
{


 public:
    bool m_readyToStartGame;

    vce::VUint64 m_lastRoundTripTime;
    vce::VUint64 m_lastHostTimeDiff; // pong受信時点での時差
    
 JClient() : m_readyToStartGame(false) {
    }

    void ready(){ m_readyToStartGame = true; }
    
	void Closed(vce::VCE_CLOSEREASON reason){
        std::cerr << "host connection closed." << std::endl;
    };
    bool isSendable(){
        return( GetState() == vce::VCE_STATE_ESTABLISHED );
    }
        
    
	virtual void recv_guestinfo(vce::VUint32 id);
	virtual void recv_sync(vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data,vce::VUint32 data_qt);
	virtual void recv_delete(vce::VUint32 guestid, vce::VUint32 id);
	virtual void recv_pong(vce::VUint64 hostclock,vce::VUint64 guestclock);
	virtual void recv_getid_result(const vce::VUint64 *idarray,vce::VUint32 idarray_qt);

    
};





class GameListener : public vce::Listener 
{
private:
    
	vce::Session * Accepter( vce::VUint32 remote_addr, vce::VUint16 remote_port );
	void Detached(){ delete this;        };
	void Exception( vce::VCE_EXCEPTION type );

 public:
    GameListener(){
    }
};


extern GameListener *g_listener;



#endif

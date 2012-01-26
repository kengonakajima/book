#include "net.h"
#include "movable.h"
#include "app.h"
#include "util.h"

void JServer::recv_ping(vce::VUint64 guestclock)
{
    send_pong( vce::GetTime(), guestclock );
}

void JClient::recv_pong(vce::VUint64 hostclock, vce::VUint64 guestclock )
{
    
    m_lastRoundTripTime = vce::GetTime() - guestclock;
    m_lastHostTimeDiff = abs(vce::GetTime() - hostclock);
    std::cerr << "recv pong: host:" << hostclock << " guest:" << guestclock << " diff:" << m_lastHostTimeDiff << std::endl;    
}

// format:
// 00: index
// 01: 値の0バイト目  の列
// short, float, float の列だったら
// ind s0 s1 ind f0 f1 f2 f3 ind f0 f1 f2 f3
// 読む方は読み尽くしたら終わり
// 元のデータはbufからサイズ分だけCOPYする

void SyncValue::getDiffBuff( vce::VUint8 *outbuf, size_t *outbuflen )
{
    *outbuflen = 0;
    size_t writeOfs=0;

    for(int i=0;i<SYNCVALUES_MAX;i++){
        if( m_changes[i] ){
            outbuf[writeOfs] = static_cast<vce::VUint8>( i );
            writeOfs++;
            memcpy( outbuf + writeOfs, m_buf + ofs(i), m_sizes[i] );
            writeOfs += m_sizes[i];
            //            std::cerr << "i:" << i << " ofs:" << ofs(i) << " wo:" << writeOfs << std::endl;
        }
    }
    *outbuflen = writeOfs;
    //    if( writeOfs>0) std::cerr << "outbuflen:" << *outbuflen << std::endl;
}

// バッファから読み込んで値を更新するが、全部ではないので、 m_changesにフラグを立てる
void SyncValue::readBuffer( const vce::VUint8 *inbuf, size_t inbuflen )
{
    size_t readOfs=0;

    for(;;){
        vce::VUint8 index = inbuf[readOfs];
        //        std::cerr << "i:" << (int)(index) << " ";
        assert(index<SYNCVALUES_MAX);
        readOfs ++;
        memcpy( m_buf + ofs( index ), inbuf + readOfs, m_sizes[index] );
        //        std::cerr << "sz:" << m_sizes[index] << " ";
        assert( (m_sizes[index] == 1 )|| (m_sizes[index] == 4 ) );
                
        readOfs += m_sizes[index];
        m_changes[index]=true;
        if( readOfs == inbuflen ) return;
    }
    assert(0); // never reaches
}



// syncを受信した。
void JClient::recv_sync( vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data,vce::VUint32 data_qt)
{
    //        std::cerr << "recv_sync: id:" << id << " buflen:" << data_qt << std::endl;
    if( m_readyToStartGame ){
        Movable::receiveSyncValueBuffer( guestid, id, data, data_qt );
    }
        
}

void JClient::recv_delete(vce::VUint32 guestid, vce::VUint32 id)
{
    if( m_readyToStartGame ){
        Movable::receiveDelete( guestid, id );
    }
    
}

void JServer::recv_delete( vce::VUint32 guestid, vce::VUint32 id )
{
    Movable::receiveDelete( guestid, id );
        
    // Movableが見つかっても、見つからなくても、全ゲストに同報する
    std::vector<JServer*> vs = g_app->getServerSessions();
    std::vector<JServer*>::iterator its;
    for(its=vs.begin(); its != vs.end(); ++ its ){
        JServer *js = (*its);
        js->send_delete( guestid, id );
    }
}


void JServer::recv_getid(vce::VUint16 num)
{
    vce::VUint64 outbuf[1000];
    int i;
    for(i=0;i<num && i < ARRAYLEN(outbuf);i++){
        outbuf[i] = g_app->getNewID();
    }
    send_getid_result( outbuf, i );
}

void JClient::recv_getid_result(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
    std::cerr << "recv_getid_result got:" << idarray_qt << std::endl;
    for(int i=0;i<idarray_qt; i++){
        g_app->m_idpool->add( idarray[i]);
        //        std:: cerr << "i"<<idarray[i] << " ";
    }
}
void JClient::recv_guestinfo(vce::VUint32 id)
{
    g_app->m_guestid = id;
}


void JServer::Connected()
{
    send_guestinfo( uID );
}

void SyncValue::selfTest()
{
    SyncValue *v = new SyncValue();
    v->registerIntType( 0 );
    v->registerIntType( 1 );
    v->registerFloatType( 7 );
    v->registerFloatType( 8 );
    v->registerFloatType( 9 );
    v->registerFloatType( 10 );
    v->registerCharType( 15 );

    v->setInt( 0, 11111 );
    v->setInt( 1, 22222 );
    v->setFloat( 7, 3.3333f );
    v->setFloat( 8, 4.4444f );
    v->setFloat( 9, 5.5555f );
    v->setFloat( 10, 6.6666f );
    v->setChar( 15, 111 );

    vce::VUint8 buf[128];
    size_t buflen;
    v->getDiffBuff( buf, &buflen );
    assert( buflen == ((1+4)*2+(1+4)*4+(1+1)*1));

    SyncValue *vv = new SyncValue();
    vv->registerIntType( 0 );
    vv->registerIntType( 1 );
    vv->registerFloatType( 7 );
    vv->registerFloatType( 8 );
    vv->registerFloatType( 9 );
    vv->registerFloatType( 10 );
    vv->registerCharType( 15 );
    vv->readBuffer( buf, buflen );
    assert( vv->getInt( 0 ) == 11111 );
    assert( vv->getInt( 1 ) == 22222 );
    assert( vv->getFloat( 7 ) == 3.3333f );
    assert( vv->getFloat( 8 ) == 4.4444f );
    assert( vv->getFloat( 9 ) == 5.5555f );
    assert( vv->getFloat( 10 ) == 6.6666f );
    assert( vv->getChar( 15 ) == 111 );

    delete vv;    
    delete v;
}

void JServer::recv_sync(vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data,vce::VUint32 data_qt)
{

    //    std::cerr << "recv_sync: id:" << id << " buflen:" << data_qt << std::endl;    
    // Playerの動きはすべて同期
    std::vector<JServer*> vs = g_app->getServerSessions();
    std::vector<JServer*>::iterator its;
    for(its=vs.begin(); its != vs.end(); ++ its ){
        JServer *js = (*its);
        if( js != this ){
            js->send_sync( guestid, id, data, data_qt );
        }
    }
    Movable::receiveSyncValueBuffer( guestid, id, data, data_qt );
    
}


#include "../gmsv/common.h"
#include "kproto.h"

#include "kcli.h"
#include "app.h"


#include <assert.h>

void KClient::Closed(vce::VCE_CLOSEREASON reason)
{
    std::cerr << "closed..." << std::endl;
}
void KClient::Detached()
{
    g_app->kcli = NULL;
    g_app->state = App::TEST_SESSION_CLOSED;
}


void KClient::recv_ping( vce::VUint64 t )
{
    if( g_app->state == App::TEST_INGAME ){
        g_app->lastRoundTripTime = vce::GetTime() - g_app->lastPingSentAt;
        std::cerr << " now:" << vce::GetTime() << std::endl;
    } else {
        if( g_app->state == App::TEST_PING_SENT ){
            g_app->state = App::TEST_PING_RECEIVED;
            std::cerr << "ping-recv ok" << std::endl;            
        }
    }
}

void KClient::recv_signupResult(ResultCode result)
{
    std::cerr << "result: " << result << std::endl;
    assert( g_app->state == App::TEST_SIGNUP_SENT);
    assert( result == SUCCESS || result == ALREADY );
    g_app->state = App::TEST_SIGNUP_RECEIVED;
    std::cerr << "signup ok" << std::endl;
    
}

void KClient::recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt)
{
    if( g_app->state == App::TEST_LISTCHARACTER_SENT ){
        assert( result == SUCCESS );
        g_app->state = App::TEST_LISTCHARACTER_RECEIVED;
        std::cerr << "recv_listCharacterResult ok. qt:" << stat_qt << std::endl;
    }    
}

    
void KClient::recv_createCharacterResult(ResultCode result)
{
    assert( g_app->state == App::TEST_CREATECHARACTER_SENT );
    assert( result == SUCCESS );
    g_app->state = App::TEST_CREATECHARACTER_RECEIVED;
    std::cerr << "recv_createCharacterResult ok." << std::endl;
    
}

void KClient::recv_authenticationResult(ResultCode result)
{
    assert( g_app->state == App::TEST_AUTHENTICATION_SENT);
    assert( result == SUCCESS );
    g_app->state = App::TEST_AUTHENTICATION_RECEIVED;
    std::cerr << "recv_authenticationResult ok." << std::endl;
    
}

void KClient::recv_loginResult(ResultCode result,vce::VUint32 movableID)
{
    std::cerr << "recv_login result: " << result << std::endl;    
    assert(g_app->state == App::TEST_LOGIN_SENT );
    assert( result == SUCCESS );
    std::cerr << "recv_login OK. my id: " << movableID << std::endl;
    g_app->state = App::TEST_LOGIN_RECEIVED;

    // 新しい鯖にログインしたら古いのを消す
    if( g_app->myMovableID != 0 ){
        g_app->deleteMovable( g_app->myMovableID );
    }
    g_app->myMovableID = movableID;


}

    

void KClient::recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 floorID )
{
    //    std::cerr << "move: movableID:" << movableID << " xy:" << posx << "," << posy << std::endl;
    assert( g_app->state >= App::TEST_LOGIN_RECEIVED );

    if( movableID == g_app->myMovableID ){
        g_app->myCoord = Coord( posx, posy );
        g_app->myFloorID = floorID;
    }
    if( g_app->getMovable(movableID) == NULL ){
        CliMovable *m = new CliMovable( movableID, typeID, Coord( posx, posy ) );
        assert(m);
        g_app->movmap[movableID] = m;
        send_characterStatus( movableID );
    } else {
        g_app->movmap[movableID]->setCoord( Coord( posx, posy ) );
        g_app->movmap[movableID]->typeID = typeID;
    }
    
}


void KViewClient::recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 f )
{
    std::cerr << "kv move:" << Coord(posx,posy).to_s() << " id:" << movableID << std::endl;
    if( movableID == g_app->myMovableID )return;

    if( g_app->getMovable(movableID) == NULL ){
        CliMovable *m = new CliMovable( movableID, typeID, Coord( posx, posy ) );
        assert(m);
        g_app->movmap[movableID] = m;
        send_characterStatus( movableID );
    } else {
        g_app->movmap[movableID]->setCoord( Coord( posx, posy ) );
        g_app->movmap[movableID]->typeID = typeID;
    }

}



void KClient::recv_disappearNotify(vce::VSint32 movableID)
{
    std::cerr << "dis: movableID:" << movableID << std::endl;
    assert( g_app->state >= App::TEST_LOGIN_RECEIVED );

    if( movableID == g_app->myMovableID )return;

    g_app->deleteMovable(movableID);
    
}
void KViewClient::recv_disappearNotify(vce::VSint32 movableID)
{
    if( movableID == g_app->myMovableID )return;
    g_app->deleteMovable(movableID);
    
}

void KClient::recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat)
{
    if( movableID == g_app->myMovableID ){
        g_app->myCharacterStatus = charstat;
    }
}


void KClient::recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text)
{
}

void KClient::recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage )
{
    std::cerr << "attacked id:" << attackedMovableID << std::endl;
    CliMovable *m = g_app->getMovable( attackedMovableID );
    if(m){
        m->changed = true; //状態だけ更新して表示はappに任せる
        m->attacked = true;
        m->lastAttackDamage = damage;
    }
}

void KClient::recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt)
{
}

void KClient::recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt)
{
    std::cerr << "LLLLLL: " << data_qt << std::endl;
    std::cerr << "xyxy:" << x1 << "," << y1 << "," << x2 << "," << y2 << std::endl;
    
    int i=0;
    for(int y=y1; y < y2; y++ ){
        for(int x = x1; x < x2; x++ ){
            g_app->floor->setTile( Coord(x,y), data[i++] );
        }
    }

}
void KClient::recv_viewmodeResult(ResultCode result)
{
}


void KClient::recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID){}
void KClient::recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType){}  
void KClient::recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType){}
void KClient::recv_shopNotify(vce::VSint32 shopMovableID){}  
void KClient::recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price){}  
void KClient::recv_buyResult(vce::VSint32 resultCode){}  
void KClient::recv_sellResult(vce::VSint32 resultCode){}  

    


size_t KClient::Recv(const vce::VUint8 *p,size_t sz)
{
    g_app->recvBytesAccum += sz;
    return k_proto_client::Recv(p,sz);
};	


void KViewClient::Detached()
{
    std::cerr << "KViewClient:: detached." << std::endl;
    g_app->kvcli = NULL;
}

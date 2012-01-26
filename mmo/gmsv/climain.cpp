#include "vce2.h"
#include "kproto.h"
#include <iostream>
#include <assert.h>

#include "climain.h"
#include "util.h"


vce::VCE *g_vceobj;
int g_id_generator = 1;

Config conf;

int main(int argc, char *argv[])
{
    srand(time(NULL));
 
    vce::VUint64 testPeriod = 20 * 1000; // ms テストをこの秒数だけ続ける
    
    vce::VCEInitialize();
    g_vceobj = vce::VCECreate();
    g_vceobj->ReUseAddress(true);
	g_vceobj->SetPollingWait(true);    
    g_vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);    

    KClient *kcli = new KClient();
    
    assert(kcli);


    int port = 29000;
    const char *host="localhost";

    if( !g_vceobj->Connect( kcli, host, port ) ){
        std::cerr << "fatal error: cannot connect to gmsv" << std::endl;
        assert(0);        
    } else {
        std::cerr << "connecting to gmsv:" <<  host << ":"<<port<<std::endl;
        g_vceobj->Poll();
    }    

    vce::VUint64 testStarted = vce::GetTime();
    vce::VUint32 vcebugcounter = 0;
        
    while(true){
        g_vceobj->Poll();
        if( kcli->Poll() == false ){
            vcebugcounter ++;
            if( vcebugcounter > 100 ){ // 100回連続で接続できなかったら再挑戦
                std::cerr << "connect fail.." << std::endl;
                exit(1);
            }
        }

        if( vce::GetTime() > (testStarted+10*1000)){
            break;
        }
    }


    assert( kcli->evaluate());
    std::cerr << "bot test success!" << std::endl;
}

// 結果をテストする
bool KClient::evaluate()
{
    assert( recvCounter[FUNCTION_MOVENOTIFY] > 0 );
    assert( recvCounter[FUNCTION_ATTACKNOTIFY] > 0 );
    assert( recvCounter[FUNCTION_LANDSCAPERESULT] >= 6 );
    assert( recvCounter[FUNCTION_CHARACTERSTATUSRESULT] > 0 );
    
    return true;
}

void KClient::Closed(vce::VCE_CLOSEREASON reason)
{
    assert( reason == vce::VCE_CLOSE_REMOTE );
    assert( state == TEST_LOGOUT_SENT );
    std::cerr << "session closed" << std::endl;
    state = TEST_SESSION_CLOSED;
}

void KClient::recv_ping( vce::VUint64 t )
{
    assert( state == TEST_PING_SENT );
    state = TEST_PING_RECEIVED;
    std::cerr << "ping-recv ok" << std::endl;
}

void KClient::recv_signupResult( ResultCode result )
{
    std::cerr << "result: " << result << std::endl;
    assert( state == TEST_SIGNUP_SENT);
    assert( result == SUCCESS || result == ALREADY );
    state = TEST_SIGNUP_RECEIVED;
    std::cerr << "signup ok" << std::endl;
}

        
void KClient::recv_loginResult(ResultCode result, vce::VUint32 movableID )
{
    std::cerr << "recv_login result: " << result << std::endl;    
    assert(state == TEST_LOGIN_SENT );
    assert( result == SUCCESS );
    std::cerr << "recv_login OK. my id: " << movableID << std::endl;
    state = TEST_LOGIN_RECEIVED;
    myMovableID = movableID;
}
void KClient::recv_authenticationResult(ResultCode result)
{
    assert( state == TEST_AUTHENTICATION_SENT);
    assert( result == SUCCESS );
    state = TEST_AUTHENTICATION_RECEIVED;
    std::cerr << "recv_authenticationResult ok." << std::endl;
}
void KClient::recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt)
{
    assert( state == TEST_LISTCHARACTER_SENT );
    assert( result == SUCCESS );
    state = TEST_LISTCHARACTER_RECEIVED;
    std::cerr << "recv_listCharacterResult ok. qt:" << stat_qt << std::endl;
}
void KClient::recv_createCharacterResult(ResultCode result)
{
    assert( state == TEST_CREATECHARACTER_SENT );
    assert( result == SUCCESS );
    state = TEST_CREATECHARACTER_RECEIVED;
    std::cerr << "recv_createCharacterResult ok." << std::endl;
}

void KClient::recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 floorID )
{
    std::cerr << "move: movableID:" << movableID << std::endl;
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_MOVENOTIFY] ++;
    moveCounter[typeID]++;

    if( movableID == myMovableID ){
        std::cerr << "newcoord:" << Coord(posx,posy).to_s() << std::endl;
        myCoord.x = posx;
        myCoord.y = posy;
    }
    if( movmap[movableID] == NULL ){
        TestMovable *m = new TestMovable( movableID, typeID, floorID, Coord( posx, posy ) );
        assert(m);
        movmap[movableID] = m;
        send_characterStatus( movableID );
        sendCounter[FUNCTION_CHARACTERSTATUS]++;
    } else {
        movmap[movableID]->coord.x = posx;
        movmap[movableID]->coord.y = posy;
        movmap[movableID]->floorID = floorID;
        movmap[movableID]->typeID = typeID;
    }
        
}
void KClient::recv_disappearNotify(vce::VSint32 movableID)
{
    std::cerr << "dis: movableID:" << movableID << std::endl;
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_DISAPPEARNOTIFY] ++;

    if( movmap[movableID] ){
        delete movmap[movableID];
        movmap.erase(movableID);
    } else {
        movmap.erase(movableID); // TODO: mapがデフォルト値を作ってしまう仕様はトリッキー
    }
}

void KClient::recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage )
{
    std::cerr << "atk: movableID:" << attackedMovableID << std::endl;
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_ATTACKNOTIFY] ++;

}
void KClient::recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *tileType,vce::VUint32 tileType_qt)
{
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_LANDSCAPERESULT]++;

    std::cerr << "tileType_qt:" << tileType_qt << std::endl;
    assert( tileType_qt == 1024 );
    for(int i=0;i<tileType_qt;i++){
        std::cerr << tileType[i] << " ";
    }
    std::cerr << std::endl;
}
void KClient::recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat)
{
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_CHARACTERSTATUSRESULT]++;    
}

void KClient::recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt)
{
    assert( state >= TEST_LOGIN_RECEIVED );
    recvCounter[FUNCTION_ITEMNOTIFY]++;
    assert( data_qt >= 2 );

    for(int i=0;i<data_qt;i++){
        if( data[i].typeID == k_proto::ITEM_SHORTSWORD ){
            send_equip(i);
            sendCounter[FUNCTION_EQUIP]++;
            break;
        }
    }
}
void KClient::recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text){}
void KClient::recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID){}
void KClient::recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)  {}
void KClient::recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType){}
void KClient::recv_shopNotify(vce::VSint32 shopMovableID){}
void KClient::recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price){}
void KClient::recv_buyResult(vce::VSint32 resultCode){}  
void KClient::recv_sellResult(vce::VSint32 resultCode){}

void KClient::ingameSender()
{
    tick++;

    int dx = -1 + ( random() % 3 );
    int dy = -1 + ( random() % 3 );
    send_move( myCoord.x + dx,
               myCoord.y + dy );
    sendCounter[FUNCTION_MOVE]++;

    // 自分以外のmovableに興味を持つ。
    std::map<vce::VUint32,TestMovable*>::iterator it;
    int cnt=0;
    for(it=movmap.begin(); it != movmap.end(); ++it ){
        TestMovable *m = (*it).second;
        assert(m);
        if( m->typeID == k_proto::MOVABLE_GOBLIN ){
            send_attack( m->id );
            sendCounter[ FUNCTION_ATTACK]++;
            cnt++;
            if( cnt == 15) break; // 一度にあまりたくさん攻撃しない
        }
    }

    // 地形取得
    if( recvCounter[FUNCTION_LANDSCAPERESULT] == 0 ){
        // 全部一気に取得
        send_landscape( 0, 0,0, 32,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
        send_landscape( 0, 32,0, 64,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
        send_landscape( 1, 0,0, 32,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
        send_landscape( 1, 32,0, 64,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
        send_landscape( 1, 0,0, 32,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
        send_landscape( 1, 32,0, 64,32 );
        sendCounter[FUNCTION_LANDSCAPE]++;
    }
    if( recvCounter[FUNCTION_ITEMNOTIFY] == 0 ){
        send_item();
        sendCounter[FUNCTION_ITEM]++;
    }
    
}
void KClient::printStat()
{
    std::cerr << "moves: ";
    for(int i=0;i<MOVABLE_MAX;i++){
        if( moveCounter[i] > 0 ){
            std::cerr << i << ":" << moveCounter[i] << " " ;
        }
    }
    std::cerr << std::endl;
    std::cerr << "recvs: disappear:" << recvCounter[FUNCTION_DISAPPEARNOTIFY] << std::endl;
    std::cerr << "recvs: attackNot:" << recvCounter[FUNCTION_ATTACKNOTIFY] << std::endl;
    std::cerr << "recvs: landscapeRes:" << recvCounter[FUNCTION_LANDSCAPERESULT] << std::endl;
    std::cerr << "recvs: charstatusRes:" << recvCounter[FUNCTION_CHARACTERSTATUSRESULT] << std::endl;
    std::cerr << "recvs: itemNot:" << recvCounter[FUNCTION_ITEMNOTIFY] << std::endl;    
    std::cerr << "sends: charstat:" << sendCounter[FUNCTION_CHARACTERSTATUS] << std::endl;
    std::cerr << "movmapnum:" << movmap.size() << std::endl;
}

bool KClient::Poll()
{
    switch(state){
    case TEST_INIT:
        if( GetState() == vce::VCE_STATE_ESTABLISHED ){
            std::cerr << "send ping" << std::endl;
            send_ping( vce::GetTime());
            state = TEST_PING_SENT;            
            break;
        } else {
            std::cerr << GetState() << ">" ;
            if( GetState() == vce::VCE_STATE_UNKNOWN ){
                return false;
            }
        }
        break;
    case TEST_PING_SENT:
        std::cerr << ">";
        break;
    case TEST_PING_RECEIVED:
        {
            
            // 新しいIDを生成して登録(sign up)する

            std::ostringstream idss;
            idss << "ringo" << time(NULL) << "." << g_id_generator;
            localAccountName = idss.str();
            localPassword = std::string("testpass");

            g_id_generator ++;
            send_signup( localAccountName.c_str(), localPassword.c_str());
            std::cerr << "signup" << std::endl;
            state = TEST_SIGNUP_SENT;

        }
        break;
    case TEST_SIGNUP_SENT:
        std::cerr << ">";
        break;
    case TEST_SIGNUP_RECEIVED:
        send_authentication( localAccountName.c_str(), localPassword.c_str());
        state = TEST_AUTHENTICATION_SENT;
        std::cerr << "auth" << std::endl;
        break;
        
    case TEST_AUTHENTICATION_SENT:
        std::cerr << ">";        
        break;
    case TEST_AUTHENTICATION_RECEIVED:
        send_createCharacter( "ringo" );
        state = TEST_CREATECHARACTER_SENT;
        std::cerr << "createchar" << std::endl;
        break;

    case TEST_CREATECHARACTER_SENT:
        std::cerr << ">";                
        break;
    case TEST_CREATECHARACTER_RECEIVED:
        std::cerr << "createchar ok." << std::endl;
        
        send_listCharacter();
        state = TEST_LISTCHARACTER_SENT;
        std::cerr << "listchar." << std::endl;
        break;
        
    case TEST_LISTCHARACTER_SENT:
        std::cerr << ">";                
        break;
    case TEST_LISTCHARACTER_RECEIVED:
        std::cerr << "listchar ok." << std::endl;
        send_login( "ringo" );        
        state = TEST_LOGIN_SENT;
        std::cerr << "login." << std::endl;        
        break;

    case TEST_LOGIN_SENT:
        std::cerr << ">";
        break;
    case TEST_LOGIN_RECEIVED:
        state = TEST_INGAME;
        break;
    case TEST_INGAME:
        {
            vce::VUint64 nowtimer = vce::GetTime();
            if( nowtimer > (vtimer+500) ){
                vtimer = nowtimer;
                std::cerr << ">";
                ingameSender();
                printStat();
            }
        }
        break;
    case TEST_LOGOUT_SENT:
        std::cerr << ">";
        break;
    case TEST_SESSION_CLOSED:
        break;
    case TEST_FINISHED:
        // do nothing
        std::cerr << ".";
        break;
    default:
        std::cerr << "invalid test state" << std::endl;        
        assert(0);
    }
    return true;    
}


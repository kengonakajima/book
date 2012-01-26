#ifndef _CLIMAIN_H_
#define _CLIMAIN_H_

#include "vce2.h"
#include "kproto.h"
#include "common.h"


class Config
{
 public:
    std::string accountName;
    std::string password;
    std::string characterName;    

    
    Config(){
        accountName = std::string("kengonakajima");
        password = std::string("testpassword");
        characterName = std::string( "ringo" );
    }
};

class TestMovable
{
 public:
    vce::VUint32 id;
    vce::VUint32 typeID;
    vce::VUint32 floorID;    
    Coord coord;

    TestMovable( vce::VUint32 _id, vce::VUint32 _typeID, vce::VUint32 _floorID,  Coord _coord ){
        id = _id;
        typeID = _typeID;
        floorID = _floorID;
        coord = _coord;
    }
    TestMovable(){}
};


class KClient : public k_proto_client
{
 private:
    vce::VUint32 myMovableID;
    Coord myCoord;    

    
    vce::VUint32 tick;
    vce::VUint64 vtimer;

    std::string localAccountName;
    std::string localPassword;

    vce::VUint32 recvCounter[FUNCTIONS_END];
    vce::VUint32 sendCounter[FUNCTIONS_END];
    vce::VUint32 moveCounter[MOVABLE_MAX];

    // ゲーム進行記憶用
    std::map<vce::VUint32,TestMovable*> movmap;
    
public:
    KClient(){
        state = TEST_INIT;
        memset( recvCounter, 0, sizeof(recvCounter));
        memset( moveCounter, 0, sizeof(moveCounter));    
    }

 
    // テストの状態遷移
    typedef enum {
        TEST_INIT = 0,                     // 接続直後の初期状態
        TEST_PING_SENT,                    // ping()を送信した
        TEST_PING_RECEIVED,                // ping()の返しを受信した
        TEST_SIGNUP_SENT,                  // signup()を送信した
        TEST_SIGNUP_RECEIVED,              // signupResult()を受信した
        TEST_AUTHENTICATION_SENT,          // authentication()を送信した
        TEST_AUTHENTICATION_RECEIVED,      // authenticationResult()を受信した
        TEST_CREATECHARACTER_SENT,         // createCharacter()を送信した
        TEST_CREATECHARACTER_RECEIVED,     // createCharacterResult()を受信した
        TEST_LISTCHARACTER_SENT,           // listCharacter()を送信した
        TEST_LISTCHARACTER_RECEIVED,       // listCharacterResult()を受信した
        TEST_LOGIN_SENT,                   // login()を送信した
        TEST_LOGIN_RECEIVED,               // loginResult()を受信した
        TEST_INGAME,                       // ゲームをプレイしている状態
        TEST_LOGOUT_SENT,                  // logout()を送信した
        TEST_SESSION_CLOSED,               // logoutResult()を受信した
        TEST_FINISHED                      // テスト完了、いつでも終わって良い状態
    } TestState;

    
    TestState state;

    bool evaluate();
    void printStat();

	void recv_ping(vce::VUint64 t );
    void recv_signupResult(ResultCode result);
	void recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt);
    
	void recv_createCharacterResult(ResultCode result);
	void recv_authenticationResult(ResultCode result);
	void recv_loginResult(ResultCode result,vce::VUint32 movableID);
    
	void recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text);
	void recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 f );
	void recv_disappearNotify(vce::VSint32 movableID);  
	void recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat);    
    
	void recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID);  
	void recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage );  
	void recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)  ;
	void recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt);
    
	void recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *tileType,vce::VUint32 tileType_qt);
	void recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType);
    void recv_shopNotify(vce::VSint32 shopMovableID);  
	void recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price);  
	void recv_buyResult(vce::VSint32 resultCode);  
	void recv_sellResult(vce::VSint32 resultCode);  
	void recv_viewmodeResult(ResultCode result){}

    
	void Closed(vce::VCE_CLOSEREASON reason);

    
    bool Poll();
    void ingameSender();
    
};







#endif

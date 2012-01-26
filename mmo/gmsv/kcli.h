#ifndef _KCLI_H_
#define _KCLI_H_

#include "common.h"

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
    KClient();
    
    // テストの状態遷移
    typedef enum {
        TEST_INIT = 0, 
        TEST_PING_SENT, 
        TEST_PING_RECEIVED,
        TEST_SIGNUP_SENT, 
        TEST_SIGNUP_RECEIVED, 
        TEST_AUTHENTICATION_SENT,
        TEST_AUTHENTICATION_RECEIVED,
        TEST_CREATECHARACTER_SENT,
        TEST_CREATECHARACTER_RECEIVED,                
        TEST_LISTCHARACTER_SENT, //1char  listが先。
        TEST_LISTCHARACTER_RECEIVED,
        TEST_LOGIN_SENT, 
        TEST_LOGIN_RECEIVED, 
        TEST_INGAME, 
        TEST_LOGOUT_SENT, 
        TEST_SESSION_CLOSED, 
        TEST_FINISHED 
    } TestState;

    
    TestState state;

    bool evaluate();
    void printStat();

	void recv_ping(const char *data);
	void recv_signupResult(ResultCode result);
	void recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt);
    
	void recv_createCharacterResult(ResultCode result);
	void recv_authenticationResult(ResultCode result);
	void recv_loginResult(ResultCode result,vce::VUint32 movableID);
    
	void recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text){}
	void recv_moveNotify(vce::VSint32 movableID,vce::VSint32 imageTypeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 z, k_proto::Direction dir);
	void recv_disappearNotify(vce::VSint32 movableID);  
	void recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat);    
    
	void recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID){}  
	void recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage );  
	void recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType){}  
	void recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt);
    
	void recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const vce::VSint32 *tileType,vce::VUint32 tileType_qt);
	void recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType){}
    void recv_shopNotify(vce::VSint32 shopMovableID){}  
	void recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price){}  
	void recv_buyResult(vce::VSint32 resultCode){}  
	void recv_sellResult(vce::VSint32 resultCode){}  

    
	void Closed(vce::VCE_CLOSEREASON reason);

    
    bool Poll();
    void ingameSender();
    
};



#endif

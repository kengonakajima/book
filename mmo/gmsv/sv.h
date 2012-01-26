#ifndef _SV_H_
#define _SV_H_



#include "vce2.h"
#include "kproto.h"
#include "dbproto.h"
#include <iostream>
#include <assert.h>

#include "common.h"

#define MAX_CLIENT 1000  

class PlayerCharacter;

class KServer : public k_proto_server
{

 private:
    int m_lastFunction; // genで呼ばれた最後の関数

    // DBに保存されているもののコピー. DBから何か受信するごとに更新する。
    db_proto::Player m_DBPlayer;
    std::vector<db_proto::PlayerCharacter> m_DBPlayerCharacter;
    std::vector<db_proto::CharacterItem> m_DBCharacterItem;
    std::vector<db_proto::CharacterSkill> m_DBCharacterSkill;

    
    // 各プロトコルの成功を記録していく (idiom)
    bool m_authenticationSuccess;
    bool m_loginSuccess;

    // viewモード関連
    bool m_viewStarted; // viewモードなら1
    vce::VUint32 m_viewFloorID;
    Coord m_viewCoord;

    PlayerCharacter *m_pc;
    std::string m_usingPCName; // 複数あるキャラのうちどれを使ってるか

    vce::VUint64 m_lastMoveRecvAt; // 移動要求の頻度を厳密に制限 (idiom)
    
 public:
    KServer(){
        m_lastFunction = FUNCTIONS_END;
        m_authenticationSuccess = false;
        m_loginSuccess = false;
        m_pc = NULL;
        m_lastMoveRecvAt = 0;
        m_viewStarted = false;
    };

    // accessor
    PlayerCharacter *getPC(){ return m_pc; }
    db_proto::PlayerCharacter* getDBPC(){
        std::vector<db_proto::PlayerCharacter>::iterator it;
        for(it = m_DBPlayerCharacter.begin();
            it != m_DBPlayerCharacter.end();
            ++it ){
            if( strcmp( (*it).name, m_usingPCName.c_str())==0 ){
                return & (*it);
            }
        }
        assert(!"never go here");
        return NULL;
    }
    bool isViewmode( Coord *co, vce::VUint32 *flid){
        if(co)*co = m_viewCoord;
        if(flid)*flid = m_viewFloorID;
        return m_viewStarted;
    }

	// vce funcs
	void Attached(){};
	void Connected(){};
	void Closed(vce::VCE_CLOSEREASON type);
	void Detached(){};
	void Exception( vce::VCE_EXCEPTION type ){};
	size_t Recv(const vce::VUint8 *p,size_t sz){
        return k_proto_server::Recv(p,sz);

    };

	void recv_ping(vce::VUint64 t);

    void recv_signup(const char *accountname,const char *password);
	void recv_listCharacter();
	void recv_createCharacter(const char *characterName);
	void recv_authentication(const char *accountname,const char *password);
	void recv_login(const char *characterName);
	void recv_chat(const char *text){};  
	void recv_move(vce::VSint32 toX,vce::VSint32 toY);  
	void recv_characterStatus(vce::VSint32 movableID);  
	void recv_attack(vce::VSint32 movableID);
	void recv_item();
	void recv_equip(vce::VSint32 slotID);  
    void recv_landscape( vce::VSint32 floorID, vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2);
    void recv_viewmode(vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y, vce::VUint8 mode );
    
	void recv_useItem(vce::VSint32 slotID){};  
	void recv_buy(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID){};  
	void recv_sell(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID){};
	void recv_logout();

	void recv_useSkill(vce::VSint32 skillID){};  

    // DB receiver
    void db_recv_put_Player_result( db_proto::ResultCode result,db_proto::Player data);
    void db_recv_put_PlayerCharacter_result( db_proto::ResultCode result, db_proto::PlayerCharacter data );
    
    void db_recv_get_Player_by_accountName_result( db_proto::ResultCode result, const db_proto::Player *data, vce::VUint32 data_qt );
    void db_recv_get_PlayerCharacter_by_playerID_result( db_proto::ResultCode result, const db_proto::PlayerCharacter *data, vce::VUint32 data_qt );

    void db_recv_get_CharacterItem_by_characterID_result( db_proto::ResultCode result, const db_proto::CharacterItem *data, vce::VUint32 data_qt);    
    void db_recv_get_CharacterSkill_by_characterID_result( db_proto::ResultCode result, const db_proto::CharacterSkill *data,vce::VUint32 data_qt);

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

KServer *findKServer( vce::VUint32 sessionID );

#endif

#ifndef _GMSVMAIN_H_
#define _GMSVMAIN_H_
#include <iostream>


#include "id.h"
#include "kproto.h"
#include "dbproto.h"

class DBClient : public db_proto_client
{
    // network flags
    bool idQuerying;

 public:
    DBClient(){
        idQuerying = false; // 複数の新規IDをまとめて発行させるidiom
    };

    // work funcs
    void setIDQuerying( bool flag ){  idQuerying = flag; }
    bool getIDQuerying(){ return idQuerying; }
    
    // vce session funcs
    void Connected(){};
    void Closed(){
        std::cerr << "dbclient connection closed.. quit." << std::endl;
        assert(0);
    };

    // protocol funcs
    void recv_ping( vce::VUint64 tm ){};
    void recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt );



	void recv_put_Player_result(vce::VUint32 sessionID,ResultCode result,Player data);  
	void recv_get_Player_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt){}  
	void recv_get_Player_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt){}  
	void recv_get_Player_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt);  
	void recv_get_Player_ranking_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt){}  
	void recv_put_PlayerCharacter_result(vce::VUint32 sessionID,ResultCode result,PlayerCharacter data);  
	void recv_get_PlayerCharacter_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);  
	void recv_get_PlayerCharacter_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_ranking_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt){}  
	void recv_get_PlayerCharacter_ranking_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);  
	void recv_put_CharacterItem_result(vce::VUint32 sessionID,ResultCode result,CharacterItem data){}  
	void recv_get_CharacterItem_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt){}  
	void recv_get_CharacterItem_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt){}  
	void recv_get_CharacterItem_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt);  
	void recv_get_CharacterItem_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt){}  
	void recv_put_CharacterSkill_result(vce::VUint32 sessionID,ResultCode result,CharacterSkill data){}  
	void recv_get_CharacterSkill_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt){}  
	void recv_get_CharacterSkill_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt){}  
	void recv_get_CharacterSkill_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt);  
	void recv_get_CharacterSkill_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt){}  
	void recv_put_PlayerLock_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data){}
	void recv_get_PlayerLock_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt){}
	void recv_get_PlayerLock_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt){}
	void recv_put_PlayerLock_if_state_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data){}
    

    
};



k_proto::CharacterStatus copyDBPlayerCharacterToKCharacterStatus( db_proto::PlayerCharacter pc );

vce::VUint32 getNewID();

extern DBClient *g_dbcli;
extern IDPool *g_idpool;

#endif

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
    // table Player
    void recv_put_Player( vce::VUint32 sessionID, Player data );
    // simple getter funcs
    void recv_get_Player_by_id( vce::VUint32 sessionID, vce::VUint64 id );
    void recv_get_Player_ranking_by_id( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_Player_by_accountName( vce::VUint32 sessionID, const char * accountName );
    void recv_get_Player_ranking_by_accountName( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    // table PlayerCharacter
    void recv_put_PlayerCharacter( vce::VUint32 sessionID, PlayerCharacter data );
    // simple getter funcs
    void recv_get_PlayerCharacter_by_id( vce::VUint32 sessionID, vce::VUint64 id );
    void recv_get_PlayerCharacter_ranking_by_id( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_PlayerCharacter_by_playerID( vce::VUint32 sessionID, vce::VUint64 playerID );
    void recv_get_PlayerCharacter_ranking_by_playerID( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_PlayerCharacter_by_name( vce::VUint32 sessionID, const char * name );
    void recv_get_PlayerCharacter_ranking_by_name( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_PlayerCharacter_by_level( vce::VUint32 sessionID, vce::VUint16 level );
    void recv_get_PlayerCharacter_ranking_by_level( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    // table CharacterItem
    void recv_put_CharacterItem( vce::VUint32 sessionID, CharacterItem data );
    // simple getter funcs
    void recv_get_CharacterItem_by_id( vce::VUint32 sessionID, vce::VUint64 id );
    void recv_get_CharacterItem_ranking_by_id( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_CharacterItem_by_characterID( vce::VUint32 sessionID, vce::VUint64 characterID );
    void recv_get_CharacterItem_ranking_by_characterID( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    // table CharacterSkill
    void recv_put_CharacterSkill( vce::VUint32 sessionID, CharacterSkill data );
    // simple getter funcs
    void recv_get_CharacterSkill_by_id( vce::VUint32 sessionID, vce::VUint64 id );
    void recv_get_CharacterSkill_ranking_by_id( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_get_CharacterSkill_by_characterID( vce::VUint32 sessionID, vce::VUint64 characterID );
    void recv_get_CharacterSkill_ranking_by_characterID( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    // table PlayerLock
    void recv_put_PlayerLock( vce::VUint32 sessionID, PlayerLock data );
    // simple getter funcs
    void recv_get_PlayerLock_by_playerID( vce::VUint32 sessionID, vce::VUint64 playerID );
    void recv_get_PlayerLock_ranking_by_playerID( vce::VUint32 sessionID, SortType sorttype, vce::VUint32 limit, vce::VUint32 offset );
    void recv_put_PlayerLock_if_state( vce::VUint32 sessionID, PlayerLock data, vce::VUint8 state_test );
};
#endif

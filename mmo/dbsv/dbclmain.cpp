/*
  generated code
 */
#include "vce2.h"
#include "dbproto.h"
#include "dbclmain.h"
#include "util.h"
#include <map>
void random_str( char *out, size_t len )
{
    for(int i=0;i<len;i++){
        out[i] = 'A' + ( random() % 25 );
    }
    out[len-1]='\0';
}
int main( int argc, char **argv )
{
    vce::VCE *vceobj;    
    assert(vce::VCEInitialize());
    vceobj = vce::VCECreate();
    vceobj->ReUseAddress(true);
    vceobj->SetPollingWait(true);
    vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);
    // 通常のクライアントのテスト用
    DBClient *dbcli = new DBClient();
    if( !vceobj->Connect( dbcli, "localhost", 10010 )){
        std::cerr << "fatal error: cannot connect to dbsv" << std::endl;
        return 1;
    }
    dbcli->log=&std::cerr;
    while(true){
        if( dbcli->GetState() == vce::VCE_STATE_ESTABLISHED
            && dbcli->getLatency() < 3000 ){
            // 構造体を作る
            db_proto::Player tmp_Player;
            tmp_Player.id = (vce::VUint64)(random()%100);
            random_str( tmp_Player.accountName,  50 );
            random_str( tmp_Player.passwordHash,  50 );
            // クエリ送信
            dbcli->send_put_Player( dbcli->uID, tmp_Player );
            dbcli->send_get_Player_by_id( dbcli->uID, tmp_Player.id );
            dbcli->send_get_Player_ranking_by_id( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_Player_by_accountName( dbcli->uID, tmp_Player.accountName );
            dbcli->send_get_Player_ranking_by_accountName( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            // 構造体を作る
            db_proto::PlayerCharacter tmp_PlayerCharacter;
            tmp_PlayerCharacter.id = (vce::VUint64)(random()%100);
            tmp_PlayerCharacter.playerID = (vce::VUint64)(random()%100);
            random_str( tmp_PlayerCharacter.name,  50 );
            tmp_PlayerCharacter.level = (vce::VUint16)(random()%100);
            tmp_PlayerCharacter.exp = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.hp = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.maxhp = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.floorID = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.x = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.y = (vce::VUint32)(random()%100);
            tmp_PlayerCharacter.equippedItemTypeID = (vce::VUint32)(random()%100);
            // クエリ送信
            dbcli->send_put_PlayerCharacter( dbcli->uID, tmp_PlayerCharacter );
            dbcli->send_get_PlayerCharacter_by_id( dbcli->uID, tmp_PlayerCharacter.id );
            dbcli->send_get_PlayerCharacter_ranking_by_id( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_PlayerCharacter_by_playerID( dbcli->uID, tmp_PlayerCharacter.playerID );
            dbcli->send_get_PlayerCharacter_ranking_by_playerID( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_PlayerCharacter_by_name( dbcli->uID, tmp_PlayerCharacter.name );
            dbcli->send_get_PlayerCharacter_ranking_by_name( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_PlayerCharacter_by_level( dbcli->uID, tmp_PlayerCharacter.level );
            dbcli->send_get_PlayerCharacter_ranking_by_level( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            // 構造体を作る
            db_proto::CharacterItem tmp_CharacterItem;
            tmp_CharacterItem.id = (vce::VUint64)(random()%100);
            tmp_CharacterItem.characterID = (vce::VUint64)(random()%100);
            tmp_CharacterItem.typeID = (vce::VUint32)(random()%100);
            tmp_CharacterItem.num = (vce::VUint32)(random()%100);
            // クエリ送信
            dbcli->send_put_CharacterItem( dbcli->uID, tmp_CharacterItem );
            dbcli->send_get_CharacterItem_by_id( dbcli->uID, tmp_CharacterItem.id );
            dbcli->send_get_CharacterItem_ranking_by_id( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_CharacterItem_by_characterID( dbcli->uID, tmp_CharacterItem.characterID );
            dbcli->send_get_CharacterItem_ranking_by_characterID( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            // 構造体を作る
            db_proto::CharacterSkill tmp_CharacterSkill;
            tmp_CharacterSkill.id = (vce::VUint64)(random()%100);
            tmp_CharacterSkill.characterID = (vce::VUint64)(random()%100);
            tmp_CharacterSkill.typeID = (vce::VUint32)(random()%100);
            tmp_CharacterSkill.level = (vce::VUint32)(random()%100);
            // クエリ送信
            dbcli->send_put_CharacterSkill( dbcli->uID, tmp_CharacterSkill );
            dbcli->send_get_CharacterSkill_by_id( dbcli->uID, tmp_CharacterSkill.id );
            dbcli->send_get_CharacterSkill_ranking_by_id( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            dbcli->send_get_CharacterSkill_by_characterID( dbcli->uID, tmp_CharacterSkill.characterID );
            dbcli->send_get_CharacterSkill_ranking_by_characterID( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            // 構造体を作る
            db_proto::PlayerLock tmp_PlayerLock;
            tmp_PlayerLock.playerID = (vce::VUint64)(random()%100);
            tmp_PlayerLock.state = (vce::VUint8)(random()%100);
            tmp_PlayerLock.ownerServerID = (vce::VUint32)(random()%100);
            // クエリ送信
            dbcli->send_put_PlayerLock( dbcli->uID, tmp_PlayerLock );
            dbcli->send_get_PlayerLock_by_playerID( dbcli->uID, tmp_PlayerLock.playerID );
            dbcli->send_get_PlayerLock_ranking_by_playerID( dbcli->uID, db_proto::SORT_BIGGERFIRST, 10, 10 );
            // set_if関連
            {
                db_proto::PlayerLock tmp_if_PlayerLock = tmp_PlayerLock;
                tmp_PlayerLock.state = 0;
                tmp_if_PlayerLock.state = 1;
                dbcli->send_put_PlayerLock( dbcli->uID, tmp_PlayerLock);                
                dbcli->send_put_PlayerLock_if_state( dbcli->uID, tmp_if_PlayerLock, 0);
                tmp_if_PlayerLock.state = 1;                
                dbcli->send_put_PlayerLock_if_state( dbcli->uID, tmp_if_PlayerLock, 1);
            }
            dbcli->send_getNewID( db_proto::MAXIDSET );
            dbcli->send_ping( vce::GetTime());
            dbcli->lastPingSentAt = vce::GetTime();
        }
        vceobj->Poll();        
        std::cerr << ".";        
    }
    return 0;
}
void DBClient::recv_put_Player_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, Player data )
{
    std::cerr << "recv: Player " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
void DBClient::recv_get_Player_by_id_result( vce::VUint32 sessionID, ResultCode result, const Player *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_Player_ranking_by_id_result( vce::VUint32 sessionID, ResultCode result, const Player *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_Player_by_accountName_result( vce::VUint32 sessionID, ResultCode result, const Player *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: accountName " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_Player_ranking_by_accountName_result( vce::VUint32 sessionID, ResultCode result, const Player *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: accountName r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_put_PlayerCharacter_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, PlayerCharacter data )
{
    std::cerr << "recv: PlayerCharacter " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
void DBClient::recv_get_PlayerCharacter_by_id_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_PlayerCharacter_ranking_by_id_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_PlayerCharacter_by_playerID_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: playerID " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_PlayerCharacter_ranking_by_playerID_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: playerID r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_PlayerCharacter_by_name_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: name " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_PlayerCharacter_ranking_by_name_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: name r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_PlayerCharacter_by_level_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: level " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_PlayerCharacter_ranking_by_level_result( vce::VUint32 sessionID, ResultCode result, const PlayerCharacter *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: level r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_put_CharacterItem_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, CharacterItem data )
{
    std::cerr << "recv: CharacterItem " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
void DBClient::recv_get_CharacterItem_by_id_result( vce::VUint32 sessionID, ResultCode result, const CharacterItem *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_CharacterItem_ranking_by_id_result( vce::VUint32 sessionID, ResultCode result, const CharacterItem *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_CharacterItem_by_characterID_result( vce::VUint32 sessionID, ResultCode result, const CharacterItem *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: characterID " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_CharacterItem_ranking_by_characterID_result( vce::VUint32 sessionID, ResultCode result, const CharacterItem *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: characterID r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_put_CharacterSkill_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, CharacterSkill data )
{
    std::cerr << "recv: CharacterSkill " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
void DBClient::recv_get_CharacterSkill_by_id_result( vce::VUint32 sessionID, ResultCode result, const CharacterSkill *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_CharacterSkill_ranking_by_id_result( vce::VUint32 sessionID, ResultCode result, const CharacterSkill *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: id r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_get_CharacterSkill_by_characterID_result( vce::VUint32 sessionID, ResultCode result, const CharacterSkill *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: characterID " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_CharacterSkill_ranking_by_characterID_result( vce::VUint32 sessionID, ResultCode result, const CharacterSkill *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: characterID r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_put_PlayerLock_result( vce::VUint32 sessionID, db_proto_client::ResultCode result, PlayerLock data )
{
    std::cerr << "recv: PlayerLock " << std::endl;    
    assert( result == db_proto::SUCCESS || result == db_proto::DUPENTRY );
    std::cerr << "sid:" << sessionID << " uid:" << this->uID << std::endl;
    assert( sessionID == this->uID );
}
void DBClient::recv_get_PlayerLock_by_playerID_result( vce::VUint32 sessionID, ResultCode result, const PlayerLock *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: playerID " << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );    
}
void DBClient::recv_get_PlayerLock_ranking_by_playerID_result( vce::VUint32 sessionID, ResultCode result, const PlayerLock *data, vce::VUint32 data_qt )
{
    std::cerr << "recv: playerID r:" << result << std::endl;
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );        
}
void DBClient::recv_put_PlayerLock_if_state_result( vce::VUint32 sessionID, ResultCode result, const PlayerLock data )
{
    assert( result == db_proto::SUCCESS );
    assert( sessionID == this->uID );
    assert( data.state == 1 );
}
void DBClient::recv_ping( vce::VUint64 timestamp )
{
    lastPingRecvFromServer = timestamp;
    std::cerr << "received ping from dbsv:" << timestamp << std::endl;
}
void DBClient::recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
    std::cerr << "recv_getNewIDResult: get " << idarray_qt << " ids" << std::endl;
    for(int i=0;i<idarray_qt;i++){
        std::cerr << idarray[i] << ":";
    }
    std::cerr << std::endl;        
}

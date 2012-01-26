
#include "vce2.h"

#include "kproto.h"
#include "dbproto.h" 


#include "gmsvmain.h"
#include "sv.h"
#include "common.h"
#include "floor.h"
#include "movable.h"
#include "zone.h"

#include <iostream>


// すべてのセッションから使う1個だけ必要なもの
vce::VCE *g_vceobj;
DBClient *g_dbcli;
IDPool *g_idpool;

void DBClient::recv_get_PlayerCharacter_by_playerID_result(vce::VUint32 sessionID,  ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_get_PlayerCharacter_by_playerID_result(result,data, data_qt );
}
k_proto::CharacterStatus copyDBPlayerCharacterToKCharacterStatus( db_proto::PlayerCharacter pc )
{
    k_proto::CharacterStatus cs( pc.name,
                                 pc.hp,
                                 pc.maxhp,
                                 pc.level,
                                 pc.exp,
                                 pc.floorID,
                                 pc.x,
                                 pc.y );
}



void DBClient::recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
    std::cerr << "recv_getNewIDResult: got " << idarray_qt << std::endl;
    for(int i=0;i<idarray_qt; i++){
        g_idpool->add( idarray[i]);
        std:: cerr << idarray[i] << " ";
    }
    g_dbcli->setIDQuerying(false);
}
    
void DBClient::recv_get_PlayerCharacter_ranking_by_level_result(vce::VUint32 sessionID,  ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
    std::cerr << "recv_get_PlayerCharacter_ranking_by_level_result." << std::endl;
    for(int i=0;i<data_qt;i++){
        std::cerr << "ID:" << data[i].id << " Level:" << data[i].level << std::endl;
    }
}

void DBClient::recv_put_Player_result(vce::VUint32 sessionID,  ResultCode result,Player data)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_put_Player_result( result, data );
}
void KServer::db_recv_put_Player_result( db_proto::ResultCode result, db_proto::Player data)
{
    if( m_lastFunction == FUNCTION_SIGNUP ){
        if( result == db_proto::SUCCESS ){
            send_signupResult( k_proto::SUCCESS );
        } else {
            send_signupResult( k_proto::FAIL );
        }
    } else {
        assert(0);
    }
}
void DBClient::recv_put_PlayerCharacter_result(vce::VUint32 sessionID,  ResultCode result,PlayerCharacter data)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_put_PlayerCharacter_result( result, data );
}
void KServer::db_recv_put_PlayerCharacter_result( db_proto::ResultCode result, db_proto::PlayerCharacter data )
{
    if( m_lastFunction == FUNCTION_CREATECHARACTER ){
        if( result == db_proto::SUCCESS ){
            send_createCharacterResult( k_proto::SUCCESS );
        } else {
            send_createCharacterResult( k_proto::FAIL );
        }
    } 
}



void DBClient::recv_get_Player_by_accountName_result(vce::VUint32 sessionID,  ResultCode result,const Player *data,vce::VUint32 data_qt)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_get_Player_by_accountName_result( result, data, data_qt );
}
void KServer::db_recv_get_Player_by_accountName_result( db_proto::ResultCode result, const db_proto::Player *data, vce::VUint32 data_qt )
{
    if( m_lastFunction == FUNCTION_AUTHENTICATION ){
        if( result != db_proto::SUCCESS || data_qt != 1 ){
            
            send_authenticationResult( FAIL );
        } else {
            m_DBPlayer = data[0];
            send_authenticationResult( SUCCESS );
            m_authenticationSuccess = true;
        }
    } else {
        assert(0);
    }
}

void DBClient::recv_get_CharacterItem_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_get_CharacterItem_by_characterID_result( result, data, data_qt );        
}
void KServer::db_recv_get_CharacterItem_by_characterID_result( db_proto::ResultCode result, const db_proto::CharacterItem *data, vce::VUint32 data_qt)
{
    if( data_qt > 0 ){
        m_DBCharacterItem.resize(data_qt);
        for(int i=0;i<data_qt;i++){
            m_DBCharacterItem[i] = data[i];
            if( m_pc ){
                m_pc->setItem( i, Item( data[i].id, static_cast<k_proto::ItemType>(data[i].typeID), data[i].num));
            }
        }
    }
}

void DBClient::recv_get_CharacterSkill_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)
{
    KServer *ks = findKServer( sessionID );
    if(ks) ks->db_recv_get_CharacterSkill_by_characterID_result( result, data, data_qt );
}
void KServer::db_recv_get_CharacterSkill_by_characterID_result( db_proto::ResultCode result, const db_proto::CharacterSkill *data,vce::VUint32 data_qt)
{
    if( data_qt > 0 ){
        m_DBCharacterSkill.resize(data_qt);
        for(int i=0;i<data_qt;i++){
            m_DBCharacterSkill[i] = data[i];
            if( m_pc ){
                m_pc->setSkill( i, Skill( data[i].id, static_cast<k_proto::SkillType>(data[i].typeID), data[i].level ));
            }
        }
    }
}
void KServer::db_recv_get_PlayerCharacter_by_playerID_result( db_proto::ResultCode result, const db_proto::PlayerCharacter *data, vce::VUint32 data_qt )
{
    if( m_lastFunction == FUNCTION_LISTCHARACTER ){
        k_proto::CharacterStatus ps[100];
        if( result == db_proto::SUCCESS ){
            int loopcnt = minvalue( ARRAYLEN(ps), data_qt );
            m_DBPlayerCharacter.resize(loopcnt);
            for(int i=0;i<loopcnt;i++){
                ps[i] = copyDBPlayerCharacterToKCharacterStatus( data[i] );
                m_DBPlayerCharacter[i] = data[i];
            }
            send_listCharacterResult( SUCCESS, ps, loopcnt );
        } else {
            send_listCharacterResult( FAIL, ps, 0 );
        }
    } else {
        assert(0);
    }
    
}




void keepIDPool()
{
    if( g_idpool->getLeft() < 1000
        && g_dbcli->GetState() == vce::VCE_STATE_ESTABLISHED
        && g_dbcli->getIDQuerying() == false ){
        g_dbcli->send_getNewID( 100 );
        g_dbcli->setIDQuerying(true);

        std::cerr << "########## " << g_idpool->getLeft() << std::endl;
    }
}

// DEBUG: testing
bool dbTested = false;
const int TESTSESSIONID = 0;
void dbTest()
{
    if( g_dbcli->GetState() == vce::VCE_STATE_ESTABLISHED
        && g_idpool->getLeft() > 100
        && dbTested == false ){
                
        g_dbcli->send_ping( vce::GetTime() );

        vce::VUint64 playerID = g_idpool->get();
                
        db_proto::Player pdata( playerID, "ringoaccount", "testpassword");

        g_dbcli->send_put_Player( TESTSESSIONID,  pdata );
        g_dbcli->send_get_Player_by_id( TESTSESSIONID,   playerID );

        vce::VUint64 characterID = g_idpool->get();
        db_proto::PlayerCharacter pcdata( characterID, // id
                                          playerID, //playerID
                                          "ringo",
                                          1 + (random()%10),
                                          100 + (random()%1000),
                                          10 + (random()%100),
                                          20 + (random()%100),
                                          1 + ( random()%100),
                                          512,
                                          512,
                                          k_proto::ITEM_NONE );
        g_dbcli->send_put_PlayerCharacter( TESTSESSIONID,  pcdata );
        g_dbcli->send_get_PlayerCharacter_by_playerID( TESTSESSIONID,  playerID );

        vce::VUint64 skillID = g_idpool->get();

        db_proto::CharacterSkill csdata( skillID, characterID, k_proto::SKILL_CUTTREE, 1 );
        g_dbcli->send_put_CharacterSkill( TESTSESSIONID,  csdata );
        g_dbcli->send_get_CharacterSkill_by_characterID( TESTSESSIONID,  characterID );

        g_dbcli->send_get_PlayerCharacter_ranking_by_level( TESTSESSIONID,  db_proto::SORT_BIGGERFIRST, 10, 10 );
        std::cerr << "send dbsv test" << std::endl;
        dbTested = true;
    }
}

// 起動時引数:
int main( int argc, char **argv )
{
    if( argc != 2 ){
        std::cerr << "usage: ./gmsv SERVERID" << std::endl;
        std::cerr << "ex: ./gmsv 0" << std::endl;
        return 1;
    }
    Zone::setServerID( atoi( argv[1]) );
    
    // ゲーム世界を初期化
    World::initLandscape();    
    
    // VCEミドルウェアを初期化する
    vce::VCEInitialize();
    g_vceobj = vce::VCECreate();
    g_vceobj->ReUseAddress(true);
	g_vceobj->SetPollingWait(true);
    g_vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);

    // バックエンドへの接続を初期化する
    // 最初に1回だけ接続し、途中で接続が切れたらプロセスを強制終了する。
    // サーバの起動順は、まずバックエンドでgmsvは最後に起動する。
    g_dbcli = new DBClient();
    if( !g_vceobj->Connect( g_dbcli, "localhost", 10010, 1000 )){
        std::cerr << "fatal error: cannot connect to dbsv" << std::endl;
        return 1;
    }
    g_dbcli->log=&std::cerr;
    

    // cliとの通信用にフロントエンドのリスナーを初期化する
    g_listener = new GameListener();
    ZoneServerAddress *zadr = Zone::getServerAddress( Zone::serverID );
    g_vceobj->Listen( g_listener, zadr->portNumber );

    // etc.
    g_idpool = new IDPool(10000);
    assert(g_idpool);
    
    // ループ開始
    time_t everysec = 0;
    int loopc=0;
    while(true){
        keepIDPool();
        if( everysec != time(NULL)){
            std::cerr << ".";
            dbTest();
            everysec = time(NULL);

            if( (time(NULL)%10)==0){
                std::cerr << "Stat: movable:" << World::countMovable() << std::endl;
            }
        }

        g_vceobj->Poll();
        World::poll();
        
    }

    return 0;
    
}


//
// KServerの recv_関数群の定義。 クライアントからの一次窓口をここに集めている。
//
//

#include "vce2.h"
#include "util.h"
#include "sv.h"
#include "gmsvmain.h"
#include "floor.h"
#include "movable.h"
#include "zone.h"

GameListener *g_listener;


// KServerのsession ID を指定して検索する。
KServer *findKServer( vce::VUint32 sessionID )
{
    KServer *sessions[MAX_CLIENT];
    int n = g_listener->GetChildren((vce::Session**)sessions, ARRAYLEN(sessions));
    for(int i=0;i<n;i++){
        std::cerr << "sid:" << sessions[i]->uID << std::endl;            
        if( sessions[i]->uID == sessionID ){
            return sessions[i];
        }
    }
    std::cerr << "session not found:" << sessionID << std::endl;    
    return NULL;    
}



void GameListener::Exception( vce::VCE_EXCEPTION type ){
    std::cerr << "exception !!!!!!!!!!" << vce::ntoa( type ) << std::endl;
    switch( type ){
    case vce::VCE_EXCEPT_BINDFAIL:
        break;
    default:
        break;
    }        
};


vce::Session * GameListener::Accepter( vce::VUint32 remote_addr, vce::VUint16 remote_port ){
    std::cerr << "GameListener: accepted new connection. addr:" << remote_port << std::endl;
    return new KServer();
};




void KServer::recv_ping(vce::VUint64 timestamp){
    send_ping(timestamp);
};
void KServer::recv_signup(const char *accountname,const char *password)
{
    std::string pw = makeHashString( std::string(password) );
    db_proto::Player pdata( g_idpool->get(), accountname, pw.c_str());
    g_dbcli->send_put_Player( uID, pdata );

    m_lastFunction = FUNCTION_SIGNUP;
}

void KServer::recv_authentication(const char *accountname,const char *password)
{
    g_dbcli->send_get_Player_by_accountName( uID, accountname );
    m_lastFunction = FUNCTION_AUTHENTICATION;
}

void KServer::recv_listCharacter()
{
    if( !m_authenticationSuccess )return;
    
    g_dbcli->send_get_PlayerCharacter_by_playerID( uID,  m_DBPlayer.id );
    m_lastFunction = FUNCTION_LISTCHARACTER;
}


void KServer::recv_createCharacter(const char *characterName)
{
    if( !m_authenticationSuccess )return;
    
    Location lc = World::getStartLocation();
    
    db_proto::PlayerCharacter pc( g_idpool->get(),
                                  m_DBPlayer.id,
                                  characterName,
                                  1, // level
                                  0, //exp
                                  10, //hp
                                  10, //maxhp,
                                  lc.floor->id, // floorid
                                  lc.coord.x, //x
                                  lc.coord.y, ///y
                                  k_proto::ITEM_NONE
                                 );
    g_dbcli->send_put_PlayerCharacter( uID,  pc );
    //新規アイテムも投入。put_PCが成功しなくてもとりあえず投げる。
    db_proto::CharacterItem itm0( g_idpool->get(),
                                  pc.id,
                                  k_proto::ITEM_SHORTSWORD,
                                  2 );
    db_proto::CharacterItem itm1( g_idpool->get(),
                                  pc.id,
                                  k_proto::ITEM_LONGSWORD,
                                  1 );
    db_proto::CharacterSkill sk0( g_idpool->get(),
                                  pc.id,
                                  k_proto::SKILL_CUTTREE,
                                  1 );

    g_dbcli->send_put_CharacterItem( uID,  itm0 );
    g_dbcli->send_put_CharacterItem( uID,  itm1 );
    g_dbcli->send_put_CharacterSkill( uID,  sk0 );
    
    m_lastFunction = FUNCTION_CREATECHARACTER;
}

// かならず、listCharacterが先に呼ばれてロードされていることが前提。
// load失敗していたらNOTLOADエラー返す
void KServer::recv_login( const char *characterName )
{
    if( !m_authenticationSuccess )return;
    
    m_lastFunction = FUNCTION_LOGIN;
    m_usingPCName = std::string( characterName );

    db_proto::PlayerCharacter *dbpc = getDBPC();
    if(!dbpc){
        send_loginResult( FAIL,0);
        std::cerr << "character not found:" << characterName << std::endl;
        return;
    }

    Floor *f = World::getFloor( dbpc->floorID );
    if(!f || Coord( dbpc->x, dbpc->y ).insideFloor() == false ){
        send_loginResult( FAIL,0);
        std::cerr << "invalid location:" << dbpc->floorID << "," << dbpc->x << "," << dbpc->y << std::endl;
        return;
    }

    // このサーバの担当領域か？
    Coord logoutCoord = Coord(dbpc->x,dbpc->y);
    if( !Zone::inThisZone( dbpc->floorID, logoutCoord) ){
        // 領域外でも、１タイル分だけ外れているなら、位置を修正してログインさせる。
        // 厳密に１タイルではなくもうちょっとマージンが広くても良い。
        Coord c0 = logoutCoord.translate(1,0);
        Coord c1 = logoutCoord.translate(0,1);
        Coord c2 = logoutCoord.translate(0,-1);
        Coord c3 = logoutCoord.translate(-1,0);
        
        bool b0 = Zone::inThisZone( dbpc->floorID, c0 );
        bool b1 = Zone::inThisZone( dbpc->floorID, c1 );
        bool b2 = Zone::inThisZone( dbpc->floorID, c2 );
        bool b3 = Zone::inThisZone( dbpc->floorID, c3 );

        if( b0 ){ dbpc->x = c0.x; dbpc->y = c0.y; }
        if( b1 ){ dbpc->x = c1.x; dbpc->y = c1.y; }
        if( b2 ){ dbpc->x = c2.x; dbpc->y = c2.y; }
        if( b3 ){ dbpc->x = c3.x; dbpc->y = c3.y; }        

        bool resolved = b0 || b1 || b2 || b3;
        if( !resolved ){
            send_loginResult( FAIL, 0 );
            std::cerr << "location is not in this zone." << std::endl;
            return;
        }
    }


    // キャラクターを生成して存在するようにする
    m_pc = World::allocPlayerCharacter( World::getFloor(dbpc->floorID),
                                        Coord( dbpc->x, dbpc->y ),
                                        this );
    assert(m_pc);

    m_pc->stat = CharStat( dbpc->hp, dbpc->maxhp, dbpc->level, dbpc->exp );

    // 最終的に結果送信
    send_loginResult( SUCCESS, m_pc->id );
    m_loginSuccess = true;
            
    m_pc->notify( k_proto::FUNCTION_MOVENOTIFY );
            
    // アイテム、スキル、クエストをロードする(遅延して後で読み込まれる)
    g_dbcli->send_get_CharacterItem_by_characterID( uID,  dbpc->id );
    g_dbcli->send_get_CharacterSkill_by_characterID( uID,  dbpc->id );

    // DEBUG: 敵つくる
    if( dbpc->exp == 0 ){
        int cnt =0 ;
        while(true){
            if( World::allocEnemy( m_pc->floor,
                                   m_pc->coord.translate( -5 + ( random() %11 ),
                                                          -5 + ( random() %11 ) ),
                                   MOVABLE_GOBLIN ) ){
                cnt++;
                if(cnt==10)break;
            }
        }
    }
    
    return;
}

void KServer::recv_move(vce::VSint32 toX,vce::VSint32 toY)
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    // パケット遅延に対するマージンを加味(idiom)
    double latency_margin = 0.5;
    if( m_lastMoveRecvAt > vce::GetTime() - (DEFAULT_MOVE_INTERVAL*latency_margin) ){
        return;
    }
    m_lastMoveRecvAt = vce::GetTime();
    
    m_pc->networkMove( toX, toY );

    //    std::cerr << "move: " << m_pc->to_s() << std::endl;
    
    // TODO: テスト用に移動するたびに毎回セーブ
    db_proto::PlayerCharacter *dbpc = getDBPC();
    dbpc->x = m_pc->coord.x;
    dbpc->y = m_pc->coord.y;
    g_dbcli->send_put_PlayerCharacter( uID, *dbpc );

    // popper
    if( ( random() % 3 ) == 0 ){
        World::allocEnemy( m_pc->floor,
                           m_pc->coord.translate( -10 + ( random() %21 ),
                                                  -10 + ( random() %21 ) ),
                           MOVABLE_GOBLIN );
    }
        
}

void KServer::recv_attack(vce::VSint32 movableID)
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    

    Enemy *e = static_cast<Enemy*>(World::findMovable( movableID ));
    if(!e)return;

    if( e->coord.distance( m_pc->coord ) >= 4.0f ){
        return;
    }

    int dmg = 3; 
    e->attacked(m_pc,dmg);

    m_pc->sendStatusTo(this);
}
void KServer::recv_landscape( vce::VSint32 floorID, vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2)
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    Floor *fl = World::getFloor(floorID);
    if(!fl)return;

    TileType data[4000];
    int ind = 0;
    for(int y=y1; y < y2; y++ ){
        for(int x=x1; x < x2; x++){
            data[ind++] = fl->getTile(Coord(x,y)).typeID;
            if( ind == ARRAYLEN(data)) break;
        }
        if( ind == ARRAYLEN(data)) break;        
    }
    send_landscapeResult( floorID, x1,y1,x2,y2, data, ind );
    
}

void KServer::recv_characterStatus(vce::VSint32 movableID)
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    Character *ch = static_cast<Character*>(World::findMovable( movableID ));
    if(!ch)return;

    ch->sendStatusTo(this);
}
void KServer::recv_item()
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    m_pc->sendItemNotifyTo(this);
}

void KServer::recv_equip(vce::VSint32 slotID)
{
    if( !m_authenticationSuccess )return;
    if( !m_pc )return;

    if( m_pc->equip(slotID) ){
        // TODO: 毎回DB保存
        db_proto::PlayerCharacter *dbpc = getDBPC();
        dbpc->equippedItemTypeID = m_pc->equippedItemTypeID;
        g_dbcli->send_put_PlayerCharacter( uID, *dbpc );
    }
    
}

void KServer::recv_viewmode(vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y, vce::VUint8 mode )
{
    std::cerr << "viewmode:" << x << "," <<  y << "," << mode << std::endl;
    Floor *f = World::getFloor(floorID);
    Coord c(x,y);

    if( f && c.insideFloor() ){
        send_viewmodeResult( k_proto::SUCCESS );
        m_viewStarted = (mode != 0);
        m_viewFloorID = floorID;
        m_viewCoord = Coord(x,y);
    }
}

void KServer::recv_logout()
{
    if(m_pc){
        m_pc->saveStat();
        World::deleteMovable(m_pc);        
        m_pc = NULL;
    }
    Close();
}
void KServer::Closed(vce::VCE_CLOSEREASON type)
{
    // logout()を受信していたら、その時点でm_pcはNULLにしているはずだが、
    // logoutを受信する前にネットワークの問題で切断されることもあるので、
    // ここで念のため保存をしておく。　　(idiom)
    if( m_pc ){
        m_pc->saveStat();
        World::deleteMovable(m_pc);
        m_pc = NULL;
    }
}

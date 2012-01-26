#ifndef _APP_H_
#define _APP_H_

#include "sprite.h"
#include "font.h"

#include "kproto.h"
#include "kcli.h"
#include "../gmsv/zone.h"

#define PIXEL_PER_COORD 32  // Coordの1あたり何ピクセルかの対応表

#define PAGE_SIZE 32  // フロアの1ページサイズ(XYとも)

class CliFloor
{
 private:
    

    
 public:
    
    Tile tiles[FLOOR_YSIZE][FLOOR_XSIZE];
    bool loaded[FLOOR_YSIZE/PAGE_SIZE][FLOOR_XSIZE/PAGE_SIZE];

    CliFloor(){
        memset( tiles, 0, sizeof(tiles));
        memset( loaded, 0, sizeof(loaded));
    }

    void setTile( Coord c, k_proto::TileType t ){
        tiles[c.y][c.x] = Tile(t);
    }
    Tile getTile( Coord c ){
        if( !c.insideFloor() )return Tile(k_proto::TILE_ROCK);
        return tiles[c.y][c.x];
    }
    void coordToPageIndex( Coord c, int *px, int *py ){
        *px = c.x / PAGE_SIZE;
        *py = c.y / PAGE_SIZE;
        assert( *px >= 0
                && *py >= 0
                && *px < FLOOR_XSIZE/PAGE_SIZE
                && *py < FLOOR_YSIZE/PAGE_SIZE );
    }
    bool getLoaded( Coord c ){
        int px, py;
        coordToPageIndex( c, &px, &py );
        return loaded[py][px];
    }
    void setLoaded( Coord c, bool f ){
        int px, py;
        coordToPageIndex( c, &px, &py );
        loaded[py][px] = f;
    }
    Rect getPageRect( Coord c ){
        int px, py;
        coordToPageIndex( c, &px, &py );
        Coord base( px * PAGE_SIZE, py * PAGE_SIZE );
        return Rect( base, base.translate( PAGE_SIZE, PAGE_SIZE ));
    }
        
};


class CliMovable
{
 public:
    vce::VUint32 id;
    k_proto::MovableType typeID;
    bool changed;
    bool attacked;
    int lastAttackDamage;
    Coord coord;

    CliMovable( vce::VUint32 _id, k_proto::MovableType _typeID, Coord _coord ){
        id = _id;
        typeID = _typeID;
        coord = _coord;
        changed = true;
        attacked = false;
        lastAttackDamage = 0;
    }
    CliMovable(){}

    void setCoord( Coord c ){ coord = c; changed = true; }
    
    
};


class KClient;

class App
{
    vce::VCE *m_vceobj;

    SDL_Surface *m_screen;
    std::map<int,Sprite*> m_sprmap; // intが負ならBG
    std::map<int,Image*> m_imgmap;
    Sprite *m_bgspr[FLOOR_YSIZE][FLOOR_XSIZE]; //idiom
    static const int VIEW_RANGE = 7; // 上下左右にこれだけ見える
    static const int NUM_SPRITES = 1000;
    static const int DEFAULT_LATENCY = 250;

    Font *m_font;

    std::size_t m_recvPerSec;

    ZoneServerAddress *nextLoginServerAddress;

    
    int executeBody();

    void freeAll();
    void moveAllSprites();
    void updateView();
    Image *movableTypeToImage( k_proto::MovableType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *tileTypeToImage( k_proto::TileType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *effectTypeToImage( k_proto::EffectType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *typeIDToImage( int t );
    int typeIDToLayer( int t );
    static const int LAYER_MAX = 999;
    Point coordToLogicalScreenPixel( Coord c );
    Sprite *getMovSprite( vce::VUint32 id){
        Sprite *s = m_sprmap[id];
        if(!s){
            m_sprmap.erase(id);
        }
        return s;
    }
    void setMovSprite( Sprite *s ){
        m_sprmap[s->id]=s;
    }
    void deleteMovSprite( Sprite *s )
    {
        m_sprmap.erase(s->id);
        delete s;
    }
    std::vector<Sprite*> getAllSprites();


    bool firstLogin ;

    // ingameSender用
    vce::VUint32 tick;
    vce::VUint64 ingameUpdateTimer;

    // ゲーム進行の状態
    vce::VUint32 m_id_generator;

 public:
    KClient *kcli;
    KViewClient *kvcli;
    
    std::map<vce::VUint32,CliMovable*> movmap;
    CliFloor *floor;
    

    static const int WIDTH = 640;
    static const int HEIGHT = 480;
    static const int VIDEO_BPP = 8;
    
    int execute();

    App( int argc, char **argv );

    // テストの状態遷移
    typedef enum {
        TEST_INIT = 0,
        TEST_CONNECTING,
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

    
    vce::VUint32 myMovableID;
    Coord myCoord;
    vce::VUint32 myFloorID;
    k_proto_client::CharacterStatus myCharacterStatus;

    
    // サーバアクセスに必要な情報
    std::string localAccountName;
    std::string localPassword;

    //統計用
    std::size_t recvBytesAccum;
    vce::VUint64 lastPingSentAt;
    vce::VUint64 lastRoundTripTime;

    ~App(){
        delete floor;
    }

    std::vector<CliMovable*> getMovables();
    CliMovable* getMovable(vce::VUint32 id);
    CliMovable* getMovable( Coord c );
    void deleteMovable( vce::VUint32 id );    
    bool Poll();
    void ingameSender();

    void sendMove( int dx, int dy );
    void sendLogout();    
    bool tryAttack( int dx, int dy );

};

extern App *g_app;

#endif

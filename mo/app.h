#ifndef _APP_H_
#define _APP_H_

#include "sprite.h"
#include "font.h"
#include "game.h"
#include "floor.h"
#include "movable.h"
#include "net.h"
#include "id.h"

#include <string>
#include <sstream>
#include <iostream>
#include <math.h>
#include <assert.h>
#include "vce2.h"



class App
{
    vce::VCE *m_vceobj;
    GameListener *m_listener;
    JClient *m_jcli;

    SDL_Surface *m_screen;
    std::map<int,Sprite*> m_sprmap; // intが負ならBG
    std::map<int,Image*> m_imgmap;
    Sprite *m_bgspr[FLOOR_YSIZE][FLOOR_XSIZE]; //idiom
    static const int VIEW_RANGE = 7; // 上下左右にこれだけ見える
    static const int NUM_SPRITES = 1000;
    static const int HOST_GUESTID = 0;
    Font *m_font;

    std::size_t m_recvPerSec;

    std::string m_serverHost;
    vce::VUint64 m_serverPort;
    vce::VUint64 m_lastPingSentAt;

    
    
    bool isHost(){ return m_serverPort == 0; }
    
    int executeBody();

    void freeAll();


    Image *movableTypeToImage( MovableType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *tileTypeToImage( TileType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *effectTypeToImage( EffectType t ){
        return typeIDToImage( static_cast<int>(t));
    }
    Image *typeIDToImage( int t );
    int typeIDToLayer( int t );
    static const int LAYER_MAX = 999;
    Point coordToLogicalScreenPixel( Coord c );
    Coord logicalScreenPixelToCoord( Point p );
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

 public:

    Floor *m_floor;

    static const int WIDTH = 640;
    static const int HEIGHT = 480;
    static const int VIDEO_BPP = 8;
    
    int execute();

    App( int argc, char **argv );

    PlayerCharacter *m_mychar;
    vce::VUint32 getMyCharID(){
        if( m_mychar ){
            return m_mychar->id;
        } else {
            return 0;
        }
    }
        

    vce::VUint32 m_guestid; // ホストから取得するguest id
    
    // guestではIDプールを維持する
    IDPool *m_idpool;
    void keepIDPool();
    
    //統計用
    std::size_t m_recvBytesAccum;
    //    vce::VUint64 m_lastRoundTripTime;

    ~App(){
        if( m_jcli ) delete m_jcli;
        delete m_floor;
    }

    std::vector<Movable*> getMovables();
    std::vector<Movable*> getMovables(Coord c);
    
    Movable* getMovable( vce::VUint32 id);

    vce::VUint32 getGuestID(){ return m_guestid; }
    void deleteMovable( vce::VUint32 id );    
    void moveAllMovables( vce::VUint64 now );
    void updateBackground( vce::VUint64 now );
    void updateSprites( vce::VUint64 now );
    void drawAllSprites( vce::VUint64 now );

    void ingameSender();

    void sendMove( int dx, int dy );
    void sendLogout();    
    bool tryAttack( int dx, int dy );

    std::vector<JServer*> getServerSessions();


 private:
    // 再利用を防ぐためにポインタではなくIDを使うidiom
    // すべてのmovableオブジェクトをここで統一的に管理する
    std::map<vce::VUint32,Movable*> movableMap; 

    // ローカルのIDを1個1個振っていく
    vce::VUint32 idgnerator; 


    void pcMove( int dx, int dy );
    void hostTryPop( vce::VUint64 now );
    
    void attackAtMovable( Character *attacker, Character *attacked );
    void sendAllMovableDiffsToHost();
    void sendAllMovableDiffsToGuests();

        
 public:
    
    // global CRUD
    PlayerCharacter *allocPlayerCharacter( vce::VUint32 guestid, vce::VUint32 id, Coord c );
    Enemy *allocEnemy( vce::VUint32 guestid, vce::VUint32 id, Coord c, MovableType t );
    Bullet *allocShootBullet( vce::VUint32 guestid, vce::VUint32 id, Coord fromCoord, Coord toCoord, vce::VUint32 shooterID, MovableType hitType );    
    void deleteMovable( Movable *m );
    Movable *findMovable( vce::VUint32 id );
    void showAttackEffect( Coord c, int dmg, int hp );
    
    // workfuncs
    void initLandscape();    
    vce::VUint32 getNewID();
    void poll();
    vce::VUint32 countMovable();
    void broadcastKill( Character *ch );
    
    Sprite * spriteAppear( Movable *m, Coord c );
    void dumpMovables();
    
};

extern App *g_app;

#endif

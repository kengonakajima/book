#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "kproto.h"
#include "common.h"
#include "sv.h"




class PlayerCharacter;
class Movable;
class Enemy;
// 1フロア
class Floor
{
 private:
    std::map<vce::VUint32,Movable*> movmap; // そのフロアにいるMovableを保存しておく
    
 public:
    
    Tile tiles[FLOOR_YSIZE][FLOOR_XSIZE]; // ランダムアクセスで速度を上げるidiom

    vce::VUint32 id;
    Floor( vce::VUint32 _id ){
        id = _id;
    }
    Floor(){}
    void setTile( Coord c, k_proto::TileType t ){
        tiles[c.y][c.x] = Tile(t);
    }
    Tile getTile( Coord c ){
        if( !c.insideFloor() )return Tile(k_proto::TILE_ROCK);
        return tiles[c.y][c.x];
    }
    bool walkable( Coord c );

    bool hitMovable( Coord c );
    Movable * getMovable(Coord c );
    void registerMovable( Movable *m );
    void unregisterMovable( Movable *m );
};



// 世界全体
class World
{
 private:
    // 再利用を防ぐためにポインタではなくIDを使うidiom
    static std::map<vce::VUint32,Movable*> movableMap; // すべてのmovableオブジェクトをここで統一的に管理する

    // ローカルのIDを1個1個振っていく
    static vce::VUint32 idgnerator; 
    static Floor *floors[FLOORNUM];
    
 public:
    // global
    static Floor *getFloor( vce::VUint32 floorID );
    static Location getStartLocation();
    
    // global CRUD
    static PlayerCharacter *allocPlayerCharacter( Floor *f, Coord c, KServer* );
    static Enemy *allocEnemy( Floor *f, Coord c, k_proto::MovableType t );
    static void deleteMovable( Movable *m );
    static Movable *findMovable( vce::VUint32 id );
    
    // workfuncs
    static void initLandscape();    
    static vce::VUint32 getNewID(){ return ++idgnerator; }
    static void poll();
    static vce::VUint32 countMovable();

    
};



#endif

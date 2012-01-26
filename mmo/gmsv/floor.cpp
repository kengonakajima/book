
#include "floor.h"
#include "gmsvmain.h"
#include "movable.h"
#include "zone.h"

#include <iostream>
#include <assert.h>

std::map<vce::VUint32,Movable*> World::movableMap;
vce::VUint32 World::idgnerator = 1;
Floor *World::floors[FLOORNUM];


// フロアデータ。スタート地点は、floor0の左半分。
// R:ROCK
// S:SAND
// W:WATER
// G:GRASS
// L:LAVA
// U:UPSTAIR
// D:DOWNSTAIR



Floor *World::getFloor( vce::VUint32 floorID )
{
    // TODO: linear search..
    for(int i=0;i<FLOORNUM;i++){
        if( floors[i] && floors[i]->id == floorID ){
            return floors[i];
        }
    }
    return NULL;
}


    

// 世界を初期化する
void World::initLandscape()
{

    // 地形
    // floor0: 周囲:水 全体:草 たまに:岩 10個:下り階段
    // floor1: 周囲:岩 全体:砂 たまに:岩 10個:上りと下り階段
    // floor2: 周囲:岩 全体:砂 たまに:溶岩 10個:上り階段
    k_proto::TileType t[3][3]= {
        { k_proto::TILE_WATER, k_proto::TILE_GRASS, k_proto::TILE_ROCK },
        { k_proto::TILE_ROCK, k_proto::TILE_SAND, k_proto::TILE_ROCK },
        { k_proto::TILE_ROCK, k_proto::TILE_SAND, k_proto::TILE_LAVA } };
    
    for( int i=0;i<FLOORNUM;i++){
        Floor *f = new Floor(i);
        for( int y=0;y<FLOOR_YSIZE;y++){
            for( int x=0;x<FLOOR_XSIZE;x++){
                if( x == 0 || y == 0
                    || x == FLOOR_XSIZE-1 || y == FLOOR_YSIZE-1 ){
                    f->setTile( Coord(x,y), t[i][0] );
                } else {
                    f->setTile( Coord(x,y), t[i][1] );
                }
                if( ( random() % 20 ) == 0 ){
                    f->setTile( Coord(x,y), t[i][2] );
                }
            }
        }
        floors[i] = f;
    }
    // 真ん中の仕切りの壁をおく（フロアの分割：シームレスを実演するため）
    for( int i=0;i<(FLOORNUM-1);i++){
        Floor *f = floors[i];
        for( int y=0;y<FLOOR_YSIZE;y++){
            if( y >= (FLOOR_YSIZE/2-1) && y <= (FLOOR_YSIZE/2+1))continue;// 真ん中は切れ目を置く（そうでないと行けない）
            for( int x=FLOOR_XSIZE/2-1;x<FLOOR_XSIZE/2+2;x++){
                f->setTile( Coord(x,y),t[i][0]);
            }
        }
    }
    
    // 全部できた後に階段を作る
    for( int i=0;i<(FLOORNUM-1);i++){
        Floor *f = floors[i];
        for( int y=0;y<FLOOR_YSIZE;y++){
            for( int x=0;x<FLOOR_XSIZE;x++){
                if( ( random() % 1000)==0
                    && f->getTile( Coord(x,y)).typeID!=k_proto::TILE_UPSTAIR ){
                    f->setTile( Coord(x,y), k_proto::TILE_DOWNSTAIR );
                    Floor *df = floors[i+1];
                    df->setTile( Coord(x,y), k_proto::TILE_UPSTAIR );
                }
            }
        }
    }
}

PlayerCharacter *World::allocPlayerCharacter( Floor *f, Coord c, KServer *ks )
{
    if( Zone::inThisZone( f->id, c ) == false ){
        return NULL;
    }
    
    PlayerCharacter *ch = new PlayerCharacter( f, c, ks, k_proto::MOVABLE_HUMAN );
    assert(ch);
    ch->id = World::getNewID();
    ch->controller = ks;
    World::movableMap[ch->id] = ch;
    f->registerMovable(ch);
    ch->notify( k_proto::FUNCTION_MOVENOTIFY );    
    return ch;
}

Enemy *World::allocEnemy( Floor *f, Coord c, k_proto::MovableType t )
{
    if( Zone::inThisZone( f->id, c ) == false ){
        return NULL;
    }    
    if( !c.insideFloor() )return NULL;
    
    Enemy *e = new Enemy( f, c, t );
    assert(e);
    e->id = World::getNewID();
    World::movableMap[e->id] = e;
    f->registerMovable(e);
    e->notify( k_proto::FUNCTION_MOVENOTIFY );

    return e;
}


void World::deleteMovable( Movable *m )
{
    std::cerr << "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDELETE:" << m->id << std::endl;
    movableMap.erase(m->id);
    m->floor->unregisterMovable(m);
    m->notify( k_proto::FUNCTION_DISAPPEARNOTIFY );
    delete m;
}

Location World::getStartLocation()
{
    return Location( getFloor(0), Coord( FLOOR_XSIZE/2-30, FLOOR_YSIZE/2 ));
}

void Floor::registerMovable( Movable *m ){
    movmap[m->id] = m;
}
void Floor::unregisterMovable( Movable *m ){
    movmap.erase( m->id );
}


void World::poll()
{
    std::vector<Movable*> vdel;
    std::map<vce::VUint32,Movable*>::iterator it;
    vce::VUint64 now = vce::GetTime();
    for(it = movableMap.begin(); it != movableMap.end(); ++it ){
        Movable *m = (*it).second;
        if(m){ // mapのループ中にdeleteするから
            m->poll(now);
            if( m->toDelete ){
                vdel.push_back(m);
            }
        }
    }
    
    // 削除すべきものは動かした後でまとめて削除
    {
        std::vector<Movable*>::iterator it;
        for(it=vdel.begin();it!=vdel.end(); ++it){
            World::deleteMovable( (*it));
        }
    }
}
vce::VUint32 World::countMovable()
{
    return movableMap.size();
}

Movable *World::findMovable( vce::VUint32 id )
{
    return movableMap[id];
}

Movable * Floor::getMovable(Coord c )
{
    std::map<vce::VUint32,Movable*>::iterator it;
    for(it=movmap.begin(); it != movmap.end(); ++it){
        if( (*it).second->coord == c ){
            return (*it).second;
        }
    }
    return NULL;
}

bool Floor::hitMovable( Coord c )
{
    Movable *m = getMovable(c);
    if( m )return true;
    return false;
}
bool Floor::walkable( Coord c )
{
    bool out = true;

    if( !getTile(c).walkable() ) out = false;
    if( hitMovable(c) ) out = false;

    if( !Zone::inThisZone( id, c ) ) out = false;
    return out;
}

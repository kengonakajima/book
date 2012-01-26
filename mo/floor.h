#ifndef _FLOOR_H_
#define _FLOOR_H_


#include <map>
#include "vce2.h"

class PlayerCharacter;
class Movable;
class Enemy;

// 1フロア
class Floor
{
 private:

    
 public:
    
    Tile tiles[FLOOR_YSIZE][FLOOR_XSIZE]; // ランダムアクセスで速度を上げるidiom


    Floor(){}
    void setTile( int x, int y, TileType t ){
        assert(x>=0&&x<FLOOR_XSIZE);
        assert(y>=0&&y<FLOOR_YSIZE);
        tiles[y][x] = Tile(t);
    }
    Tile getTile( Coord c ){
        if( c.x<0 || c.x>=FLOOR_XSIZE || c.y<0 || c.y>=FLOOR_XSIZE ){
            return Tile(TILE_NONE);
        } else {
            return tiles[static_cast<int>(c.y)][static_cast<int>(c.x)];
        }
    }

    Movable * hitMovable( Coord c, vce::VUint32 exceptID );
    Movable * hitMovableType( Coord c, MovableType mt );    
    Movable * getMovable( Coord c );


};





    





#endif


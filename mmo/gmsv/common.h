#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <sstream>
#include <iostream>
#include <math.h>
#include "vce2.h"
#include "kproto.h"

/*
  gmsvとcliとで共通の定数や設定等を記述する
  
 */

#define FLOORNUM 3
#define FLOOR_XSIZE 512
#define FLOOR_YSIZE 512

#define NOTIFY_DISTANCE 20 // 通知が届く範囲

#define DEFAULT_MOVE_INTERVAL 250 // 移動パケットの頻度

//////////////////////////////////////////
class Coord
{
 public:
    vce::VSint32 x,y;
    Coord(vce::VSint32 _x, vce::VSint32 _y ){
        x = _x;
        y = _y;
    }
    Coord(){
    }

    Coord translate( Coord p ){
        return Coord( x + p.x, y + p.y );
    }

    Coord translate( int _x, int _y ){
        return Coord( this->x + _x, this->y + _y );
    }    
    
    Coord operator+( Coord p ){
        return Coord(x+p.x,y+p.y);
    }
    bool operator==( Coord c ){
        return c.x == x && c.y == y;
    }
    
    Coord right(){
        return Coord( x + 1, y );
    }
    Coord left(){
        return Coord( x - 1, y );
    }
    Coord up(){
        return Coord( x, y - 1 );
    }
    Coord down(){
        return Coord( x, y + 1 );
    }
    double distance( Coord p ){
        return sqrt( (double)(p.x - x)*(p.x - x) + (p.y - y)*(p.y - y) );
    }
    bool insideFloor()
    {
        if( x < 0 || y < 0 || x >= FLOOR_XSIZE || y >= FLOOR_YSIZE ){
            return false;
        } else {
            return true;
        }
    }

    std::string to_s(){
        std::ostringstream ss;
        ss << "(" << x << "," << y << ")";
        return ss.str();
    }


    
};

class Rect
{
 public:
    Coord topLeft, bottomRight;
    Rect( Coord tl, Coord br ){
        topLeft = tl;
        bottomRight = br;
    }
    Rect(){}

    // 右下含まない
    bool includes( Coord c ){
        return ( c.x >= topLeft.x
                 && c.y >= topLeft.y
                 && c.x < bottomRight.x
                 && c.y < bottomRight.y );
    }

    // p1,p2の間に含まれるランダムな点を返す(右下含む）
    Coord randomWithin(){
        int dx = bottomRight.x - topLeft.x + 1;
        int dy = bottomRight.y - topLeft.y + 1;
        
        return Coord(
                     topLeft.x + ( random() % dx ),
                     topLeft.y + ( random() % dy ) );
    }
    
};

class Floor;
class Location
{
 public:
    Coord coord;
    Floor *floor;
    Location( Floor *f, Coord c ){
        coord = c;
        floor = f;
    }
};

// 
class Tile
{
 public:    
    k_proto::TileType typeID;

    Tile( k_proto::TileType t ){
        typeID = t;
    }
    Tile(){}
    
    // 種類を増やしていく過程でテーブルルックアップ方式等にする
    bool walkable(){
        return !( typeID == k_proto::TILE_WATER || typeID == k_proto::TILE_ROCK );
    }
    bool damagable(){
        return( typeID == k_proto::TILE_LAVA );
    }
    bool stair(){
        return( typeID == k_proto::TILE_UPSTAIR || typeID == k_proto::TILE_DOWNSTAIR);
    }
        
};


#endif

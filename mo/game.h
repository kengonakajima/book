#ifndef _GAME_H_
#define _GAME_H_

#include <math.h>
#include <assert.h>

static const int DEFAULT_LATENCY = 250;



typedef enum {
    MOVABLE_NONE=0,
    MOVABLE_MIN=100,
    MOVABLE_HUMAN=100,
    MOVABLE_ENEMY=101,
    MOVABLE_RAT=102,
    MOVABLE_TREE=103,
    MOVABLE_BULLET=104,    
    MOVABLE_MAX=199,
    MOVABLETYPE_ForceFour=0x7fffffff//for 32bit compile
} MovableType;

typedef enum {
    TILE_NONE=0,
    TILE_MIN=1,
    TILE_SAND=1,
    TILE_GRASS=2,
    TILE_WATER=3,
    TILE_ROCK=4,
    TILE_LAVA=5,
    TILE_UPSTAIR=6,
    TILE_DOWNSTAIR=7,
    TILE_TILEDFLOOR=8,
    TILE_MAX=49,
    TILETYPE_ForceFour=0x7fffffff//for 32bit compile
} TileType;


typedef enum {
    EFFECT_NONE=0,
    EFFECT_MIN=200,
    EFFECT_ATTACK=200,
    EFFECT_MAX=299,
    EFFECTTYPE_ForceFour=0x7fffffff//for 32bit compile
} EffectType;

typedef enum {
    ITEM_NONE=0,
    ITEM_MIN=50,
    ITEM_SHORTSWORD=50,
    ITEM_LONGSWORD=51,
    ITEM_MAX=99,
    ITEMTYPE_ForceFour=0x7fffffff//for 32bit compile
} ItemType;



#define FLOOR_XSIZE 512
#define FLOOR_YSIZE 512

#define NOTIFY_DISTANCE 20 // 通知が届く範囲

#define DEFAULT_MOVE_INTERVAL 250 // 移動パケットの頻度

//////////////////////////////////////////
class Coord
{
 public:
    float x, y;
    Coord(float _x, float _y ){
        x = _x;
        y = _y;
    }
    Coord(){
    }

    int ix(){
        return static_cast<int>(x);
    }
    int iy(){
        return static_cast<int>(y);
    }
    Coord translate( Coord p ){
        return Coord( x + p.x, y + p.y );
    }
    Coord captureInteger(){
        Coord c;
        c.x = static_cast<int>(x) * 1.0f;
        c.y = static_cast<int>(y) * 1.0f;
        return c;
    }

    Coord translate( int _x, int _y ){
        return Coord( this->x + _x, this->y + _y );
    }    
    
    Coord operator+( Coord p ){
        return Coord(x+p.x,y+p.y);
    }
    Coord operator-( Coord p ){
        return Coord(x-p.x,y-p.y);
    }
    bool operator==( Coord c ){
        return (c.x == x) && (c.y == y);
    }
    bool operator!=( Coord c ){
        return (c.x != x) || (c.y != y);
    }
    Coord abs(){
        Coord c(this->x, this->y);
        if( c.x < 0 ) c.x *= -1.0f;
        if( c.y < 0 ) c.y *= -1.0f;
        return c;        
    }

    float distance( Coord p ){
        return sqrt( (float)(p.x - x)*(p.x - x) + (p.y - y)*(p.y - y) );
    }
    float length(){
        return distance( Coord(0,0));
    }
    bool insideFloor()
    {
        if( x < 0 || y < 0 || x >= FLOOR_XSIZE || y >= FLOOR_YSIZE ){
            return false;
        } else {
            return true;
        }
    }
    bool hit( Coord c )
    {
        const float HITSIZE = 0.5f;
        if( ( c.x < x + HITSIZE ) 
            && ( x < c.x + HITSIZE ) 
            && ( c.y < y + HITSIZE )
            && ( y < c.y + HITSIZE ) ){
            return true;
        } else {
            return false;
        }
    }
    bool stopped(){
        return ( x == 0.0f && y == 0.0f );
    }
    
    // 指定した座標に至るまでの8方向のdxdyを求める
    void dxdy8( Coord toCoord, int *dx, int *dy ){
        if( x > toCoord.x ){
            *dx = -1;
        } else if( x < toCoord.x ){
            *dx = 1;
        } else {
            *dx = 0;
        }
        if( y > toCoord.y ){
            *dy = -1;
        } else if( y < toCoord.y ){
            *dy = 1;
        } else {
            *dy = 0;
        }
    }

    std::string to_s(){
        std::ostringstream ss;
        ss << "(" << x << "," << y << ")";
        return ss.str();
    }
    
};

#if 0
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
#endif


// 
class Tile
{
 public:    
    TileType typeID;

    Tile( TileType t ){
        typeID = t;
    }
    Tile(){}
    
    bool walkable(){
        if( typeID == TILE_SAND ||
            typeID == TILE_GRASS ||
            typeID == TILE_LAVA ||
            typeID == TILE_UPSTAIR ||
            typeID == TILE_DOWNSTAIR ||
            typeID == TILE_TILEDFLOOR ){
            return true;
        } else {
            return false;
        }

    }
        
};

#define PIXELS_PER_COORD 32  // Coordの1あたり何ピクセルかの対応表




#endif


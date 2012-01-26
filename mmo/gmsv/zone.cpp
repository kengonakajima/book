#include "zone.h"

#include "kproto.h"

vce::VUint32 Zone::serverID=0xffffffff; // arbitrary. TODO

// アドレスとは別のＩＤを与えておくidiom
ZoneServerAddress g_zoneServerAddresses[] = {
    ZoneServerAddress( "localhost", 29000, 0 ),
    ZoneServerAddress( "localhost", 29001, 1 ),
    ZoneServerAddress( "localhost", 29002, 2 ),
    ZoneServerAddress( "localhost", 29003, 3 ),    
};

// サンプルとして、フロアを縦に2個に分割する
// どの位置がどのサーバが管理するはずかの構造体 (idiom)
Zone g_zones[] = {
    // floor 0
    Zone( 0, 0, Rect(Coord(0,0),Coord(FLOOR_XSIZE/2,FLOOR_YSIZE))),
    Zone( 1, 0, Rect(Coord(FLOOR_XSIZE/2,0), Coord(FLOOR_XSIZE,FLOOR_YSIZE))),
    // floor 1
    Zone( 0, 1, Rect(Coord(0,0),Coord(FLOOR_XSIZE/2,FLOOR_YSIZE))),
    Zone( 1, 1, Rect(Coord(FLOOR_XSIZE/2,0), Coord(FLOOR_XSIZE,FLOOR_YSIZE))),
    // floor 2
    Zone( 0, 2, Rect(Coord(0,0),Coord(FLOOR_XSIZE/2,FLOOR_YSIZE))),
    Zone( 1, 2, Rect(Coord(FLOOR_XSIZE/2,0), Coord(FLOOR_XSIZE,FLOOR_YSIZE))),
};

// 位置から、サーバIDを求める
// デフォルトで0
vce::VUint32 Zone::locationToServerID( vce::VUint32 floorID, Coord c )
{
    for(int i=0; i<ARRAYLEN(g_zones);i++){
        if( g_zones[i].rect.includes(c)){
            return g_zones[i].gmsvID;
        }
    }
    return k_proto::NONE;
}

void Zone::setServerID( vce::VUint32 id )
{
    serverID = id;
}

          
ZoneServerAddress* Zone::getServerAddress( vce::VUint32 id )
{
    for(int i=0;i<ARRAYLEN(g_zoneServerAddresses);i++){
        if( g_zoneServerAddresses[i].gmsvID == id ){
            return & g_zoneServerAddresses[i];
        }
    }
    return NULL;
    
}

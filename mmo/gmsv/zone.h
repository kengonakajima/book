#ifndef _ZONE_H_
#define _ZONE_H_

#include "vce2.h"
#include "util.h"
#include "common.h"

#include <string>

class ZoneServerAddress
{
 public:
    std::string hostName;
    vce::VUint16 portNumber;
    vce::VUint32 gmsvID;

    ZoneServerAddress( std::string _hn, vce::VUint16 _pn, vce::VUint32 _gmsvID ){
        hostName = _hn;
        portNumber = _pn;
        gmsvID = _gmsvID;
    }

};


// どのフロアのどの位置がどのサーバインスタンスに属しているかの情報
class Zone
{
public:
    static vce::VUint32 serverID;
    vce::VUint32 gmsvID;
    vce::VUint32 floorID;
    Rect rect;

    Zone( vce::VUint32 _gmsvID,
          vce::VUint32 _floorID,
          Rect _rect ){
        gmsvID = _gmsvID;
        floorID = _floorID;
        rect = _rect;
    }
    static void setServerID( vce::VUint32 id );
    static vce::VUint32 locationToServerID( vce::VUint32 floorID, Coord c );
    static bool inThisZone( vce::VUint32 floorID, Coord c ){
        return locationToServerID( floorID, c ) == serverID;
    }
    static ZoneServerAddress* getServerAddress( vce::VUint32 id );
};



#endif

#ifndef _MOVABLE_H_
#define _MOVABLE_H_

#include "vce2.h"
#include "net.h"
#include "game.h"
#include "sprite.h"

#include <iostream>

         

/*
  modelとしてのMovable
 */
class Movable
{
 public:
    SyncValue *m_pSync;

    typedef enum {
        SVT_TYPEID,
        SVT_COORD_X,
        SVT_COORD_Y,
        SVT_DELTAPERSEC_X,
        
        SVT_DELTAPERSEC_Y,
        SVT_GOAL_X,
        SVT_GOAL_Y,
        SVT_TOSTOP,
        
        SVT_TODELETE,
        SVT_TARGETID,    
        SVT_HP,
        SVT_MAXHP,
        
        SVT_MP,
        SVT_MAXMP,        
        SVT_EXP,
        SVT_LEVEL,
        
        SVT_SHOOTER_ID,
        SVT_HITTYPE,
    } SyncValueType;
    

    // 変化しない変数
    vce::VUint32 id; // ゲーム空間全体で一意のID(host/guest含む)
    vce::VUint32 guestid; // どのゲストで作られたか(0ならhost)
    
    // 頻繁に変化する変数
    MovableType typeID; // synced
    Coord coord; // synced
    Coord deltaPerSec; // 移動速度
    bool toStop; //goalCoordを使うならtrue
    Coord goalCoord; // その地点に到達したら止まる
    bool toDelete; // synced
    
    vce::VUint64 thinkCounter;
    vce::VUint64 lastThinkAt;
    vce::VUint64 lastMoveAt;
    vce::VUint64 createdAt;
    vce::VUint32 pollInterval;
    vce::VUint64 deleteAt;

    FontSprite *m_followFontSprite;

 Movable( vce::VUint32 _guestid, vce::VUint32 _id, MovableType _typeID, Coord _coord ) : 
    id(_id),
        guestid(_guestid),
        typeID( _typeID ),
        coord(_coord),
        toDelete(false),
        deleteAt(0),
        pollInterval(DEFAULT_LATENCY),
        thinkCounter(0),
        lastThinkAt(vce::GetTime()),
        createdAt(vce::GetTime()),
        goalCoord(_coord),
        deltaPerSec(0,0),
        lastMoveAt(vce::GetTime()),
        toStop(false),
        m_followFontSprite(NULL) {
        m_pSync = new SyncValue();

        registerSyncValueTypes( m_pSync );
        
        m_pSync->setInt( SVT_TYPEID, typeID );
        m_pSync->setFloat( SVT_COORD_X, coord.x );
        m_pSync->setFloat( SVT_COORD_Y, coord.y );
        
    }
    Movable(){}
    ~Movable(){
        if( m_pSync ) delete m_pSync;
    }
    static void registerSyncValueTypes( SyncValue *sync )
    {
        sync->registerIntType( SVT_TYPEID );
        sync->registerFloatType( SVT_COORD_X );
        sync->registerFloatType( SVT_COORD_Y );
        sync->registerFloatType( SVT_GOAL_X );
        sync->registerFloatType( SVT_GOAL_Y );
        sync->registerIntType( SVT_SHOOTER_ID );
        sync->registerFloatType( SVT_HITTYPE );
        sync->registerIntType( SVT_TARGETID );
        
    }
    
    static Movable* receiveSyncValueBuffer( vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data, vce::VUint32 data_qt );
    static void receiveDelete( vce::VUint32 guestid, vce::VUint32 id );
    
    void readSyncValueFromBuffer( vce::VUint8 *buf, size_t buflen );

    void poll(vce::VUint64 nowtime){

        move(nowtime);
        
        if( lastThinkAt + pollInterval < nowtime ){
            lastThinkAt = nowtime;
            think(nowtime);
            thinkCounter++;            
        }
    }

    void move( vce::VUint64 now );
    
    // latencyが1000で、 dが1.0差だったら、1秒かけて1Tile動くという意味。
    void moveTo( Coord to, float latency_ms, bool stop ){
        latency_ms /= 1000.0f;
        deltaPerSec = Coord( (to.x - coord.x)/latency_ms,
                             (to.y - coord.y)/latency_ms);
        goalCoord = to;
        toStop = stop;

        m_pSync->setFloat( SVT_GOAL_X, goalCoord.x );
        m_pSync->setFloat( SVT_GOAL_Y, goalCoord.y );
    }
                 
    
    virtual void think(vce::VUint64 t){}
    virtual void hit(Movable*){}
    virtual void everyMove(vce::VUint64 now){};
    
};

class CharacterStatus
{
 public:
    int hp, maxhp, mp, maxmp, exp, level;
    CharacterStatus(){}
    CharacterStatus( int _hp, int _mp, int _exp, int _level ){
        hp = maxhp = _hp;
        mp = maxmp = _mp;
        exp = _exp;
        level = _level;
    }
    void max(){
        hp = maxhp = 100;
        mp = maxmp = 50;
        exp=100;
        level=1;
    }
    void weak(){
        hp = maxhp = 10;
        mp = maxmp = 1;
        exp = 3;
        level = 1;
    }
};

class Character : public Movable
{
 public:

    CharacterStatus status; // synced
    
    Character(){}    
 Character( vce::VUint32 _guestid, vce::VUint32 _id, MovableType _typeID, Coord _coord ):
    Movable( _guestid, _id, _typeID, _coord ){
        
    }


    void attacked( int dmg, Character *shooter ); 
    void getExp( int v );
};


class PlayerCharacter : public Character
{
 public:
    PlayerCharacter(){}
 PlayerCharacter( vce::VUint32 _guestid, vce::VUint32 _id, MovableType _typeID, Coord _coord ):
    Character( _guestid, _id, _typeID, _coord ){
    }

    virtual void think(vce::VUint64 t);
    


};

class Enemy : public Character
{
 public:

    static const vce::VUint32 NO_TARGET = 0;
    vce::VUint32 m_targetID; // NO_TARGET だったらtargetしていない。 synced
    
    Enemy(){}
 Enemy( vce::VUint32 _guestid, vce::VUint32 _id, MovableType _typeID, Coord _coord ):
    Character( _guestid, _id, _typeID, _coord ){
        if( _typeID == MOVABLE_ENEMY ){
            this->status.weak();
        }
    }
    void setTarget( vce::VUint32 _targetID ){
        m_targetID = _targetID;
        m_pSync->setInt( SVT_TARGETID, _targetID );
    }

    virtual void think( vce::VUint64 now );

    void thinkEnemy();
};

class Bullet : public Movable
{
 public:
    vce::VUint32 shooterID; 
    MovableType hitType;    

    
    Bullet(){}
 Bullet( vce::VUint32 _guestid, vce::VUint32 _id, MovableType _typeID, Coord _coord , vce::VUint32 _shooterID, MovableType _hitType ) :
    Movable( _guestid, _id, _typeID, _coord ){
        this->shooterID = _shooterID;
        this->hitType = _hitType;
        this->pollInterval = 0;
 
        m_pSync->setInt( SVT_SHOOTER_ID, static_cast<int>(_shooterID ));
        m_pSync->setInt( SVT_HITTYPE, static_cast<int>(_hitType ));
    }


    virtual void think(vce::VUint64 now);
    virtual void hit(Movable*);
    virtual void everyMove(vce::VUint64 now );

    void testHit( Coord c );
    
};



bool hitPos( Coord c, Coord d, Coord *hitAt, MovableType hitType );
#endif

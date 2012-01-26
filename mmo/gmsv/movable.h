#ifndef _MOVABLE_H_
#define _MOVABLE_H_

#include "vce2.h"
#include "floor.h"
#include "sv.h"
#include "gmsvmain.h"

// 「動き得るもの」
class Movable
{
 protected:
    vce::VUint64 m_interval;
    vce::VUint64 m_lastMoveAt;
    vce::VUint32 m_counter;    
 public:
    vce::VUint32 id;
    Coord coord;
    Floor *floor;
    std::string name;
    k_proto::MovableType typeID;

    bool toDelete;
    virtual void poll( vce::VUint64 t ){}

    
    Movable( Floor *f, Coord c, k_proto::MovableType t ){
        floor = f;
        coord = c;
        typeID = t;
        toDelete = false;
        m_counter = 0;
        m_lastMoveAt = 0;
        m_interval = 0;
    }
    Movable(){}
    void notify( int gen_ftype );

    std::string to_s(){
        std::ostringstream ss;
        ss << "Movable:{ id:" << id << ", floor:" << floor->id << ", name:" << name << ", coord:" << coord.to_s() << ", typeID:" << typeID << ", interval:" << m_interval << ", lastMoveAt:"<< m_lastMoveAt << ", toDelete:" << toDelete << "}";
        return ss.str();
    }
    void setInterval( vce::VUint64 intv ){
        m_interval = intv;
        m_lastMoveAt = vce::GetTime();
    }
    void moveTo( Coord nextCoord );
};

class Item
{
 public:
    vce::VUint64 id;    
    k_proto::ItemType typeID;
    vce::VUint32 num;
    Item( vce::VUint64 _id, k_proto::ItemType _typeID, vce::VUint32 _num ){
        id = _id;
        typeID = _typeID;
        num = _num;
    }
    Item(){}
    std::string to_s(){
        std::ostringstream ss;
        ss << "Item:{ id:" << id << ", typeID:" << typeID << ", num:" << num << "}";
        return ss.str();
    }
    
};

class Skill
{
 public:
    vce::VUint64 id;
    k_proto::SkillType typeID;
    vce::VUint32 level;
    Skill( vce::VUint64 _id, k_proto::SkillType _typeID, vce::VUint32 _level ){
        id = _id;
        typeID = _typeID;
        level = _level;
    }
    Skill(){}
    std::string to_s(){
        std::ostringstream ss;
        ss << "Skill:{ id:" << id << ", typeID" << typeID << ", level:" << level << "}";
        return ss.str();
    }
};


class CharStat
{
 public:
    vce::VSint32 hp;
    vce::VSint32 maxhp;
    vce::VSint32 level;
    vce::VSint32 exp;
    CharStat( vce::VSint32 _hp, vce::VSint32 _maxhp, vce::VSint32 _level, vce::VSint32 _exp ){
        hp = _hp;
        maxhp = _maxhp;
        level = _level;
        exp = _exp;
    }
    CharStat(){}
    
    std::string to_s(){
        std::ostringstream ss;
        ss << "CharStat:{ hp:" << hp << ", maxhp:" << maxhp << ", level:" << level << ", exp:" << exp << "}";
        return ss.str();
    }
};

// 基本ステータスを持っている。
class Character : public Movable
{
 private:
 public:
    CharStat stat;
    static const int ITEMSLOTNUM = 10;
    static const int SKILLSLOTNUM = 10;


    // 高速な処理を実現するためにネイティブに処理可能な状態でキャラクターに持っておく。
    // PCではこれらに修正が加わった後にDBに書き出す(PCのみ)
    // 情報の流れは常に一方通行
    Item items[ITEMSLOTNUM];
    Skill skills[SKILLSLOTNUM];

    k_proto::ItemType equippedItemTypeID;
    
    Character( Floor *f, Coord c, k_proto::MovableType t ) : Movable( f,c,t ){
        memset( items,0,sizeof(items));
        memset( skills,0,sizeof(skills));
        equippedItemTypeID = k_proto::ITEM_NONE;
    }
    void setItem( vce::VUint32 index, Item itm ){
        items[index] = itm;
    }
    void setSkill(vce::VUint32 index, Skill sk ){
        skills[index] = sk;
    }
    std::string to_s(){
        std::ostringstream ss;
        ss << "Character:{ stat:" << stat.to_s() << ", items:[";
        for(int i=0;i<ITEMSLOTNUM;i++){
            ss << items[i].to_s() << ",";
        }
        ss << "], ";
        for(int i=0;i<SKILLSLOTNUM;i++){
            ss << skills[i].to_s() << ",";
        }
        ss << "], ";
        ss << Movable::to_s();
        return ss.str();
    }
    virtual void poll( vce::VUint64 t ){}
    virtual void saveStat(){}
    bool equip( vce::VUint32 slotID );
    void addExp( int v );
    void checkLevel();
    void attacked( Character *attacker, int dmg );
    
    // network helpers
    void sendStatusTo( KServer *ks );
    void attackNotify( vce::VUint32 attackerid, vce::VUint32 attackedid, int dmg  );
    void sendItemNotifyTo( KServer *ks );
};

// 誰かに対する恨みなどの付加的情報をもとに動きまわるCharacter
class Enemy : public Character
{
    void initEnemyStatus();
    
 public:
    void goblinThink();
    
    Enemy( Floor *f, Coord c , k_proto::MovableType t ) : Character(f,c,t){
        initEnemyStatus();
    }

    virtual void poll( vce::VUint64 t ){
        // TODO: 種類が多くなってきたら表にする等する
        if( t > (m_interval+m_lastMoveAt)){
            m_counter++;
            switch(typeID){
            case k_proto::MOVABLE_GOBLIN:
                goblinThink();                
                break;
            }
            m_lastMoveAt = t;
        }
    }



};



// ネットワークから接続して操作するプレイヤー用キャラクター。
class PlayerCharacter : public Character
{
    void recoverHP();

    vce::VUint64 m_lastNotifyAt;
    
 public:
    KServer *controller;
 PlayerCharacter( Floor *f, Coord c, KServer *_ctrl, k_proto::MovableType t ) : Character( f, c, t ){
        controller = _ctrl;
        m_lastNotifyAt = 0;
    }

    void networkMove( int toX, int toY );
    std::string to_s(){
        std::ostringstream ss;
        ss << "PlayerCharacter:{ controller:" << controller << ", " << Character::to_s() << "}";
        return ss.str();
    }
    virtual void poll( vce::VUint64 t ){
        if( t > (5000+m_lastMoveAt)){
            // PCは5秒に1回hp回復　(virtual pollツリーはidiom)
            recoverHP();
            m_lastMoveAt = t;
        }
        if( t > (1000+m_lastNotifyAt)){ // 定期的に通知するidiom
            notify(k_proto::FUNCTION_MOVENOTIFY );
            m_lastNotifyAt = t;
        }
    }
    virtual void saveStat(){
        if( controller ){
            db_proto::PlayerCharacter *dbpc = controller->getDBPC();
            if(dbpc){
                dbpc->exp = stat.exp;
                dbpc->hp = stat.hp;
                dbpc->maxhp = stat.maxhp;
                dbpc->level = stat.level;
                dbpc->floorID = floor->id;
                dbpc->x = coord.x;
                dbpc->y = coord.y;
                g_dbcli->send_put_PlayerCharacter( controller->uID, *dbpc );
            }
        }
    }
};



#endif


#include "movable.h"
#include "sv.h"
#include "util.h"

// 移動通知
void Movable::notify( int gen_ftype )
{
    KServer *sessions[MAX_CLIENT];
    int n = g_listener->GetChildren((vce::Session**)sessions, ARRAYLEN(sessions));
    for(int i=0;i<n;i++){
        PlayerCharacter *pc = sessions[i]->getPC();
        if(pc){
            // 同じフロアの近くにいるプレイヤーに伝える
            if( pc->floor != this->floor
                || pc->coord.distance( this->coord ) > NOTIFY_DISTANCE )continue;
        } else {
            // pcが無くても特別にwatchする状態だったら送る
            Coord c;
            vce::VUint32 flid;
            if( sessions[i]->isViewmode(&c, &flid ) == false ) continue;
            if( this->floor->id != flid
                || this->coord.distance( c ) > NOTIFY_DISTANCE )continue;
        }
        

        switch( gen_ftype ){
        case k_proto::FUNCTION_MOVENOTIFY:
            sessions[i]->send_moveNotify( this->id,
                                          this->typeID,
                                          this->name.c_str(),
                                          this->coord.x,
                                          this->coord.y,
                                          this->floor->id );

            break;
        case k_proto::FUNCTION_DISAPPEARNOTIFY:
            sessions[i]->send_disappearNotify( this->id );
            break;
        }
    }
}
void Character::attackNotify( vce::VUint32 attackerid, vce::VUint32 attackedid, int dmg  )
{
    KServer *sessions[MAX_CLIENT];
    int n = g_listener->GetChildren((vce::Session**)sessions, ARRAYLEN(sessions));
    for(int i=0;i<n;i++){
        PlayerCharacter *pc = sessions[i]->getPC();
        if( !pc )continue;
        
        // 同じフロアの近くにいるプレイヤーに伝える
        if( pc->floor != this->floor
            || pc->coord.distance( this->coord ) > NOTIFY_DISTANCE )continue;
        sessions[i]->send_attackNotify( attackerid, attackedid, dmg );
        this->sendStatusTo(sessions[i]);
    }    
}


void PlayerCharacter::networkMove( int toX, int toY )
{
    if( absvalue( toX - coord.x ) > 1 || absvalue( toY - coord.y ) > 1 ){
        return;
    }

    Coord nc = Coord(toX, toY) ;
    if( !nc.insideFloor() )return;
    if( !floor->walkable(nc))return;

    Tile t = floor->getTile(nc);
    if( t.damagable() && stat.hp <= 1 ){ // ダメージ受けているときは歩いて行けない
        return;
    }
            

    coord = nc;

    notify( k_proto::FUNCTION_MOVENOTIFY );
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// Enemyのルーチン類
void Enemy::goblinThink()
{
    
    // GOBLINはランダムウォーク
    Coord nc = Rect( coord.translate(-1,-1),
                     coord.translate(1,1) ).randomWithin();
    //    std::cerr << "goblinThink cnt:" << m_counter << " hp:" << stat.hp << nc.to_s() << " cur:" << coord.to_s() <<  std::endl;

    if( ( random()%100)==0){
        toDelete = true;
        return;
    }
    if( !nc.insideFloor() ) return;
    if( floor->walkable(nc) == false)return;

    moveTo(nc);

    // 4方向に誰かいたら攻撃する
    Movable *m0 = floor->getMovable( coord.translate(1,0));
    Movable *m1 = floor->getMovable( coord.translate(0,1));
    Movable *m2 = floor->getMovable( coord.translate(0,-1));
    Movable *m3 = floor->getMovable( coord.translate(-1,0));

    if( m0 && m0->typeID == k_proto::MOVABLE_HUMAN ){
        static_cast<Character*>(m0)->attacked( this, 2 );
    }
    if( m1 && m1->typeID == k_proto::MOVABLE_HUMAN ){
        static_cast<Character*>(m1)->attacked( this, 2 );
    }
    if( m2 && m2->typeID == k_proto::MOVABLE_HUMAN ){
        static_cast<Character*>(m2)->attacked( this, 2 );
    }
    if( m3 && m3->typeID == k_proto::MOVABLE_HUMAN ){
        static_cast<Character*>(m3)->attacked( this, 2 );
    }    
    
    
    
}
void Movable::moveTo( Coord nextCoord )
{
    coord = nextCoord;
    notify( k_proto::FUNCTION_MOVENOTIFY ); // 移動を通知
}

// 敵の種類からステータスを設定する
void Enemy::initEnemyStatus()
{
    switch( typeID ){
    case k_proto::MOVABLE_GOBLIN:
        {
            int h = 5 + (random()%10);
            stat = CharStat( h,h,1,0);
            setInterval(1000);
        }
        break;
    default:
        assert(0);
        break;
    }
}

// 攻撃された。
void Character::attacked( Character *attacker, int dmg )
{

    // 装備によってつよくする
    switch( attacker->equippedItemTypeID ){
    case k_proto::ITEM_SHORTSWORD:
        dmg *= 2;
        break;
    case k_proto::ITEM_LONGSWORD:
        dmg *= 3;
        break;
    }

    std::cerr << "attacking. dmg:" << dmg << std::endl;
    // HP
    if( dmg > stat.hp ){
        dmg = stat.hp;
    }
    stat.hp -= dmg;

    attackNotify( attacker->id, this->id, dmg );
                  
    if( stat.hp == 0 ){
        if( typeID == k_proto::MOVABLE_HUMAN ){
            stat.hp = stat.maxhp;
            stat.exp *= 0.8;
        } else {
            std::cerr << "killed. id:" << this->id << std::endl;
            toDelete = true;

            // 殺したので経験値ボーナス
            attacker->addExp(7);
        }
    }

    //
    if( typeID == k_proto::MOVABLE_HUMAN ){
        PlayerCharacter*pc = static_cast<PlayerCharacter*>(this);
        pc->sendStatusTo(pc->controller);
    }
}
void Character::sendStatusTo( KServer *ks )
{
    k_proto::CharacterStatus cs( this->name.c_str(),
                                 this->stat.hp,
                                 this->stat.maxhp,
                                 this->stat.level,
                                 this->stat.exp,
                                 this->floor->id,
                                 this->coord.x,
                                 this->coord.y );
    
    ks->send_characterStatusResult( this->id, cs );
}

void Character::sendItemNotifyTo( KServer *ks )
{
    k_proto::CharacterItem out[ITEMSLOTNUM];

    int outi=0;
    for(int i=0;i<ITEMSLOTNUM;i++){
        if( items[i].id ){
            out[outi++] = k_proto::CharacterItem( i, items[i].typeID, items[i].num);
        }
    }
    ks->send_itemNotify( out, outi );
}
bool Character::equip( vce::VUint32 slotID )
{
    if( slotID >= ITEMSLOTNUM ) return false;

    if( items[slotID].typeID && items[slotID].num > 0 ){
        equippedItemTypeID = items[slotID].typeID;
        return true;
    } else {
        equippedItemTypeID = k_proto::ITEM_NONE;
        return false;
    }
}
void Character::addExp( int v )
{
    stat.exp += v;
    checkLevel();
    saveStat();
}

void Character::checkLevel()
{
    vce::VUint32 nextExp = stat.level * 100;
    if( stat.exp > nextExp ){
        stat.level++; // TODO: 本来はテーブルにする
        stat.maxhp += 4;
        stat.hp = stat.maxhp;
        saveStat();
    }
}

void PlayerCharacter::recoverHP()
{
    if( stat.hp < stat.maxhp ){
        stat.hp ++;
        sendStatusTo( controller );
    }
}

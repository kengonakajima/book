#include "movable.h"
#include "app.h"

#include <iostream>

void Enemy::think(vce::VUint64 now)
{
    switch( typeID ){
    case  MOVABLE_ENEMY:
        thinkEnemy();
        break;
    }
}

void Enemy::thinkEnemy()
{
    // targetがあるかどうかで動きが変わる
    int dx=0;
    int dy=0;
    
    if( m_targetID != NO_TARGET ){
        if( (thinkCounter % 2 ) == 0 ){
            Movable *m = g_app->getMovable( m_targetID );
            if(m){
                coord.dxdy8( m->coord, &dx, &dy );            
            } else {
                setTarget( NO_TARGET );
            }
        }
           
    } else {

        if( ( random() % 7 ) != 0 ){
            dx = dy = 0;
        } else {
            dx = -1 + ( random() % 3 );
            dy = -1 + ( random() % 3 );
        }
    }
    

    // 進む
    Coord nextCo = coord.translate(dx,dy);
    if( g_app->m_floor->hitMovable( nextCo, id ) == NULL 
            && g_app->m_floor->hitMovable( coord, id ) == NULL
            && g_app->m_floor->getTile( nextCo ).walkable() == true ){
        this->moveTo( nextCo, 250.0f, true );
            }
    
    // 撃つ
    if( ( random() % 8 ) == 0 ){
        Coord dc = g_app->m_mychar->coord - this->coord;
        if( dc.abs().length() < 9 ){
            dc.x *= 2;
            dc.y *= 2;
            if( dc.x != 0 || dc.y != 0 ){
                g_app->allocShootBullet( g_app->getGuestID(),
                                         g_app->getNewID(),
                                         coord, coord + dc, id, MOVABLE_HUMAN );
            }
        }
    }

    if( ( random() % 19 ) == 0 ){
        // ターゲットが無かったら探してターゲットにする
        if( m_targetID == NO_TARGET ){
            std::vector<Movable*> v;
            v = g_app->getMovables();
            std::vector<Movable*>::iterator it;
            std::vector<Movable*> found;
            for(it=v.begin(); it != v.end(); ++it ){
                if( (*it)->typeID == MOVABLE_HUMAN
                    && (*it)->guestid == g_app->getGuestID()
                    && (*it)->coord.distance( coord ) < 8.0f ){
                    found.push_back( (*it) );
                }
            }
            if( found.size() > 0 ){
                int i = random() % found.size();
                setTarget( found[i]->id );
                std::cerr << "found player target ID:" << m_targetID << std::endl;
            }
                
        }
    }

    // たまには忘れる
    if( ( random() % 41 ) == 0 ){
        setTarget( NO_TARGET );
    }
    
}

void Bullet::think(vce::VUint64 now)
{

}

// from c,  with vector d, hit at *hitat
// 解像度は10, ゲタは1000
// ret: true if hit.
bool hitPos( Coord c, Coord d, Coord *hitAt, MovableType hitType )
{
    
    float dx = d.x * 1000;
    float dy = d.y * 1000;
    float mdx = dx / 10;
    float mdy = dy / 10;
    float origx = c.x * 1000;
    float origy = c.y * 1000;

    Coord answer = d;
    for(int i=0;i<10;i++){
        Coord curco( ( origx + mdx * i ) / 1000.0f,
                     ( origy + mdy * i ) / 1000.0f );
        if( g_app->m_floor->hitMovable(curco, 0) == false ){
            if( g_app->m_floor->getTile(curco).walkable() == false ){
                *hitAt = answer;
                return true;
            }
            std::vector<Movable *>v = g_app->getMovables(curco);
            std::vector<Movable *>::iterator it;
            for(it=v.begin(); it != v.end(); it ++ ){
                if( (*it) && (*it)->typeID == hitType ){
                    *hitAt = answer;
                    std::cerr << "hit at obj: ans:" << curco.to_s() << std::endl;
                    return true;
                }
            }
            
        }
        answer = curco;
    }
    return false;
}



void Character::attacked( int dmg, Character *shooter )
{
    if( typeID == MOVABLE_ENEMY ){ // TODO: 関数ポインタによる差し替えをする
        Enemy *e = static_cast<Enemy*>(this);
        e->setTarget( shooter->id ); // 攻撃されたら対象を記憶
    }

    status.hp -= dmg;
    if( status.hp < 0 ){
        status.hp = 0;
        status.exp -= dmg;

        switch( typeID  ){
        case MOVABLE_HUMAN:
            break;
        case MOVABLE_ENEMY:
            // Enemyの場合は倒せる
            toDelete = true;
            shooter->getExp(3);

            g_app->broadcastKill(this);
            
            break;
        } 
    }

    // effect
    g_app->showAttackEffect( coord, dmg, status.hp );
    
}

void Character::getExp( int v )
{
    this->status.exp += v;
    if( this->status.exp > 100 ){
        this->status.level ++;
        this->status.exp -= 100;
    }
}

void PlayerCharacter::think( vce::VUint64 now )
{
    if( ( thinkCounter % 5 ) == 0 ){
        m_pSync->fillChanges(true); // たまに全部の情報を同期する
    }
    
}
void Movable::move(vce::VUint64 now )
{
    float dt = (now - lastMoveAt)/1000.0f;
    if( dt > 0 ){
        int xsign = 0, ysign = 0;
        if( coord.x < goalCoord.x )xsign = -1; else if( coord.x > goalCoord.x )xsign = 1;
        if( coord.y < goalCoord.y )ysign = -1; else if( coord.y > goalCoord.y )ysign = 1;
            
        coord.x += deltaPerSec.x * dt;
        coord.y += deltaPerSec.y * dt;
        //            std::cerr << "dt:" << dt << std::endl;            
        //            std::cerr << "x:" << coord.x << "," << coord.y << std::endl;
        //            std::cerr << "dx:" << deltaPerSec.x << "," << deltaPerSec.y << std::endl;

        int xsign2 = 0, ysign2 = 0;
        if( coord.x < goalCoord.x )xsign2 = -1; else if( coord.x > goalCoord.x )xsign2 = 1;
        if( coord.y < goalCoord.y )ysign2 = -1; else if( coord.y > goalCoord.y )ysign2 = 1;
        if( ( xsign != xsign2 || ysign != ysign2 ) && toStop ){
            deltaPerSec = Coord(0,0);
            coord = goalCoord;
        }
    }
    lastMoveAt = now;

    everyMove(now);
    
    //衝突判定
    Movable *hitm =  g_app->m_floor->hitMovable( coord, this->id );
    if( hitm ){
        this->hit( hitm );
    }

    // ネットワーク用の処理
    m_pSync->setFloat( SVT_COORD_X, coord.x );
    m_pSync->setFloat( SVT_COORD_Y, coord.y );
}

void Bullet::everyMove(vce::VUint64 now)
{
    if( g_app->m_floor->getTile(coord).walkable() == false){
        toDelete = true;
    }    
}

void Bullet::hit( Movable *m )
{

    Movable *shooter = g_app->getMovable(shooterID);
    if( !shooter ){
        toDelete = true;
        return;
    }
    
    if( shooter->typeID == MOVABLE_HUMAN ){
        if( m=g_app->m_floor->hitMovableType( coord, MOVABLE_ENEMY ) ){
            toDelete = true;
            Enemy *e = static_cast<Enemy*>(m);
            e->attacked(3, static_cast<Character*>(shooter));
        }
    } else if( shooter->typeID == MOVABLE_ENEMY ){
        if( m=g_app->m_floor->hitMovableType( coord, MOVABLE_HUMAN ) ){
            toDelete = true;
            PlayerCharacter *pc = static_cast<PlayerCharacter*>(m);
            pc->attacked(3,static_cast<Character*>(shooter));
        }
    }
}


Movable * Movable::receiveSyncValueBuffer( vce::VUint32 guestid, vce::VUint32 id, const vce::VUint8 *data, vce::VUint32 data_qt )
{
    SyncValue sval;
    Movable::registerSyncValueTypes( &sval );
    sval.readBuffer( data, data_qt );
    
    Movable *m = g_app->getMovable(id);

    if(!m){
        if( sval.isChanged( SVT_TYPEID )
            && sval.isChanged( SVT_COORD_X )
            && sval.isChanged( SVT_COORD_Y ) ){
            MovableType t = static_cast<MovableType>(sval.getInt( SVT_TYPEID ));
            Coord co( sval.getFloat( SVT_COORD_X ),
                      sval.getFloat( SVT_COORD_Y ) );
    
            std::cerr << "************ no movable. id:" << id << " t:" << t << std::endl;
            switch( t ){
            case MOVABLE_ENEMY:
                {
                    m = g_app->allocEnemy( guestid, id, co, t );
                }
                break;
            case MOVABLE_HUMAN:
                {
                    m = g_app-> allocPlayerCharacter( guestid, id, co );
                }
                break;
            case MOVABLE_BULLET:
                {
                    Coord goal( sval.getFloat( SVT_GOAL_X ),
                                sval.getFloat( SVT_GOAL_Y ) );
                    vce::VUint32 shooterID = sval.getInt( SVT_SHOOTER_ID );
                    Movable *shooter;
                    
                    // 見た目に整合性がとれてないときは出現させない
                    if( ( shooter = g_app->getMovable( shooterID ) )
                        && shooter->coord.distance(co)< 1.0f ){
                        m = g_app->allocShootBullet( guestid,
                                                     id,
                                                     co,
                                                     goal,
                                                     sval.getInt( SVT_SHOOTER_ID ),
                                                     static_cast<MovableType>(sval.getInt( SVT_HITTYPE ) ));
                        if(m){
                            std::cerr << "SHOOT: t:" << m->typeID << std::endl;
                        } else {
                            std::cerr << "couldnt shoot:" << goal.to_s() << std::endl;
                        }
                            
                    } else {
                        std::cerr << "SHOOTER is gone.";
                    }
                }
                break;
            default:
                //ignore. wait for snapshot
                break;
            }
            if(m){
                std::cerr << "CREATED: " << m->coord.to_s() <<  " t:" << m->typeID << std::endl;
            } 

        }
    } else {

        bool toSync = false;
        switch( m->typeID ){
        case MOVABLE_ENEMY:
            {
                Enemy *e = static_cast<Enemy*>(m);
                if( sval.isChanged( SVT_TARGETID ) ){
                    e->setTarget( sval.getInt( SVT_TARGETID ));
                }
                // ターゲットしてない敵だけ同期する
                if( e->m_targetID == Enemy::NO_TARGET ){
                    toSync = true;
                }
            }
            break;
        case MOVABLE_HUMAN:
            toSync = true;
            break;
        case MOVABLE_BULLET:
            toSync = true;
            break;
        default:
            assert(!"invalid typeid");
            break;
        }
        if( toSync ){
            if( sval.isChanged( SVT_COORD_X ) ){
                m->coord.x = sval.getFloat( SVT_COORD_X );
            }
            if( sval.isChanged( SVT_COORD_Y ) ){
                m->coord.y = sval.getFloat( SVT_COORD_Y );
            }
            //            m->m_pSync->clearChanges();
        }

    }

    
    return m;
}

void Movable::receiveDelete( vce::VUint32 guestid, vce::VUint32 id )
{
    Movable *m = g_app->getMovable(id);
    if(m){
        m->toDelete = true;
    }
}

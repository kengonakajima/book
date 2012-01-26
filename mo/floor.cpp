#include "vce2.h"
#include "game.h"

#include "floor.h"
#include "movable.h"
#include "app.h"

#include <iostream>


// 指定した場所に、指定した種類のmovableがいるならそれを返す
// アタリ判定を行う。
Movable * Floor::hitMovableType( Coord c, MovableType mt )
{
    std::vector<Movable*> v;
    v = g_app->getMovables();
    std::vector<Movable*>::iterator it;
    for(it=v.begin(); it != v.end(); ++it ){
        if( (*it)->coord.hit( c ) && (*it)->typeID == mt ){
            return (*it);
        }
    }
    return NULL;
}

Movable * Floor::hitMovable( Coord c, vce::VUint32 exceptID )
{
    std::vector<Movable*> v;
    v = g_app->getMovables();
    std::vector<Movable*>::iterator it;
    for(it=v.begin(); it != v.end(); ++it ){
        if( (*it)->coord.hit( c )  ){
            if( exceptID == 0 ){
                return (*it);
            } else {
                if( exceptID != (*it)->id ){
                    return (*it);
                }
            }
        }
    }
    return NULL;
}


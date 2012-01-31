/*
  J Multiplayer.
  使い方

  ./cli       : ホストとして起動
  ./cli localhost : ゲストとして起動してアドレスに接続する。接続できなければ終了
  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "SDL.h"

#include "vce2.h"
#include "util.h"
#include "app.h"
#include "sprite.h"
#include "font.h"
#include "id.h"


// KServerのsession ID を指定して検索する。
std::vector<JServer*> App::getServerSessions()
{
    JServer *sessions[10];
    int n = m_listener->GetChildren((vce::Session**)sessions, ARRAYLEN(sessions));
    std::vector<JServer*> v;
    for(int i=0;i<n;i++){
        v.push_back( sessions[i] );
    }
    return v;
}


void GameListener::Exception( vce::VCE_EXCEPTION type ){
    std::cerr << "exception !!!!!!!!!!" << vce::ntoa( type ) << std::endl;
    switch( type ){
    case vce::VCE_EXCEPT_BINDFAIL:
        break;
    default:
        break;
    }        
};

vce::Session * GameListener::Accepter( vce::VUint32 remote_addr, vce::VUint16 remote_port ){
    std::cerr << "GameListener: accepted new connection. addr:" << remote_port << std::endl;
    return new JServer();
}



App::App( int argc, char **argv )
{
    if( argc == 1 ){
        // host
        m_serverHost = std::string("");
        m_serverPort = 0;
        std::cerr << "start as HOST" << std::endl;
    } else if( argc == 2 ){
        m_serverHost = std::string( argv[1] );
        m_serverPort = 29900;
        std::cerr << "start as GUEST" << std::endl;
    }

    m_guestid = HOST_GUESTID;
    
    m_floor = new Floor();
    m_recvBytesAccum = 0;

    memset( m_bgspr,0, sizeof(m_bgspr) );
    
    /* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        throw;
	}

	

	/* Set video mode */
	m_screen = SDL_SetVideoMode(WIDTH, HEIGHT, VIDEO_BPP, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT );
	if ( ! m_screen ) {
        std::cerr << "Couldn't set " << WIDTH << "x" << HEIGHT << " video mode: " << SDL_GetError() << std::endl;
        throw;
	}

    // fonts form SDLFont
    m_font = new Font( "fonts/font2", 1,1,1,1 );
    assert(m_font);

    // initialize network
    vce::VCEInitialize();
    m_vceobj = vce::VCECreate();
    m_vceobj->ReUseAddress(true);
	m_vceobj->SetPollingWait(true);    
    m_vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);    

    if( isHost() ){
        fprintf(stderr, "start host(server)\n" );
        // host, listen.
        m_listener = new GameListener();
        m_vceobj->Listen( m_listener, 29900 );
    } else {
        fprintf(stderr, "start guest(client)\n" );        
        // guest, connect.
        m_jcli = new JClient();
        if( !m_vceobj->Connect( m_jcli, m_serverHost.c_str(), m_serverPort, 1000 ) ){
            std::cerr << "fatal error: cannot connect to host." << std::endl;
            assert(0);
        } else {
            std::cerr << "connecting to host.." << std::endl;
            m_vceobj->Poll();
        }

        // IDプールを準備する
        m_idpool = new IDPool(10000);
        assert(m_idpool);
    }
}

void App::freeAll()
{
    {
        std::map<int,Image*>::iterator it;
        for(it = m_imgmap.begin(); it != m_imgmap.end(); ++it ){
            delete (*it).second;
        }
        m_imgmap.clear();
    }
    {
        std::map<int,Sprite*>::iterator it;
        for(it = m_sprmap.begin(); it != m_sprmap.end(); ++it ){
            delete (*it).second;
        }
        m_sprmap.clear();
    }

    delete m_font;
}

bool Sprite_less(const Sprite* left, const Sprite* right)
{
    return left->layer < right->layer;
}

    
void App::drawAllSprites( vce::VUint64 )
{
    SDL_Rect update_rects[NUM_SPRITES];
    int nupdates =0;
    vce::VUint64 now = vce::GetTime();

    // spriteをソートする
    std::vector<Sprite*> sorter;
    std::map<int,Sprite*>::iterator it;
	for ( it = m_sprmap.begin(); it != m_sprmap.end(); ++it ){
        sorter.push_back( (*it).second );
    }
    std::sort( sorter.begin(), sorter.end(), Sprite_less );


    
    Sprite *mySpr = getMovSprite( m_mychar->id );
    Point center(WIDTH/2,HEIGHT/2);
    Point delta(0,0);
    if( mySpr){
        delta = Point( center.x - mySpr->point.x, center.y - mySpr->point.y );
    }
    

    std::vector<Sprite*>::iterator sit;
	for ( sit = sorter.begin(); sit != sorter.end(); ++sit ){
        Sprite *spr = (*sit);
        // 描画
        Image *img = spr->getImage();
        if( img ){
            SDL_Rect r = spr->rect();

            if(!spr->absolute ){
                r.x += delta.x;
                r.y += delta.y;
            }
        
            SDL_BlitSurface( spr->getImage()->getSurface(), NULL, m_screen, & r  );
            update_rects[nupdates++] = r;
        } else { // TODO: imgがなかったらfontだと見なす. virtual draw()とかやる？
            FontSprite *fs = static_cast<FontSprite*>(spr);
            int dx = 0, dy = 0;
            if( !fs->absolute ){
                dx = delta.x;
                dy = delta.y;
            }
            m_font->drawString( m_screen,
                                fs->point.x + dx,
                                fs->point.y + dy,
                                fs->c_str());
        }
    }

    // lastly, show stats
    if(m_mychar){
        std::ostringstream ss;
        ss << " host:" << isHost()
           << " gID:" << getGuestID()
           << " Lv:" << m_mychar->status.level
           << " HP:" << m_mychar->status.hp
           << "/" << m_mychar->status.maxhp
           << " EXP:" << m_mychar->status.exp
           << "             ping:" << ( m_jcli ? m_jcli->m_lastRoundTripTime : 0 )
           << " recv:" << m_recvPerSec << "b/s "
           << " " << m_mychar->coord.to_s(); 
        m_font->drawString( m_screen, 10,5, ss.str().c_str());
    }
    
	/* Update the screen! */
#if 0
	if ( (m_screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF ) {
		SDL_Flip(m_screen);
	} else {
		SDL_UpdateRects(m_screen, nupdates, update_rects);
	}
#endif
	SDL_Flip(m_screen);    
}

int App::execute()
{
    int r = executeBody();
    SDL_Quit();
    return r;
}


int App::executeBody()
{
    srand(time(NULL));


    // syncvalのテスト
    SyncValue::selfTest();
    
    // ゲーム空間を初期化 
    initLandscape();

    if( ! isHost()  ){
        // ゲストの場合はゲストIDを最初にまず取得する
        vce::VUint64 started = vce::GetTime();
        while(true){
            m_vceobj->Poll();
            if( m_jcli->GetState() == vce::VCE_STATE_ESTABLISHED ){
                keepIDPool();
            
                if( m_guestid != HOST_GUESTID
                    && m_idpool->getLeft() > 1000 ){
                    std::cerr << "got a guest id:" << m_guestid << std::endl;
                    std::cerr << "got idpool:" << m_idpool->getLeft() << std::endl;
                    m_jcli->ready();
                    break;
                }
                if( vce::GetTime() > ( started + 2000 ) ){
                    std::cerr << "guest id and idpool timeout." << std::endl;
                    return -1;
                }                
            } else {
                if( vce::GetTime() > ( started + 1000 ) ){
                    std::cerr << "connect timeout." << std::endl;
                    return -1;                
                }
            }
        }
    }
    
    /* 自分のキャラクターを初期化 */
    m_mychar = allocPlayerCharacter( getGuestID(), getNewID(), Coord(2,2));


    /* 描画まわりを初期化 */
	/* Loop, blitting sprites and waiting for a keystroke */
    bool done = false;
	SDL_Event event;    
    vce::VUint64 framecnt=0, startTime=vce::GetTime();


    int movedx=0, movedy = 0;
    int nextmovedx=0, nextmovedy = 0;
    bool keyup=false;
    
    vce::VUint64 moveInterval = DEFAULT_LATENCY;
    vce::VUint64 lastMoveAt = vce::GetTime();
    vce::VUint64 lastSecAt=0;
    
	while ( !done ) {

        if( lastSecAt < vce::GetTime() - 1000 ){
            lastSecAt = vce::GetTime();
            m_recvPerSec = m_recvBytesAccum;
            m_recvBytesAccum = 0;
        }
        

		/* Check for events */
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                {
                    int dx = (event.button.x - m_screen->w/2)/PIXELS_PER_COORD;
                    int dy = (event.button.y - m_screen->h/2)/PIXELS_PER_COORD;
                    Coord cc = m_mychar->coord.translate(dx,dy);
                    allocShootBullet( getGuestID(),
                                      getNewID(),
                                      m_mychar->coord, cc,
                                      m_mychar->id,
                                      MOVABLE_ENEMY );
                }
                break;
            case SDL_KEYUP:
                movedx = 0; movedy = 0;
                keyup = true;
                break;
            case SDL_KEYDOWN:
                /* Any keypress quits the app... */
                switch( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    done=1;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    movedx = 1; movedy = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    movedx = -1; movedy = 0;
                    break;
                case SDLK_UP:
                case SDLK_w:
                    movedx = 0; movedy = -1;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    movedx = 0; movedy = 1;
                    break;
                case SDLK_SPACE: break;
                case SDLK_p:
                    dumpMovables();
                    
                    break;
                case SDLK_b:    
                    break;
                case SDLK_e:
                    // enemy
                    {
                        Coord co = m_mychar->coord.translate( 2, 0 );
                        if( m_floor->hitMovable(co, 0 ) == NULL
                            && m_floor->getTile(co).walkable() ){
                            Enemy *e = allocEnemy( getGuestID(),
                                                   getNewID(),
                                                   co.captureInteger(),
                                                   MOVABLE_ENEMY );
                        }
                    }
                    break;                    
        
                    
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
                break;
			}
		}

        if( movedx != 0 || movedy != 0 ){ // keyおした
            if( (lastMoveAt + moveInterval)< vce::GetTime() ){
                pcMove( movedx, movedy );
                lastMoveAt = vce::GetTime();
                nextmovedx = nextmovedy = 0;
            } else { // 移動中に押した
                if( keyup ){
                    keyup = false;
                    nextmovedx = movedx;
                    nextmovedy = movedy;
                }
            }
        } else {
            if( ( lastMoveAt + moveInterval ) < vce::GetTime()
                && ( nextmovedx != 0 || nextmovedy != 0 ) ){
                pcMove( nextmovedx, nextmovedy );
                lastMoveAt = vce::GetTime();
                nextmovedx = nextmovedy = 0;
            }
        }

        
        /* Erase all the sprites if necessary */
        SDL_FillRect( m_screen, NULL, SDL_MapRGB( m_screen->format, 0x00, 0x00, 0x00) );
        framecnt++;

		


        // network things
        m_vceobj->Poll();

        vce::VUint64 now = vce::GetTime();


        if( isHost() ) this->hostTryPop(now);
        
        this->moveAllMovables(now);
        this->updateSprites(now);
        this->updateBackground(now);        
        this->drawAllSprites(now);

        if( isHost() ){
            this->sendAllMovableDiffsToGuests();
        } else {
            this->sendAllMovableDiffsToHost();
        }

        if( !isHost() ){
            keepIDPool();
        }
        
	}
    std::cerr << "Avg FPS:" << ( (double)framecnt / (double)( vce::GetTime() - startTime ) * 1000.0f ) << std::endl;

    // 解放
    freeAll();

    return 0;
}

void App::keepIDPool()
{
    if( m_idpool->getLeft() < 1000 ){
        if( m_jcli->isSendable()) m_jcli->send_getid( 100 );
        //        std::cerr << "send_getid" << std::endl;
    }
}

Point App::coordToLogicalScreenPixel( Coord c )
{
     return Point( c.x * PIXELS_PER_COORD, c.y * PIXELS_PER_COORD );
}
Coord App::logicalScreenPixelToCoord( Point p )
{
    return Coord( p.x / PIXELS_PER_COORD, p.y / PIXELS_PER_COORD );
}


Image *App::typeIDToImage( int t )
{

    if( !m_imgmap[(int)t]  ){
        const char *name=NULL;

        // TODO: 設定ファイルにする。(idiom)
        switch(t){
        case 0: name = "images/zero.bmp"; break;            
        case TILE_SAND: name = "images/sand.bmp"; break;
        case TILE_GRASS: name = "images/grass.bmp"; break;
        case TILE_WATER: name = "images/water.bmp"; break;
        case TILE_ROCK: name = "images/rock.bmp"; break;
        case TILE_LAVA: name = "images/lava.bmp"; break;
        case TILE_UPSTAIR: name = "images/upstair.bmp"; break;
        case TILE_DOWNSTAIR: name = "images/downstair.bmp"; break;
        case TILE_TILEDFLOOR: name = "images/tiledfloor.bmp"; break;
        case MOVABLE_HUMAN: name = "images/human.bmp"; break;
        case MOVABLE_ENEMY: name = "images/goblin.bmp"; break;            
        case MOVABLE_RAT: name = "images/rat.bmp"; break;
        case MOVABLE_TREE: name = "images/tree.bmp"; break;
        case MOVABLE_BULLET: name = "images/bullet.bmp"; break;
        case EFFECT_ATTACK: name = "images/damage.bmp"; break;
        default:
            name = "error.bmp";
            break;
        }
        Image *newimg = new Image(name);
        assert(newimg);
        m_imgmap[t] = newimg;
    }
    
    return m_imgmap[t];
    
}
// 描画順番の指定
// 数字が多いほど後に描画される
int App::typeIDToLayer( int t )
{
    if( t >= TILE_MIN && t < TILE_MAX ){
        return 1;
    } else if( t >= ITEM_MIN && t < ITEM_MAX ){
        return 2;
    } else if( t >= MOVABLE_MIN && t < MOVABLE_MAX ){
        return 3;
    } else if( t >= EFFECT_MIN && t < EFFECT_MAX ){
        return 4;
    } else {
        return 5;
    }
}




void App::dumpMovables()
{
    std::vector<Movable*>v = getMovables();
    std::vector<Movable*>::iterator it;
    for(it=v.begin();it != v.end(); ++it){
        std::cerr << "v ptr:" << (*it) << " id:" << (*it)->id << " c:" << (*it)->coord.x << "," << (*it)->coord.y << std::endl;
    }
    
}


std::vector<Movable*> App::getMovables()
{
    std::vector<Movable*> out;
    std::map<vce::VUint32,Movable*>::iterator it;
    for(it=movableMap.begin();it != movableMap.end(); ++it){
        out.push_back( (*it).second );
    }
    return out;
}

std::vector<Movable*> App::getMovables(Coord c)
{
    std::vector<Movable*> out;
    std::map<vce::VUint32,Movable*>::iterator it;
    for(it=movableMap.begin();it != movableMap.end(); ++it){
        if( (*it).second->coord == c ){
            out.push_back( (*it).second );
        }
    }
    return out;
}


Movable* App::getMovable(vce::VUint32 id)
{
    Movable *m = movableMap[id];
    if(!m){
        movableMap.erase(id);
    }
    return m;
}



void App::deleteMovable( vce::VUint32 id )
{
    if( movableMap[id] ){
        delete movableMap[id];
        movableMap.erase(id);
    } else {
        movableMap.erase(id); // TODO: mapがデフォルト値を作ってしまう仕様はトリッキー
    }
}

// フロアデータ。スタート地点は、floor0の左半分。
// R:ROCK
// S:SAND
// W:WATER
// G:GRASS
// L:LAVA
// U:UPSTAIR
// D:DOWNSTAIR



// 世界を初期化する
void App::initLandscape()
{

    // 地形
    // floor0: 周囲:水 全体:草 たまに:岩 10個:下り階段
    // floor1: 周囲:岩 全体:砂 たまに:岩 10個:上りと下り階段
    // floor2: 周囲:岩 全体:砂 たまに:溶岩 10個:上り階段
    TileType t[3] = { TILE_ROCK, TILE_TILEDFLOOR, TILE_ROCK };


    m_floor = new Floor();
    for( int y=0;y<FLOOR_YSIZE;y++){
        for( int x=0;x<FLOOR_XSIZE;x++){
            if( x == 0 || y == 0
                || x == FLOOR_XSIZE-1 || y == FLOOR_YSIZE-1 ){
                m_floor->setTile( x,y, t[0] );
            } else {
                m_floor->setTile( x,y, t[1] );
            }
            if( ( random() % 20 ) == 0 ){
                m_floor->setTile( x,y, t[2] );
            }
        }
    }


}

PlayerCharacter *App::allocPlayerCharacter( vce::VUint32 guestid, vce::VUint32 id, Coord c )
{
    PlayerCharacter *ch = new PlayerCharacter( guestid,
                                               id, 
                                               MOVABLE_HUMAN,
                                               c );
    assert(ch);
    movableMap[ch->id] = ch;
    ch->status.max();
    return ch;
}

Bullet *App::allocShootBullet( vce::VUint32 guestid, vce::VUint32 id, Coord fromCoord, Coord toCoord, vce::VUint32 shooterID, MovableType hitType )
{
    if( !fromCoord.insideFloor() || !toCoord.insideFloor() ) return NULL;

    Bullet *b = new Bullet( guestid, id, MOVABLE_BULLET, fromCoord, shooterID, hitType );
    assert(b);
    movableMap[b->id] = b;
    b->moveTo( toCoord, fromCoord.distance(toCoord)*1000 / 3, false );

    Sprite *s = spriteAppear(b, fromCoord );
    return b;
}


Enemy *App::allocEnemy( vce::VUint32 guestid, vce::VUint32 id, Coord c, MovableType t )
{
    if( !c.insideFloor() )return NULL;
    
    Enemy *e = new Enemy( guestid, id, t, c );
    assert(e);
    movableMap[e->id] = e;

    std::ostringstream ss;
    ss << e->id; 
    e->m_followFontSprite = new FontSprite( m_font,
                                            Point(0,0),
                                            ss.str().c_str(),
                                            -200000000 - getNewID());
    assert(e->m_followFontSprite);
    e->m_followFontSprite->deleteAt = 0; // 消えない
    e->m_followFontSprite->layer = LAYER_MAX;
    setMovSprite( e->m_followFontSprite );
    
    return e;
}


void App::deleteMovable( Movable *m )
{
    std::cerr << "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDELETE:" << m->id << std::endl;
    movableMap.erase(m->id);
    delete m;
}


void App::moveAllMovables( vce::VUint64 now )
{

    // movableを動かす。
    std::vector<Movable*> vdel;
    std::map<vce::VUint32,Movable*>::iterator it;

    for(it = movableMap.begin(); it != movableMap.end(); ++it ){
        Movable *m = (*it).second;
        if(m){ // mapのループ中にdeleteするから
            m->poll(now);
            if( m->toDelete ){
                vdel.push_back(m);
            }
        }
    }
    
    
    // movableを削除する。　動かした後でまとめて削除
    {
        std::vector<Movable*>::iterator it;
        for(it=vdel.begin();it!=vdel.end(); ++it){
            std::cerr << "delete mov:" << (*it)->id << " t:" << (*it)->typeID << std::endl;
            if( (*it)->m_followFontSprite ){
                (*it)->m_followFontSprite->deleteAt=1;
                (*it)->m_followFontSprite = NULL;
            }
            movableMap.erase( (*it)->id );
            delete (*it);
            
            
        }
    }
}

void App::updateSprites( vce::VUint64 now )
{

    // スプライトを出現させる
    std::vector<Movable*> vmovs = getMovables();
    std::vector<Movable*>::iterator it;    
    for(it= vmovs.begin(); it != vmovs.end(); ++it ){
            
        // new
        if( !getMovSprite( (*it)->id ) ){
            std::cerr << "new t:" << (*it)->typeID  << std::endl;
            spriteAppear( static_cast<Movable*>(*it),(*it)->coord );
            continue;
        }

        // existing
        Sprite *spr = getMovSprite((*it)->id);
        Point p = coordToLogicalScreenPixel( (*it)->coord );
        spr->setPos( p );

        // follow-fonts
        if( (*it)->m_followFontSprite ){
            (*it)->m_followFontSprite->setPos(p.translate(0,-5));
        }
    }

    // スプライトを動かす、消す
    {
        std::map<int,Sprite*>::iterator it;
        std::vector<Sprite*> vDelSpr;
        
        for(it=m_sprmap.begin();it != m_sprmap.end(); ++it){
            Sprite *sp =(*it).second;

            sp->point.x += sp->delta.x;
            sp->point.y += sp->delta.y;
            
            // 自動的に消滅する処理 (idiom)
            if( sp->deleteAt && sp->deleteAt < now ){
                vDelSpr.push_back( sp );
                continue;
            }
            
            // movableが消えたらスプライトも消す
            // 可動物は正のID。可動物だけout処理. TODO: tricky..
            if( sp->id > 0 && getMovable( sp->id ) == NULL ){
                vDelSpr.push_back(sp);
                continue;
            }
        }
        std::vector<Sprite*>::iterator itd;
        for(itd=vDelSpr.begin(); itd != vDelSpr.end(); ++itd){
            deleteMovSprite( (*itd) );
        }

    }
}

void App::updateBackground( vce::VUint64 now )
{
    // 背景を更新
    // 背景スプライトを出現させる
    Coord center = m_mychar->coord;
    for(int cy = center.y - VIEW_RANGE; cy < center.y + VIEW_RANGE; cy ++ ){
        for(int cx = center.x - VIEW_RANGE; cx < center.x + VIEW_RANGE; cx ++ ){
            if( Coord(cx,cy).insideFloor()==false)continue;
            if( !m_bgspr[cy][cx] ){
                Tile t = m_floor->getTile(Coord(cx,cy));
                if( t.typeID != 0 ){
                    Point p = coordToLogicalScreenPixel( Coord( cx, cy ) );
                    Image *img = tileTypeToImage( t.typeID );
                    int id =(cx + cy*FLOOR_XSIZE) * (-1) ; // tricky...
                    Sprite *newspr = new Sprite( p, img, id);
                    newspr->layer = typeIDToLayer(t.typeID);
                    m_bgspr[cy][cx] = newspr;
                    setMovSprite(newspr);
                }
            } 
        }
    }

    // 背景スプライトを消す
    for(int cy = center.y - VIEW_RANGE-1; cy < center.y + VIEW_RANGE+1; cy ++ ){
        for(int cx = center.x - VIEW_RANGE-1; cx < center.x + VIEW_RANGE+1; cx ++ ){
            if( Coord(cx,cy).insideFloor()==false)continue;
            if( cy == center.y - VIEW_RANGE-1
                || cy == (center.y + VIEW_RANGE+1-1)
                || cx == center.x - VIEW_RANGE-1
                || cx == (center.x + VIEW_RANGE+1-1) ){
                if( m_bgspr[cy][cx] ){
                    Sprite *spr = m_bgspr[cy][cx];
                    deleteMovSprite( spr );
                    m_bgspr[cy][cx] = NULL;
                }
            }
        }
    }

    // ネットワークのこと
    if( now > ( m_lastPingSentAt + 2000 ) ){
        m_lastPingSentAt = now;
        if( m_jcli && m_jcli->isSendable() ){
            m_jcli->send_ping( now );
        }
    }

    
}

vce::VUint32 App::countMovable()
{
    return movableMap.size();
}

Movable *App::findMovable( vce::VUint32 id )
{
    return movableMap[id];
}


// IDを指定して攻撃
void App::attackAtMovable( Character *attacker, Character *attacked )
{
    assert(attacked->typeID == MOVABLE_ENEMY );
    assert(attacker->typeID == MOVABLE_HUMAN );

    attacked->attacked( 3, attacker );
}


Sprite * App::spriteAppear( Movable *m, Coord c )
{
    Point p = coordToLogicalScreenPixel( c );
    Image *img = movableTypeToImage(m->typeID );
    Sprite *newspr = new Sprite( p, img, m->id );
    newspr->layer = typeIDToLayer( m->typeID );
    newspr->setPos( p );
    setMovSprite( newspr );

    return newspr;
}

void App::showAttackEffect( Coord c, int dmg, int hp )
{

    Image *img = effectTypeToImage( EFFECT_ATTACK);
    Point p = coordToLogicalScreenPixel(c);
    Sprite *spr = new Sprite( p, img, -100000000-getNewID()); // tricky..
    
    spr->layer = typeIDToLayer( EFFECT_ATTACK);
    spr->deleteAt = vce::GetTime() + 200;
    setMovSprite( spr );

    int id = -200000000 - getNewID(); // TODO:tricky..
    std::ostringstream ss;
    ss << dmg << "(" << hp << ")";
    FontSprite *fs = new FontSprite( m_font,
                                     p.translate( PIXELS_PER_COORD/2,0),
                                     ss.str(),
                                     id );
    fs->setPos( p.translate( 0,0 ) );
    fs->setDelta( Point( 0,-1));
    fs->deleteAt = vce::GetTime() + 500;
    fs->layer = LAYER_MAX;

    setMovSprite( fs );
}

// 必要な情報をホストに対して送る
void App::sendAllMovableDiffsToHost()
{
    if( !m_jcli || !m_jcli->isSendable()) return;
    if( !m_mychar )return;

    // マイキャラの位置を送信
    vce::VUint8 buf[256];
    size_t buflen;
    m_mychar->m_pSync->getDiffBuff( buf, &buflen );
    m_jcli->send_sync( getGuestID(), m_mychar->id, buf, buflen );

    // 自分が撃った弾
    std::vector<Movable*> vm = getMovables();    
    std::vector<Movable*>::iterator itm;
    for( itm = vm.begin(); itm != vm.end(); ++ itm ){
        Movable *m = (*itm);
        if( m->typeID == MOVABLE_BULLET ){
            Bullet *b = static_cast<Bullet*>(m);
            if( b->shooterID == m_mychar->id ){
                vce::VUint8 buf[256];
                size_t buflen;
                m->m_pSync->getDiffBuff(buf, &buflen); // 変化した分だけバッファに取得
                m_jcli->send_sync( m->guestid, m->id, buf, buflen );
            }
        }
    }
}

// 必要な情報をゲストに対して送る
void App::sendAllMovableDiffsToGuests()
{
    std::vector<JServer*> vs = getServerSessions();
    std::vector<Movable*> vm = getMovables();
    
    std::vector<Movable*>::iterator itm;
    for( itm = vm.begin(); itm != vm.end(); ++ itm ){
        Movable *m = (*itm);
        vce::VUint8 buf[256];
        size_t buflen;
        m->m_pSync->getDiffBuff(buf, &buflen); // 変化した分だけバッファに取得


        if( buflen > 0 && m->guestid == getGuestID()
            && ( m->typeID == MOVABLE_HUMAN || m->typeID == MOVABLE_ENEMY )){
            // 全部のゲストに同報送信する
            std::vector<JServer*>::iterator its;
            for(its=vs.begin(); its != vs.end(); ++its ){
                JServer *js = (*its);

                //                                std::cerr << "send_sync id:" << m->id << " len:" << buflen << std::endl;
                js->send_sync( m->guestid, m->id, buf, buflen );
            }
        }
        m->m_pSync->clearChanges();
    }
}
void App::pcMove( int dx, int dy )
{
    Coord nextCo = m_mychar->coord.translate(dx,dy);
    if( m_floor->getTile(nextCo).walkable() == true ){
        Movable *mNext;
        mNext = m_floor->hitMovableType( nextCo, MOVABLE_ENEMY);
        if( mNext ){
            attackAtMovable( m_mychar, static_cast<Character*>(mNext) );
        } else {
            if( m_mychar->deltaPerSec.stopped() ){
                m_mychar->moveTo( nextCo, 250.0f, true );
            }
        }
    } 
    
}

void App::hostTryPop( vce::VUint64 now )
{

    int r = random();
    if( ( r % 407 ) == 0 ){
        int rx = -5 + (random()% 11);
        int ry = -5 + (random()% 11);
            
        Coord co = m_mychar->coord.translate( rx, ry );
        co = co.captureInteger();
        if( g_app->m_floor->hitMovable(co, 0 ) == NULL
            && g_app->m_floor->getTile(co).walkable() == true ){
            Enemy *e = g_app->allocEnemy( getGuestID(), getNewID(), co, MOVABLE_ENEMY );
        }
    }
}

vce::VUint32 App::getNewID()
{
    if( isHost() ){
        return ++idgnerator;
    } else {
        assert( m_jcli );
        assert( m_idpool );

        return m_idpool->get();
    }
}

// 敵を殺したら全ゲストに同報する
void App::broadcastKill( Character *ch )
{
    if( isHost() ){
        std::vector<JServer*> vs = getServerSessions();
        std::vector<JServer*>::iterator its;
        for(its=vs.begin(); its != vs.end(); ++ its ){
            JServer *js = (*its);
            js->send_delete( getGuestID(), ch->id );
        }
    } else {
        if( m_jcli->isSendable() ){
            m_jcli->send_delete( getGuestID(), ch->id );
        }
    }
    
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "SDL.h"

#include "kproto.h"
#include "../gmsv/common.h"
#include "kcli.h"
#include "vce2.h"
#include "util.h"
#include "app.h"
#include "sprite.h"
#include "font.h"
#include "../gmsv/zone.h"

class FontSprite : public Sprite
{
    std::string m_str;
    Font *m_font;
    
 public:
 FontSprite( Font *f, Point p, std::string s, int id ) : Sprite( p, NULL, id )
    {
        m_str = s;
        m_font = f;    
    }
    const char *c_str(){
        return m_str.c_str();
    }
    
};


App::App( int argc, char **argv )
{
    char *acname = argv[1];

    kcli = NULL;
    kvcli = NULL;
    m_id_generator = 0;
    ingameUpdateTimer = 0;
        
    state = TEST_INIT;
    firstLogin = true;
    floor = new CliFloor();
    recvBytesAccum = 0;
    localPassword=std::string("testpass");
    if(acname){
        localAccountName=std::string(acname);
    } else {
        localAccountName=std::string("");
    }

    nextLoginServerAddress = Zone::getServerAddress(0);    
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

    //
    vce::VCEInitialize();
    m_vceobj = vce::VCECreate();
    m_vceobj->ReUseAddress(true);
	m_vceobj->SetPollingWait(true);    
    m_vceobj->SetSelectAlgorithmCallback(MySelectAlgorithmCallback);    



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

    
void App::moveAllSprites()
{
    SDL_Rect update_rects[NUM_SPRITES];
    int nupdates =0;
    vce::VUint64 now = vce::GetTime();

    std::vector<Sprite*> sorter;
    
    std::map<int,Sprite*>::iterator it;
	for ( it = m_sprmap.begin(); it != m_sprmap.end(); ++it ){
        sorter.push_back( (*it).second );
    }
    std::sort( sorter.begin(), sorter.end(), Sprite_less );


    std::vector<Sprite*>::iterator sit;

    Sprite *mySpr = getMovSprite( myMovableID );
    Point center(WIDTH/2,HEIGHT/2);
    Point delta(0,0);
    if( mySpr){
        delta = Point( center.x - mySpr->point.x, center.y - mySpr->point.y );
    }
    

	for ( sit = sorter.begin(); sit != sorter.end(); ++sit ){
    
        Sprite *spr = (*sit);
        
		spr->move(now);
        spr->think();

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
    std::ostringstream ss;
    ss << "ringo Lv:" << myCharacterStatus.level
        << " HP:" << myCharacterStatus.hp
        << "/" << myCharacterStatus.maxhp
        << " EXP:" << myCharacterStatus.exp
       << "             ping:" << lastRoundTripTime 
        << " recv:" << m_recvPerSec << "b/s "
       << " " << myCoord.to_s(); 
    m_font->drawString( m_screen, 10,5, ss.str().c_str());
    
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
    
	/* Loop, blitting sprites and waiting for a keystroke */
    bool done = false;
	SDL_Event event;    
    vce::VUint64 framecnt=0, startTime=vce::GetTime();
    vce::VUint32 vcebugcounter = 0;

    int movedx=0, movedy = 0;
    vce::VUint64 moveInterval = DEFAULT_LATENCY;
    vce::VUint64 lastMoveSendAt = vce::GetTime();
    vce::VUint64 lastSecAt=0;
    
	while ( !done ) {

        if( lastSecAt < vce::GetTime() - 1000 ){
            lastSecAt = vce::GetTime();
            m_recvPerSec = recvBytesAccum;
            recvBytesAccum = 0;
        }
        

		/* Check for events */
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                SDL_WarpMouse(m_screen->w/2, m_screen->h/2);
                std::cerr << event.button.x << std::endl;
                break;
            case SDL_KEYUP:
                movedx = 0; movedy = 0;
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
                    lastMoveSendAt = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    movedx = -1; movedy = 0;
                    lastMoveSendAt = 0;                    
                    break;
                case SDLK_UP:
                case SDLK_w:
                    movedx = 0; movedy = -1;
                    lastMoveSendAt = 0;                    
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    movedx = 0; movedy = 1;
                    lastMoveSendAt = 0;                    
                    break;
                case SDLK_SPACE: break;
                case SDLK_b:      break;                  
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

        if( (movedx != 0 || movedy != 0 )
            && (lastMoveSendAt + moveInterval)< vce::GetTime()
            && state == TEST_INGAME ){
            if( tryAttack( movedx, movedy ) == false ){
                // 移動しようとする。

                vce::VUint32 curSv = Zone::locationToServerID( myFloorID, myCoord );
                Coord nextCoord = myCoord.translate(movedx,movedy);
                vce::VUint32 nextSv = Zone::locationToServerID( myFloorID, nextCoord );

                // 端からさらに進もうとしたらlogout
                if( curSv != nextSv ){
                    
                    sendLogout();
                    std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
                    state = TEST_LOGOUT_SENT;
                    nextLoginServerAddress = Zone::getServerAddress(nextSv);
                }
                    
                sendMove( movedx, movedy );

            }
            lastMoveSendAt = vce::GetTime();
        }
        
        /* Erase all the sprites if necessary */
        SDL_FillRect( m_screen, NULL, SDL_MapRGB( m_screen->format, 0x00, 0x00, 0x00) );
        framecnt++;

		moveAllSprites();

        // network things
        m_vceobj->Poll();

        this->Poll();

        // ゲーム進行の状態を描画に反映させる
        updateView();
        
	}
    std::cerr << "Avg FPS:" << ( (double)framecnt / (double)( vce::GetTime() - startTime ) * 1000.0f ) << std::endl;

    // 解放
    freeAll();

    return 0;
}

Point App::coordToLogicalScreenPixel( Coord c )
{
     return Point( c.x * PIXEL_PER_COORD, c.y * PIXEL_PER_COORD );
}


Image *App::typeIDToImage( int t )
{

    if( !m_imgmap[(int)t]  ){
        const char *name=NULL;

        // TODO: 設定ファイルにする。(idiom)
        switch(t){
        case 0: name = "images/zero.bmp"; break;            
        case k_proto::TILE_SAND: name = "images/sand.bmp"; break;
        case k_proto::TILE_GRASS: name = "images/grass.bmp"; break;
        case k_proto::TILE_WATER: name = "images/water.bmp"; break;
        case k_proto::TILE_ROCK: name = "images/rock.bmp"; break;
        case k_proto::TILE_LAVA: name = "images/lava.bmp"; break;
        case k_proto::TILE_UPSTAIR: name = "images/upstair.bmp"; break;
        case k_proto::TILE_DOWNSTAIR: name = "images/downstair.bmp"; break;            
        case k_proto::MOVABLE_HUMAN: name = "images/human.bmp"; break;
        case k_proto::MOVABLE_GOBLIN: name = "images/goblin.bmp"; break;            
        case k_proto::MOVABLE_RAT: name = "images/rat.bmp"; break;
        case k_proto::MOVABLE_TREE: name = "images/tree.bmp"; break;
        case k_proto::EFFECT_ATTACK: name = "images/damage.bmp"; break;
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
    if( t >= k_proto::TILE_MIN && t < k_proto::TILE_MAX ){
        return 1;
    } else if( t >= k_proto::ITEM_MIN && t < k_proto::ITEM_MAX ){
        return 2;
    } else if( t >= k_proto::MOVABLE_MIN && t < k_proto::MOVABLE_MAX ){
        return 3;
    } else if( t >= k_proto::EFFECT_MIN && t < k_proto::EFFECT_MAX ){
        return 4;
    } else {
        return 5;
    }
}

std::vector<Sprite*> App::getAllSprites()
{
    std::vector<Sprite*> outv;
    std::map<int,Sprite*>::iterator it;
    for(it=m_sprmap.begin();it != m_sprmap.end(); ++it){
        outv.push_back( (*it).second );
    }
    return outv;
}

// getMovableIDs と getFloor , myCoord などの情報を KClientから読み込んで、
// 現在のスプライトの状態と比較して、違いがあれば、CRUDを行う。
// 
void App::updateView()
{
    // 可動物を更新

    //in
    {
        std::vector<CliMovable*> vmovs = getMovables();
        std::vector<CliMovable*>::iterator it;    
        for(it= vmovs.begin(); it != vmovs.end(); ++it ){
            Point p = coordToLogicalScreenPixel( (*it)->coord );
            if( !getMovSprite( (*it)->id ) ){
                // new
                Image *img = movableTypeToImage( (*it)->typeID );
                Sprite *newspr = new Sprite( p, img, (*it)->id );
                newspr->layer = typeIDToLayer( (*it)->typeID );
                setMovSprite( newspr );

            } else {
                if( (*it)->changed ){
                    Sprite *spr = getMovSprite((*it)->id);
                    spr->moveTo( p, DEFAULT_LATENCY );
                    (*it)->changed = false;
                    if( (*it)->attacked ){
                        (*it)->attacked = false;
                        Image *img = effectTypeToImage( k_proto::EFFECT_ATTACK);
                        Sprite *atspr = new Sprite( p, img, -100000000-(*it)->id);// TODO: tricky..
                        atspr->layer = typeIDToLayer( k_proto::EFFECT_ATTACK);
                        atspr->deleteAt = vce::GetTime() + 200;
                        setMovSprite( atspr );

                        int id = -200000000 - (*it)->id; // TODO:tricky..
                        std::ostringstream ss;
                        ss << (*it)->lastAttackDamage;
                        FontSprite *fs = new FontSprite( m_font,
                                                         p.translate( PIXEL_PER_COORD/2,0),
                                                         ss.str(),
                                                         id );
                        fs->moveTo( p.translate( 0,-50 ), 1000 );
                        fs->deleteAt = vce::GetTime() + 500;
                        fs->layer = LAYER_MAX;
                        setMovSprite( fs );
                    }
                }
            }
        }
    }
    //out
    {
        vce::VUint64 now = vce::GetTime();
        std::vector<Sprite*> v = getAllSprites();
        std::vector<Sprite*>::iterator it;
        for(it=v.begin();it != v.end(); ++it){
            // 可動物は正のID。可動物だけout処理. TODO: tricky..
            if( (*it)->id > 0
                && getMovable( (*it)->id ) == NULL ){
                deleteMovSprite( (*it) );
                continue;
            }
            // 自動的に消滅する処理 (idiom)
            if( (*it)->deleteAt
                && (*it)->deleteAt < now ){
                deleteMovSprite( (*it) );
                continue;
            }
        }
    }

    
    // 背景を更新
    //in (idiom)
    Coord center = myCoord;
    
    for(int cy = center.y - VIEW_RANGE; cy < center.y + VIEW_RANGE; cy ++ ){
        for(int cx = center.x - VIEW_RANGE; cx < center.x + VIEW_RANGE; cx ++ ){
            if( Coord(cx,cy).insideFloor()==false)continue;
            if( !m_bgspr[cy][cx] ){
                Tile t = floor->getTile(Coord(cx,cy));
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

    //out (idiom)
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
}
void App::sendLogout()
{
    if(kcli) kcli->send_logout();
    if(kvcli) kvcli->send_logout();

}
void App::sendMove( int dx, int dy )
{
    if(kcli)kcli->send_move( myCoord.x + dx, myCoord.y + dy );
}


bool App::Poll()
{

    switch(state){
    case TEST_INIT:
        kcli = new KClient();
        assert(kcli);

        if( !m_vceobj->Connect( kcli, nextLoginServerAddress->hostName.c_str(), nextLoginServerAddress->portNumber ) ){
            std::cerr << "fatal error: cannot connect to gmsv" << std::endl;
            assert(0);        
        } else {
            std::cerr << "connecting to gmsv" << std::endl;
            m_vceobj->Poll();
        }
        state = TEST_CONNECTING;

            
        break;
        
    case TEST_CONNECTING:
        if( kcli->GetState() == vce::VCE_STATE_ESTABLISHED ){
            std::cerr << "send ping" << std::endl;
            kcli->send_ping( vce::GetTime());
            state = TEST_PING_SENT;            
            break;
        } else {
            std::cerr << kcli->GetState() << ">" ;
            if( kcli->GetState() == vce::VCE_STATE_UNKNOWN ){
                return false;
            }
        }
        break;
    case TEST_PING_SENT:
        std::cerr << ">";
        break;
        // SIGNUPは省略する
    case TEST_PING_RECEIVED:
        {

            if( localAccountName == "" ){
                // アカウント名が設定されてない場合は、新しいIDを生成して登録(sign up)する
                std::ostringstream idss;
                idss << "ringo" << time(NULL) << "." << m_id_generator;
                localAccountName = idss.str();
                m_id_generator ++;

                kcli->send_signup( localAccountName.c_str(), localPassword.c_str());
                std::cerr << "signup" << std::endl;
                state = TEST_SIGNUP_SENT;                
            } else {
                // 設定されてる場合はプロトコルをスキップして、認証にうつる
                state = TEST_SIGNUP_RECEIVED;
            }
        }

        break;
    case TEST_SIGNUP_SENT:
        std::cerr << ">";        
        break;
    case TEST_SIGNUP_RECEIVED:
        kcli->send_authentication( localAccountName.c_str(), localPassword.c_str());
        state = TEST_AUTHENTICATION_SENT;
        std::cerr << "auth" << std::endl;
        break;
    case TEST_AUTHENTICATION_SENT:
        std::cerr << ">";        
        break;
    case TEST_AUTHENTICATION_RECEIVED:
        if( firstLogin ){
            kcli->send_createCharacter( "ringo" );
            firstLogin = false;
            state = TEST_CREATECHARACTER_SENT;
            std::cerr << "createchar" << std::endl;
        } else {
            state = TEST_CREATECHARACTER_RECEIVED;
            std::cerr << "createchar skipped" << std::endl;            
        }
        
        break;

    case TEST_CREATECHARACTER_SENT:
        std::cerr << ">";                
        break;
    case TEST_CREATECHARACTER_RECEIVED:
        std::cerr << "createchar ok." << std::endl;
        
        kcli->send_listCharacter();
        state = TEST_LISTCHARACTER_SENT;
        std::cerr << "listchar." << std::endl;
        break;
        
    case TEST_LISTCHARACTER_SENT:
        std::cerr << ">";                
        break;
    case TEST_LISTCHARACTER_RECEIVED:
        std::cerr << "listchar ok." << std::endl;
        kcli->send_login( "ringo" );
        state = TEST_LOGIN_SENT;
        std::cerr << "login." << std::endl;        
        break;

    case TEST_LOGIN_SENT:
        std::cerr << ">";
        break;
    case TEST_LOGIN_RECEIVED:
        state = TEST_INGAME;
        break;
    case TEST_INGAME:
        {

            vce::VUint64 now = vce::GetTime();
            if( now > (ingameUpdateTimer+200) ){
                ingameUpdateTimer = now;
                ingameSender();
                //                printStat();
            }
        }
        break;
    case TEST_LOGOUT_SENT:
        std::cerr << ">";
        break;
    case TEST_SESSION_CLOSED:
        state = TEST_FINISHED;
        break;
    case TEST_FINISHED:
        // do nothing
        std::cerr << "FINISHED!" << std::endl;
        if( nextLoginServerAddress ){
            state = TEST_INIT;
        }
        break;
    default:
        std::cerr << "invalid test state" << std::endl;        
        assert(0);
    }
    return true;    

    
}

// INGAME中に定期的に何かするための関数
void App::ingameSender()
{
    std::cerr << ">";
    tick++;
    

    // 地形ロードを試みる
    for( int dx=-1; dx <= 1; dx++){
        for(int dy=-1; dy <=1; dy++){
            Coord ckc = myCoord.translate( dx,dy );
            if(!floor->getLoaded( ckc )){
                Rect r = floor->getPageRect( ckc );
                kcli->send_landscape( myFloorID,
                                      r.topLeft.x, r.topLeft.y,
                                      r.bottomRight.x, r.bottomRight.y );
                floor->setLoaded(ckc,true); // 1回だけ読みこむ。
                std::cerr << "load page:" << ckc.x << "," << ckc.y << std::endl;
            }
        }
    }

    if( ( tick % 5 ) == 0 ){
        lastPingSentAt = vce::GetTime();
        kcli->send_ping( lastPingSentAt );
        std::cerr <<" p\n";
    }

    // viewすべきサーバがあるか調べる
    // 上下左右の４点
    {
        Coord rc = myCoord.translate( VIEW_RANGE,0);
        Coord lc = myCoord.translate( -VIEW_RANGE,0);
        Coord uc = myCoord.translate( 0,-VIEW_RANGE);
        Coord dc = myCoord.translate( 0,VIEW_RANGE);

        vce::VUint32 nowz = Zone::locationToServerID( myFloorID, myCoord);
        vce::VUint32 rz = Zone::locationToServerID( myFloorID, rc );
        vce::VUint32 lz = Zone::locationToServerID( myFloorID, lc );
        vce::VUint32 uz = Zone::locationToServerID( myFloorID, uc );        
        vce::VUint32 dz = Zone::locationToServerID( myFloorID, dc );

        ZoneServerAddress *zadr = NULL;
        if( nowz != rz ) zadr = Zone::getServerAddress(rz);
        else if( nowz != lz ) zadr = Zone::getServerAddress(lz);
        if( nowz != uz ) zadr = Zone::getServerAddress(uz);
        if( nowz != dz ) zadr = Zone::getServerAddress(dz);

        if( zadr ){
            // 見える範囲に別サーバの場所があったらつなぐが
            if( !kvcli ){
                std::cerr << "Conencting to next server" << std::endl;
                kvcli = new KViewClient();
                if(!m_vceobj->Connect( kvcli, zadr->hostName.c_str(), zadr->portNumber)){
                    assert(0);
                }
                
            } else {
                // 定期的に位置を送信し続ける
                if( kvcli->GetState() == vce::VCE_STATE_ESTABLISHED ){
                    kvcli->send_viewmode( myFloorID, myCoord.x, myCoord.y, true );
                }
            }
                
                
        } else {
            // もうそういう場所はないので、ログアウト
            if( kvcli ){
                std::cerr << "Closing from view server" << std::endl;
                kvcli->send_logout();
            }
        }
    }
}

std::vector<CliMovable*> App::getMovables()
{
    std::vector<CliMovable*> out;
    std::map<vce::VUint32,CliMovable*>::iterator it;
    for(it=movmap.begin();it != movmap.end(); ++it){
        out.push_back( (*it).second );
    }
    return out;
}
CliMovable* App::getMovable(vce::VUint32 id)
{
    CliMovable *m = movmap[id];
    if(!m){
        movmap.erase(id);
    }
    return m;
}
CliMovable *App::getMovable( Coord c )
{
    std::map<vce::VUint32,CliMovable*>::iterator it;
    for(it=movmap.begin();it != movmap.end(); ++it){
        if( (*it).second->coord == c ){
            return (*it).second;
        }
    }
    return NULL;
}

// 成功したらtrue
bool App::tryAttack( int dx, int dy )
{
    CliMovable *m = getMovable( myCoord.translate(dx,dy));
    if(m){
        kcli->send_attack( m->id );
        return true;
    }
    return false;
}

void App::deleteMovable( vce::VUint32 id )
{
    if( movmap[id] ){
        delete movmap[id];
        movmap.erase(id);
    } else {
        movmap.erase(id); // TODO: mapがデフォルト値を作ってしまう仕様はトリッキー
    }
}

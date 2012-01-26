#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "vce2.h"
#include "SDL.h"
#include "font.h"

class Image // idiom
{
    SDL_Surface *m_sfc;

 public:
    Image( const char *filename );
    ~Image(){
        SDL_FreeSurface( m_sfc );
    }
                        

    int w(){ return m_sfc->w;}
    int h(){ return m_sfc->h;}

    SDL_Surface *getSurface(){ return m_sfc; }
    
};
class Point
{
 public:
    float x,y;
    Point(float _x, float _y ) : x(_x), y(_y) {}
    Point(){}
    Point translate(int dx, int dy){
        return Point( x + dx, y + dy );
    }
    std::string to_s(){
        std::ostringstream ss;
        ss << "(" << x << "," << y << ")";
        return ss.str();
    }
    
};

class Sprite
{
    Image *m_img;
    
 public:
    int id;
    int layer;
    Point point; // 現在位置
    Point delta; // 1フレームの移動速度
    
    bool absolute; // trueなら絶対座標に表示

    vce::VUint64 deleteAt; // 将来自動的にdeleteする時刻
    
 Sprite( Point _point, Image *_img, int _id ) :
    id(_id), m_img( _img ), absolute(false), deleteAt(0), point(_point), delta(0,0)
    {

    }

    int w(){ return m_img->w(); }
    int h(){ return m_img->h(); }
    SDL_Rect rect(){
        SDL_Rect r;
        r.x = point.x;
        r.y = point.y;
        r.w = w();
        r.h = h();
        return r;
    }
    void setPos( Point p ){
        point = p;
    }
    void setDelta( Point d ){
        delta = d;
    }
    
    virtual void think(){}
                              
    Image *getImage(){ return m_img; }
        
};
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


#endif

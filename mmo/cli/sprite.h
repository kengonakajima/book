#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../gmsv/common.h"
#include "SDL.h"

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
    double x,y;
    Point(double _x, double _y ){ x = _x; y = _y; }
    Point(){}
    Point translate(int dx, int dy){
        return Point( x + dx, y + dy );
    }
};

class Sprite // idiom
{
    Image *m_img;
    
 public:
    int id;
    int layer;
    Point point; // 現在位置
    Point deltaPerSec; // 移動速度
    Point goalPoint; // その地点に到達したら止まる
    bool absolute; // trueなら絶対座標に表示
    vce::VUint64 lastMoveAt;
    vce::VUint64 deleteAt; // 将来自動的にdeleteする時刻
    
    Sprite( Point _point, Image *_img, int _id ){
        id = _id;
        goalPoint = point = _point;
        deltaPerSec = Point(0,0);
        m_img = _img;
        lastMoveAt = vce::GetTime();
        absolute = false;
        deleteAt = 0;
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
    void move( vce::VUint64 now){
        double dt = (now - lastMoveAt)/1000.0f;
        if( dt > 0 ){
            int xsign = 0, ysign = 0;
            if( point.x < goalPoint.x )xsign = -1; else if( point.x > goalPoint.x )xsign = 1;
            if( point.y < goalPoint.y )ysign = -1; else if( point.y > goalPoint.y )ysign = 1;
            
            point.x += deltaPerSec.x * dt;
            point.y += deltaPerSec.y * dt;
            //            std::cerr << "dt:" << dt << std::endl;            
            //            std::cerr << "x:" << point.x << "," << point.y << std::endl;
            //            std::cerr << "dx:" << deltaPerSec.x << "," << deltaPerSec.y << std::endl;

            int xsign2 = 0, ysign2 = 0;
            if( point.x < goalPoint.x )xsign2 = -1; else if( point.x > goalPoint.x )xsign2 = 1;
            if( point.y < goalPoint.y )ysign2 = -1; else if( point.y > goalPoint.y )ysign2 = 1;
            if( xsign != xsign2 || ysign != ysign2 ){
                deltaPerSec = Point(0,0);
            }
            
        }
        lastMoveAt = now;
    }
    void moveTo( Point to, double latency_ms ){
        latency_ms /= 1000.0f;
        deltaPerSec = Point( (to.x - point.x)/latency_ms,
                             (to.y - point.y)/latency_ms);
        goalPoint = to;
    }
    
    virtual void think(){}
                              
    Image *getImage(){ return m_img; }
        
};

#endif

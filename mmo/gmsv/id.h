#ifndef _ID_H_
#define _ID_H_

#include "vce2.h"
#include <assert.h>
#include "util.h"

class IDPool
{
    // Last in Last out.
    std::size_t stackPos; // 新しいIDを何個貯めてるか
    std::size_t stackMax; // 最大数
    vce::VUint64 *stack;
 public:
    IDPool( std::size_t sz ){
        stack = (vce::VUint64*)malloc( sz * sizeof(vce::VUint64));
        assert(stack);
        memset(stack, 0,  sz*sizeof(vce::VUint64));
        stackPos=0;
        stackMax = sz;
    }
    
    // 追加する。 追加できたらtrue
    bool add( vce::VUint64 val ){
        if( stackPos == stackMax ){
            return false;
        }
        stack[stackPos] = val;
        stackPos++;
        return true;
    }
    // 取得して在庫を減らす。 無かったらfalse
    vce::VUint64 get(){
        assert( stackPos > 0 );
        vce::VUint64 val = stack[stackPos-1];
        stack[stackPos-1]=0;
        stackPos--;
        std::cerr << "id.h: get() returns " << val << std::endl;
        return val;
    }
    vce::VUint64 getLeft(){
        return stackPos;
    }


    
};

#endif

#ifndef _KCLI_H_
#define _KCLI_H_

#include <assert.h>

// ゲーム観戦用
class KViewClient : public k_proto_client
{
 public:

    KViewClient( ){}
    ~KViewClient(){}

    // vce funcs
    //	size_t Recv(const vce::VUint8 *p,size_t sz);
    //	void Closed(vce::VCE_CLOSEREASON reason);
    void Detached();
    
    // recv funcs
	void recv_ping( vce::VUint64 t){}
	void recv_signupResult(ResultCode result){}
	void recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt){}
    
	void recv_createCharacterResult(ResultCode result){}
	void recv_authenticationResult(ResultCode result){}
	void recv_loginResult(ResultCode result,vce::VUint32 movableID){}
    
	void recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text){}
	void recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 f );
	void recv_disappearNotify(vce::VSint32 movableID);
	void recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat){}    
    
	void recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID){}  
	void recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage ){}  
	void recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)  {}
	void recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt){}

    void recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt){}
	void recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType){}
    void recv_shopNotify(vce::VSint32 shopMovableID){}  
	void recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price){}  
	void recv_buyResult(vce::VSint32 resultCode){}  
	void recv_sellResult(vce::VSint32 resultCode){}

	void recv_viewmodeResult(ResultCode result){}
    
    
};

// ゲーム本番ログイン用
class KClient : public k_proto_client
{

 public:

    KClient( ){}
    ~KClient(){}

    // vce funcs
	size_t Recv(const vce::VUint8 *p,size_t sz);
	void Closed(vce::VCE_CLOSEREASON reason);
    void Detached();
    
    // recv funcs
	void recv_ping( vce::VUint64 t);
	void recv_signupResult(ResultCode result);
	void recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt);
    
	void recv_createCharacterResult(ResultCode result);
	void recv_authenticationResult(ResultCode result);
	void recv_loginResult(ResultCode result,vce::VUint32 movableID);
    
	void recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text);
	void recv_moveNotify(vce::VSint32 movableID, k_proto::MovableType typeID,const char *name,vce::VSint32 posx,vce::VSint32 posy, vce::VSint32 f );
	void recv_disappearNotify(vce::VSint32 movableID);  
	void recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat);    
    
	void recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID);  
	void recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage );  
	void recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)  ;
	void recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt);

    void recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt);
	void recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType);
    void recv_shopNotify(vce::VSint32 shopMovableID);  
	void recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price);  
	void recv_buyResult(vce::VSint32 resultCode);  
	void recv_sellResult(vce::VSint32 resultCode);

	void recv_viewmodeResult(ResultCode result);
    

};


#endif

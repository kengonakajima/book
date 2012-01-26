#include "vce2.h"

#pragma once
#include "vce2serialize.h"

class k_proto
	:public vce::Codec
{
protected:
	void Parsed(const vce::VUint8 *dat,size_t sz);
public://functions
	enum FUNCTIONS
	{
		FUNCTION_PING,
		FUNCTION_SIGNUP,
		FUNCTION_SIGNUPRESULT,
		FUNCTION_AUTHENTICATION,
		FUNCTION_AUTHENTICATIONRESULT,
		FUNCTION_VIEWMODE,
		FUNCTION_VIEWMODERESULT,
		FUNCTION_LISTCHARACTER,
		FUNCTION_LISTCHARACTERRESULT,
		FUNCTION_CREATECHARACTER,
		FUNCTION_CREATECHARACTERRESULT,
		FUNCTION_LOGIN,
		FUNCTION_LOGINRESULT,
		FUNCTION_LOGOUT,
		FUNCTION_CHAT,
		FUNCTION_CHATNOTIFY,
		FUNCTION_MOVE,
		FUNCTION_MOVENOTIFY,
		FUNCTION_DISAPPEARNOTIFY,
		FUNCTION_CHARACTERSTATUS,
		FUNCTION_CHARACTERSTATUSRESULT,
		FUNCTION_USESKILL,
		FUNCTION_USESKILLRESULT,
		FUNCTION_ATTACK,
		FUNCTION_ATTACKNOTIFY,
		FUNCTION_ITEM,
		FUNCTION_ITEMNOTIFY,
		FUNCTION_EQUIP,
		FUNCTION_EQUIPRESULT,
		FUNCTION_LANDSCAPE,
		FUNCTION_LANDSCAPERESULT,
		FUNCTION_USEITEM,
		FUNCTION_USEITEMNOTIFY,
		FUNCTION_SHOPNOTIFY,
		FUNCTION_SHOPITEMNOTIFY,
		FUNCTION_BUY,
		FUNCTION_BUYRESULT,
		FUNCTION_SELL,
		FUNCTION_SELLRESULT,
		FUNCTIONS_END
	};
public://err codes
	enum RECVERROR
	{
		RECVERROR_INVALID_FUNCTION,
		RECVERROR_INVALID_FUNCTIONFORMAT,
		RECVERROR_DISABLE_FUNCTION,
		RECVERROR_CALLLIMIT
	};
protected:
	void Attached();
	void Connected();
	void Closed(vce::VCE_CLOSEREASON type);
	virtual void RecvException(FUNCTIONS func,RECVERROR code);
	void StatusUpdate();
public:
	k_proto();
public://enum
	enum ResultCode
	{
		FAIL=0,
		NONE=0,
		SUCCESS=1,
		ALREADY=2,
		NEEDMOREENERGY=3,
		TOOLONG=4,
		NOTLOADED=5,
		RESULTCODE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum SkillType
	{
		SKILL_CUTTREE=1,
		SKILL_PUTFIRE=2,
		SKILLTYPE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum TileType
	{
		TILE_MIN=1,
		TILE_SAND=1,
		TILE_GRASS=2,
		TILE_WATER=3,
		TILE_ROCK=4,
		TILE_LAVA=5,
		TILE_UPSTAIR=6,
		TILE_DOWNSTAIR=7,
		TILE_MAX=49,
		TILETYPE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum ItemType
	{
		ITEM_NONE=0,
		ITEM_MIN=50,
		ITEM_SHORTSWORD=50,
		ITEM_LONGSWORD=51,
		ITEM_MAX=99,
		ITEMTYPE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum MovableType
	{
		MOVABLE_NONE=0,
		MOVABLE_MIN=100,
		MOVABLE_HUMAN=100,
		MOVABLE_GOBLIN=101,
		MOVABLE_RAT=102,
		MOVABLE_TREE=103,
		MOVABLE_MAX=199,
		MOVABLETYPE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum EffectType
	{
		EFFECT_MIN=200,
		EFFECT_ATTACK=200,
		EFFECT_MAX=299,
		EFFECTTYPE_ForceFour=0x7fffffff//for 32bit compile
	};

public://const

protected://variables
	unsigned int LastUpdate;
	bool SendFunctionEnable[FUNCTIONS_END];
	bool RecvFunctionEnable[FUNCTIONS_END];
	struct _FunctionStatus
	{
		_FunctionStatus();
		void Update();
		unsigned int RecvCallCount;
		unsigned int RecvCallLimit;
		unsigned int RecvCallLimitTime;
		unsigned int DecreaseTime;
	};
	_FunctionStatus FunctionStatus[FUNCTIONS_END];
	void SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime);
public://size of length
	static const vce::VUint32 size_of_int=4;
	static const vce::VUint32 size_of_short=2;
	static const vce::VUint32 size_of_char=1;
	static const vce::VUint32 size_of_dword=4;
	static const vce::VUint32 size_of_word=2;
	static const vce::VUint32 size_of_byte=1;
	static const vce::VUint32 size_of_float=4;
	static const vce::VUint32 size_of_double=8;
	static const vce::VUint32 size_of_qword=8;
	static const vce::VUint32 size_of_string=1;
	static const vce::VUint32 size_of_unicode=4;
	static const vce::VUint32 size_of_ResultCode=4;
	static const vce::VUint32 size_of_SkillType=4;
	static const vce::VUint32 size_of_TileType=4;
	static const vce::VUint32 size_of_ItemType=4;
	static const vce::VUint32 size_of_MovableType=4;
	static const vce::VUint32 size_of_EffectType=4;
	static const vce::VUint32 size_of_CharacterStatus=size_of_string*(50)+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword;
	static const vce::VUint32 size_of_CharacterItem=size_of_dword+size_of_dword+size_of_dword;
public://struct
	class CharacterStatus
	{
	public:
		CharacterStatus(const CharacterStatus &copy);
		CharacterStatus();
		CharacterStatus(const char *name,vce::VUint32 hp,vce::VUint32 maxhp,vce::VUint32 level,vce::VUint32 exp,vce::VUint32 mapID,vce::VUint32 x,vce::VUint32 y);
		void operator=(const CharacterStatus &copy);
	public:
		char name[50];
		vce::VUint32 hp;
		vce::VUint32 maxhp;
		vce::VUint32 level;
		vce::VUint32 exp;
		vce::VUint32 mapID;
		vce::VUint32 x;
		vce::VUint32 y;
	};
	class CharacterItem
	{
	public:
		CharacterItem(const CharacterItem &copy);
		CharacterItem();
		CharacterItem(vce::VUint32 slotID,vce::VUint32 typeID,vce::VUint32 num);
		void operator=(const CharacterItem &copy);
	public:
		vce::VUint32 slotID;
		vce::VUint32 typeID;
		vce::VUint32 num;
	};
	//struct for send
public://config
	void SendEnable(FUNCTIONS func,bool enable);
	void RecvEnable(FUNCTIONS func,bool enable);
protected://recv func
	virtual void recv_ping(vce::VUint64 timestamp)=0;//<  
public://send func
	bool send_ping(vce::VUint64 timestamp);//<  
public://other
	static const unsigned int xml_crc=0x68a39445;
	static const unsigned int generated_time=0x4d7734e8;
	std::ostream *log;
};

class k_proto_server
	:public k_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_signup(const char *accountname,const char *password)=0;//<  
	virtual void recv_authentication(const char *accountname,const char *password)=0;//<  
	virtual void recv_viewmode(vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y,vce::VUint8 mode)=0;//<  
	virtual void recv_listCharacter()=0;//<  
	virtual void recv_createCharacter(const char *characterName)=0;//<  
	virtual void recv_login(const char *characterName)=0;//<  
	virtual void recv_logout()=0;//<  
	virtual void recv_chat(const char *text)=0;//<  
	virtual void recv_move(vce::VSint32 toX,vce::VSint32 toY)=0;//<  
	virtual void recv_characterStatus(vce::VSint32 movableID)=0;//<  
	virtual void recv_useSkill(vce::VSint32 skillID)=0;//<  
	virtual void recv_attack(vce::VSint32 movableID)=0;//<  
	virtual void recv_item()=0;//<  
	virtual void recv_equip(vce::VSint32 slotID)=0;//<  
	virtual void recv_landscape(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2)=0;//<  
	virtual void recv_useItem(vce::VSint32 slotID)=0;//<  
	virtual void recv_buy(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID)=0;//<  
	virtual void recv_sell(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID)=0;//<  
public://send func
	bool send_signupResult(ResultCode result);//<  
	bool send_authenticationResult(ResultCode result);//<  
	bool send_viewmodeResult(ResultCode result);//<  
	bool send_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt);//<  
	bool send_createCharacterResult(ResultCode result);//<  
	bool send_loginResult(ResultCode result,vce::VUint32 movableID);//<  
	bool send_chatNotify(vce::VSint32 talkerID,const char *name,const char *text);//<  
	bool send_moveNotify(vce::VSint32 movableID,MovableType typeID,const char *name,vce::VSint32 x,vce::VSint32 y,vce::VSint32 floorID);//<  
	bool send_disappearNotify(vce::VSint32 movableID);//<  
	bool send_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat);//<  
	bool send_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID);//<  
	bool send_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage);//<  
	bool send_itemNotify(const CharacterItem *data,vce::VUint32 data_qt);//<  
	bool send_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType);//<  
	bool send_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt);//<  
	bool send_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType);//<  
	bool send_shopNotify(vce::VSint32 shopMovableID);//<  
	bool send_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price);//<  
	bool send_buyResult(vce::VSint32 resultCode);//<  
	bool send_sellResult(vce::VSint32 resultCode);//<  
};

class k_proto_client
	:public k_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_signupResult(ResultCode result)=0;//<  
	virtual void recv_authenticationResult(ResultCode result)=0;//<  
	virtual void recv_viewmodeResult(ResultCode result)=0;//<  
	virtual void recv_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt)=0;//<  
	virtual void recv_createCharacterResult(ResultCode result)=0;//<  
	virtual void recv_loginResult(ResultCode result,vce::VUint32 movableID)=0;//<  
	virtual void recv_chatNotify(vce::VSint32 talkerID,const char *name,const char *text)=0;//<  
	virtual void recv_moveNotify(vce::VSint32 movableID,MovableType typeID,const char *name,vce::VSint32 x,vce::VSint32 y,vce::VSint32 floorID)=0;//<  
	virtual void recv_disappearNotify(vce::VSint32 movableID)=0;//<  
	virtual void recv_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat)=0;//<  
	virtual void recv_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID)=0;//<  
	virtual void recv_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage)=0;//<  
	virtual void recv_itemNotify(const CharacterItem *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)=0;//<  
	virtual void recv_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType)=0;//<  
	virtual void recv_shopNotify(vce::VSint32 shopMovableID)=0;//<  
	virtual void recv_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price)=0;//<  
	virtual void recv_buyResult(vce::VSint32 resultCode)=0;//<  
	virtual void recv_sellResult(vce::VSint32 resultCode)=0;//<  
public://send func
	bool send_signup(const char *accountname,const char *password);//<  
	bool send_authentication(const char *accountname,const char *password);//<  
	bool send_viewmode(vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y,vce::VUint8 mode);//<  
	bool send_listCharacter();//<  
	bool send_createCharacter(const char *characterName);//<  
	bool send_login(const char *characterName);//<  
	bool send_logout();//<  
	bool send_chat(const char *text);//<  
	bool send_move(vce::VSint32 toX,vce::VSint32 toY);//<  
	bool send_characterStatus(vce::VSint32 movableID);//<  
	bool send_useSkill(vce::VSint32 skillID);//<  
	bool send_attack(vce::VSint32 movableID);//<  
	bool send_item();//<  
	bool send_equip(vce::VSint32 slotID);//<  
	bool send_landscape(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2);//<  
	bool send_useItem(vce::VSint32 slotID);//<  
	bool send_buy(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID);//<  
	bool send_sell(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID);//<  
};

namespace vce_gen_serialize
{
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_RESULTCODE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_RESULTCODE
	inline bool Push(const k_proto::ResultCode src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_RESULTCODE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_SKILLTYPE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_SKILLTYPE
	inline bool Push(const k_proto::SkillType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_SKILLTYPE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_TILETYPE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_TILETYPE
	inline bool Push(const k_proto::TileType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_TILETYPE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_ITEMTYPE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_ITEMTYPE
	inline bool Push(const k_proto::ItemType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_ITEMTYPE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_MOVABLETYPE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_MOVABLETYPE
	inline bool Push(const k_proto::MovableType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_MOVABLETYPE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_EFFECTTYPE
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_EFFECTTYPE
	inline bool Push(const k_proto::EffectType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_K_PROTO_EFFECTTYPE
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_CHARACTERSTATUS
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_CHARACTERSTATUS
	inline bool Push(const k_proto::CharacterStatus &src,pack &buf)
	{
		if(!Push(src.name,buf,sizeof(src.name)))return false;
		if(!Push(src.hp,buf))return false;
		if(!Push(src.maxhp,buf))return false;
		if(!Push(src.level,buf))return false;
		if(!Push(src.exp,buf))return false;
		if(!Push(src.mapID,buf))return false;
		if(!Push(src.x,buf))return false;
		if(!Push(src.y,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_k_proto_CharacterStatus
#ifndef VCE_GEN_SERIALIZE_PACK_K_PROTO_CHARACTERITEM
#define VCE_GEN_SERIALIZE_PACK_K_PROTO_CHARACTERITEM
	inline bool Push(const k_proto::CharacterItem &src,pack &buf)
	{
		if(!Push(src.slotID,buf))return false;
		if(!Push(src.typeID,buf))return false;
		if(!Push(src.num,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_k_proto_CharacterItem
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_RESULTCODE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_RESULTCODE
	inline bool Pull(k_proto::ResultCode &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_RESULTCODE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_SKILLTYPE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_SKILLTYPE
	inline bool Pull(k_proto::SkillType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_SKILLTYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_TILETYPE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_TILETYPE
	inline bool Pull(k_proto::TileType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_TILETYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_ITEMTYPE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_ITEMTYPE
	inline bool Pull(k_proto::ItemType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_ITEMTYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_MOVABLETYPE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_MOVABLETYPE
	inline bool Pull(k_proto::MovableType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_MOVABLETYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_EFFECTTYPE
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_EFFECTTYPE
	inline bool Pull(k_proto::EffectType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_K_PROTO_EFFECTTYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_CHARACTERSTATUS
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_CHARACTERSTATUS
	inline bool Pull(k_proto::CharacterStatus &dest,pack &buf)
	{
		if(!Pull(dest.name,buf,sizeof(dest.name)))return false;
		if(!Pull(dest.hp,buf))return false;
		if(!Pull(dest.maxhp,buf))return false;
		if(!Pull(dest.level,buf))return false;
		if(!Pull(dest.exp,buf))return false;
		if(!Pull(dest.mapID,buf))return false;
		if(!Pull(dest.x,buf))return false;
		if(!Pull(dest.y,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_k_proto_CharacterStatus
#ifndef VCE_GEN_SERIALIZE_UNPACK_K_PROTO_CHARACTERITEM
#define VCE_GEN_SERIALIZE_UNPACK_K_PROTO_CHARACTERITEM
	inline bool Pull(k_proto::CharacterItem &dest,pack &buf)
	{
		if(!Pull(dest.slotID,buf))return false;
		if(!Pull(dest.typeID,buf))return false;
		if(!Pull(dest.num,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_k_proto_CharacterItem
}


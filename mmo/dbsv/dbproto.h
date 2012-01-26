#include "vce2.h"

#pragma once
#include "vce2serialize.h"

class db_proto
	:public vce::Codec
{
protected:
	void Parsed(const vce::VUint8 *dat,size_t sz);
public://functions
	enum FUNCTIONS
	{
		FUNCTION_PING,
		FUNCTION_GETNEWID,
		FUNCTION_GETNEWIDRESULT,
		FUNCTION_PUT_PLAYER,
		FUNCTION_PUT_PLAYER_RESULT,
		FUNCTION_GET_PLAYER_BY_ID,
		FUNCTION_GET_PLAYER_BY_ID_RESULT,
		FUNCTION_GET_PLAYER_RANKING_BY_ID,
		FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,
		FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,
		FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,
		FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,
		FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,
		FUNCTION_PUT_PLAYERCHARACTER,
		FUNCTION_PUT_PLAYERCHARACTER_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_BY_ID,
		FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,
		FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_BY_NAME,
		FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,
		FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,
		FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,
		FUNCTION_PUT_CHARACTERITEM,
		FUNCTION_PUT_CHARACTERITEM_RESULT,
		FUNCTION_GET_CHARACTERITEM_BY_ID,
		FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,
		FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,
		FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,
		FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,
		FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,
		FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,
		FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,
		FUNCTION_PUT_CHARACTERSKILL,
		FUNCTION_PUT_CHARACTERSKILL_RESULT,
		FUNCTION_GET_CHARACTERSKILL_BY_ID,
		FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,
		FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,
		FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,
		FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,
		FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,
		FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,
		FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,
		FUNCTION_PUT_PLAYERLOCK,
		FUNCTION_PUT_PLAYERLOCK_RESULT,
		FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,
		FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,
		FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,
		FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,
		FUNCTION_PUT_PLAYERLOCK_IF_STATE,
		FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,
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
	db_proto();
public://enum
	enum ResultCode
	{
		FAIL=0,
		NONE=0,
		SUCCESS=1,
		DUPENTRY=2,
		RESULTCODE_ForceFour=0x7fffffff//for 32bit compile
	};

	enum SortType
	{
		SORT_NONE=0,
		SORT_BIGGERFIRST=1,
		SORT_SMALLERFIRST=2,
		SORTTYPE_ForceFour=0x7fffffff//for 32bit compile
	};

public://const
	static const int MAXIDSET=1000;

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
	static const vce::VUint32 size_of_SortType=4;
	static const vce::VUint32 size_of_Player=size_of_qword+size_of_string*(50)+size_of_dword+size_of_string*(50)+size_of_dword;
	static const vce::VUint32 size_of_PlayerCharacter=size_of_qword+size_of_qword+size_of_string*(50)+size_of_dword+size_of_word+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword+size_of_dword;
	static const vce::VUint32 size_of_CharacterItem=size_of_qword+size_of_qword+size_of_dword+size_of_dword;
	static const vce::VUint32 size_of_CharacterSkill=size_of_qword+size_of_qword+size_of_dword+size_of_dword;
	static const vce::VUint32 size_of_PlayerLock=size_of_qword+size_of_byte+size_of_dword;
public://struct
	class Player
	{
	public:
		Player(const Player &copy);
		Player();
		Player(vce::VUint64 id,const char *accountName,const char *passwordHash);
		void operator=(const Player &copy);
	public:
		vce::VUint64 id;
		char accountName[50];
		char passwordHash[50];
	};
	class PlayerCharacter
	{
	public:
		PlayerCharacter(const PlayerCharacter &copy);
		PlayerCharacter();
		PlayerCharacter(vce::VUint64 id,vce::VUint64 playerID,const char *name,vce::VUint16 level,vce::VUint32 exp,vce::VUint32 hp,vce::VUint32 maxhp,vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y,vce::VUint32 equippedItemTypeID);
		void operator=(const PlayerCharacter &copy);
	public:
		vce::VUint64 id;
		vce::VUint64 playerID;
		char name[50];
		vce::VUint16 level;
		vce::VUint32 exp;
		vce::VUint32 hp;
		vce::VUint32 maxhp;
		vce::VUint32 floorID;
		vce::VUint32 x;
		vce::VUint32 y;
		vce::VUint32 equippedItemTypeID;
	};
	class CharacterItem
	{
	public:
		CharacterItem(const CharacterItem &copy);
		CharacterItem();
		CharacterItem(vce::VUint64 id,vce::VUint64 characterID,vce::VUint32 typeID,vce::VUint32 num);
		void operator=(const CharacterItem &copy);
	public:
		vce::VUint64 id;
		vce::VUint64 characterID;
		vce::VUint32 typeID;
		vce::VUint32 num;
	};
	class CharacterSkill
	{
	public:
		CharacterSkill(const CharacterSkill &copy);
		CharacterSkill();
		CharacterSkill(vce::VUint64 id,vce::VUint64 characterID,vce::VUint32 typeID,vce::VUint32 level);
		void operator=(const CharacterSkill &copy);
	public:
		vce::VUint64 id;
		vce::VUint64 characterID;
		vce::VUint32 typeID;
		vce::VUint32 level;
	};
	class PlayerLock
	{
	public:
		PlayerLock(const PlayerLock &copy);
		PlayerLock();
		PlayerLock(vce::VUint64 playerID,vce::VUint8 state,vce::VUint32 ownerServerID);
		void operator=(const PlayerLock &copy);
	public:
		vce::VUint64 playerID;
		vce::VUint8 state;
		vce::VUint32 ownerServerID;
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
	static const unsigned int xml_crc=0xc0a6b602;
	static const unsigned int generated_time=0x4d7734d2;
	std::ostream *log;
};

class db_proto_server
	:public db_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_getNewID(vce::VUint32 num)=0;//<  
	virtual void recv_put_Player(vce::VUint32 sessionID,Player data)=0;//<  
	virtual void recv_get_Player_by_id(vce::VUint32 sessionID,vce::VUint64 id)=0;//<  
	virtual void recv_get_Player_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_Player_by_accountName(vce::VUint32 sessionID,const char *accountName)=0;//<  
	virtual void recv_get_Player_ranking_by_accountName(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_put_PlayerCharacter(vce::VUint32 sessionID,PlayerCharacter data)=0;//<  
	virtual void recv_get_PlayerCharacter_by_id(vce::VUint32 sessionID,vce::VUint64 id)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_PlayerCharacter_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_PlayerCharacter_by_name(vce::VUint32 sessionID,const char *name)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_name(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_PlayerCharacter_by_level(vce::VUint32 sessionID,vce::VUint16 level)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_level(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_put_CharacterItem(vce::VUint32 sessionID,CharacterItem data)=0;//<  
	virtual void recv_get_CharacterItem_by_id(vce::VUint32 sessionID,vce::VUint64 id)=0;//<  
	virtual void recv_get_CharacterItem_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_CharacterItem_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID)=0;//<  
	virtual void recv_get_CharacterItem_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_put_CharacterSkill(vce::VUint32 sessionID,CharacterSkill data)=0;//<  
	virtual void recv_get_CharacterSkill_by_id(vce::VUint32 sessionID,vce::VUint64 id)=0;//<  
	virtual void recv_get_CharacterSkill_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_get_CharacterSkill_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID)=0;//<  
	virtual void recv_get_CharacterSkill_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_put_PlayerLock(vce::VUint32 sessionID,PlayerLock data)=0;//<  
	virtual void recv_get_PlayerLock_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID)=0;//<  
	virtual void recv_get_PlayerLock_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)=0;//<  
	virtual void recv_put_PlayerLock_if_state(vce::VUint32 sessionID,PlayerLock data,vce::VUint8 state_test)=0;//<  
public://send func
	bool send_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt);//<  
	bool send_put_Player_result(vce::VUint32 sessionID,ResultCode result,Player data);//<  
	bool send_get_Player_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt);//<  
	bool send_get_Player_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt);//<  
	bool send_get_Player_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt);//<  
	bool send_get_Player_ranking_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt);//<  
	bool send_put_PlayerCharacter_result(vce::VUint32 sessionID,ResultCode result,PlayerCharacter data);//<  
	bool send_get_PlayerCharacter_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_ranking_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerCharacter_ranking_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt);//<  
	bool send_put_CharacterItem_result(vce::VUint32 sessionID,ResultCode result,CharacterItem data);//<  
	bool send_get_CharacterItem_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterItem_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterItem_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterItem_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt);//<  
	bool send_put_CharacterSkill_result(vce::VUint32 sessionID,ResultCode result,CharacterSkill data);//<  
	bool send_get_CharacterSkill_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterSkill_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterSkill_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt);//<  
	bool send_get_CharacterSkill_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt);//<  
	bool send_put_PlayerLock_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data);//<  
	bool send_get_PlayerLock_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt);//<  
	bool send_get_PlayerLock_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt);//<  
	bool send_put_PlayerLock_if_state_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data);//<  
};

class db_proto_client
	:public db_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)=0;//<  
	virtual void recv_put_Player_result(vce::VUint32 sessionID,ResultCode result,Player data)=0;//<  
	virtual void recv_get_Player_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_Player_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_Player_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_Player_ranking_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_put_PlayerCharacter_result(vce::VUint32 sessionID,ResultCode result,PlayerCharacter data)=0;//<  
	virtual void recv_get_PlayerCharacter_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerCharacter_ranking_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_put_CharacterItem_result(vce::VUint32 sessionID,ResultCode result,CharacterItem data)=0;//<  
	virtual void recv_get_CharacterItem_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterItem_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterItem_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterItem_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_put_CharacterSkill_result(vce::VUint32 sessionID,ResultCode result,CharacterSkill data)=0;//<  
	virtual void recv_get_CharacterSkill_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterSkill_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterSkill_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_CharacterSkill_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_put_PlayerLock_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data)=0;//<  
	virtual void recv_get_PlayerLock_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_get_PlayerLock_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_put_PlayerLock_if_state_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data)=0;//<  
public://send func
	bool send_getNewID(vce::VUint32 num);//<  
	bool send_put_Player(vce::VUint32 sessionID,Player data);//<  
	bool send_get_Player_by_id(vce::VUint32 sessionID,vce::VUint64 id);//<  
	bool send_get_Player_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_Player_by_accountName(vce::VUint32 sessionID,const char *accountName);//<  
	bool send_get_Player_ranking_by_accountName(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_put_PlayerCharacter(vce::VUint32 sessionID,PlayerCharacter data);//<  
	bool send_get_PlayerCharacter_by_id(vce::VUint32 sessionID,vce::VUint64 id);//<  
	bool send_get_PlayerCharacter_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_PlayerCharacter_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID);//<  
	bool send_get_PlayerCharacter_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_PlayerCharacter_by_name(vce::VUint32 sessionID,const char *name);//<  
	bool send_get_PlayerCharacter_ranking_by_name(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_PlayerCharacter_by_level(vce::VUint32 sessionID,vce::VUint16 level);//<  
	bool send_get_PlayerCharacter_ranking_by_level(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_put_CharacterItem(vce::VUint32 sessionID,CharacterItem data);//<  
	bool send_get_CharacterItem_by_id(vce::VUint32 sessionID,vce::VUint64 id);//<  
	bool send_get_CharacterItem_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_CharacterItem_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID);//<  
	bool send_get_CharacterItem_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_put_CharacterSkill(vce::VUint32 sessionID,CharacterSkill data);//<  
	bool send_get_CharacterSkill_by_id(vce::VUint32 sessionID,vce::VUint64 id);//<  
	bool send_get_CharacterSkill_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_get_CharacterSkill_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID);//<  
	bool send_get_CharacterSkill_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_put_PlayerLock(vce::VUint32 sessionID,PlayerLock data);//<  
	bool send_get_PlayerLock_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID);//<  
	bool send_get_PlayerLock_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset);//<  
	bool send_put_PlayerLock_if_state(vce::VUint32 sessionID,PlayerLock data,vce::VUint8 state_test);//<  
};

namespace vce_gen_serialize
{
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_RESULTCODE
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_RESULTCODE
	inline bool Push(const db_proto::ResultCode src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_DB_PROTO_RESULTCODE
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_SORTTYPE
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_SORTTYPE
	inline bool Push(const db_proto::SortType src,pack &buf){return buf.DirectPush(src);}
#endif//VCE_GEN_SERIALIZE_PACK_DB_PROTO_SORTTYPE
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYER
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYER
	inline bool Push(const db_proto::Player &src,pack &buf)
	{
		if(!Push(src.id,buf))return false;
		if(!Push(src.accountName,buf,sizeof(src.accountName)))return false;
		if(!Push(src.passwordHash,buf,sizeof(src.passwordHash)))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_db_proto_Player
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYERCHARACTER
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYERCHARACTER
	inline bool Push(const db_proto::PlayerCharacter &src,pack &buf)
	{
		if(!Push(src.id,buf))return false;
		if(!Push(src.playerID,buf))return false;
		if(!Push(src.name,buf,sizeof(src.name)))return false;
		if(!Push(src.level,buf))return false;
		if(!Push(src.exp,buf))return false;
		if(!Push(src.hp,buf))return false;
		if(!Push(src.maxhp,buf))return false;
		if(!Push(src.floorID,buf))return false;
		if(!Push(src.x,buf))return false;
		if(!Push(src.y,buf))return false;
		if(!Push(src.equippedItemTypeID,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_db_proto_PlayerCharacter
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_CHARACTERITEM
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_CHARACTERITEM
	inline bool Push(const db_proto::CharacterItem &src,pack &buf)
	{
		if(!Push(src.id,buf))return false;
		if(!Push(src.characterID,buf))return false;
		if(!Push(src.typeID,buf))return false;
		if(!Push(src.num,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_db_proto_CharacterItem
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_CHARACTERSKILL
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_CHARACTERSKILL
	inline bool Push(const db_proto::CharacterSkill &src,pack &buf)
	{
		if(!Push(src.id,buf))return false;
		if(!Push(src.characterID,buf))return false;
		if(!Push(src.typeID,buf))return false;
		if(!Push(src.level,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_db_proto_CharacterSkill
#ifndef VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYERLOCK
#define VCE_GEN_SERIALIZE_PACK_DB_PROTO_PLAYERLOCK
	inline bool Push(const db_proto::PlayerLock &src,pack &buf)
	{
		if(!Push(src.playerID,buf))return false;
		if(!Push(src.state,buf))return false;
		if(!Push(src.ownerServerID,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_PACK_db_proto_PlayerLock
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_RESULTCODE
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_RESULTCODE
	inline bool Pull(db_proto::ResultCode &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_RESULTCODE
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_SORTTYPE
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_SORTTYPE
	inline bool Pull(db_proto::SortType &dest,pack &buf){return buf.DirectPull(dest);}
#endif//VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_SORTTYPE
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYER
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYER
	inline bool Pull(db_proto::Player &dest,pack &buf)
	{
		if(!Pull(dest.id,buf))return false;
		if(!Pull(dest.accountName,buf,sizeof(dest.accountName)))return false;
		if(!Pull(dest.passwordHash,buf,sizeof(dest.passwordHash)))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_db_proto_Player
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYERCHARACTER
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYERCHARACTER
	inline bool Pull(db_proto::PlayerCharacter &dest,pack &buf)
	{
		if(!Pull(dest.id,buf))return false;
		if(!Pull(dest.playerID,buf))return false;
		if(!Pull(dest.name,buf,sizeof(dest.name)))return false;
		if(!Pull(dest.level,buf))return false;
		if(!Pull(dest.exp,buf))return false;
		if(!Pull(dest.hp,buf))return false;
		if(!Pull(dest.maxhp,buf))return false;
		if(!Pull(dest.floorID,buf))return false;
		if(!Pull(dest.x,buf))return false;
		if(!Pull(dest.y,buf))return false;
		if(!Pull(dest.equippedItemTypeID,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_db_proto_PlayerCharacter
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_CHARACTERITEM
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_CHARACTERITEM
	inline bool Pull(db_proto::CharacterItem &dest,pack &buf)
	{
		if(!Pull(dest.id,buf))return false;
		if(!Pull(dest.characterID,buf))return false;
		if(!Pull(dest.typeID,buf))return false;
		if(!Pull(dest.num,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_db_proto_CharacterItem
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_CHARACTERSKILL
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_CHARACTERSKILL
	inline bool Pull(db_proto::CharacterSkill &dest,pack &buf)
	{
		if(!Pull(dest.id,buf))return false;
		if(!Pull(dest.characterID,buf))return false;
		if(!Pull(dest.typeID,buf))return false;
		if(!Pull(dest.level,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_db_proto_CharacterSkill
#ifndef VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYERLOCK
#define VCE_GEN_SERIALIZE_UNPACK_DB_PROTO_PLAYERLOCK
	inline bool Pull(db_proto::PlayerLock &dest,pack &buf)
	{
		if(!Pull(dest.playerID,buf))return false;
		if(!Pull(dest.state,buf))return false;
		if(!Pull(dest.ownerServerID,buf))return false;
		return true;
	}

#endif// VCE_GEN_SERIALIZE_UNPACK_db_proto_PlayerLock
}


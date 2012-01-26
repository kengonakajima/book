#include "dbproto.h"

#define AdjustAlign(n) ((((std::size_t)n)+3)/4*4)

db_proto::db_proto()
{
	log=NULL;
	for(int i=0;i<FUNCTIONS_END;i++)
	{
		SendFunctionEnable[i]=true;
		RecvFunctionEnable[i]=true;
	}
}

db_proto::_FunctionStatus::_FunctionStatus()
{
	RecvCallCount=0;
	RecvCallLimit=0;
	RecvCallLimitTime=0;
}

void db_proto::_FunctionStatus::Update()
{
	unsigned int now=vce::GetTime();
	if(RecvCallLimit&&RecvCallLimitTime)
	{
		if(RecvCallCount)
		{
			int interval=(int)((float)RecvCallLimit/RecvCallLimitTime*1000);
			while(DecreaseTime<now)
			{
				if(RecvCallCount)
					RecvCallCount--;
				DecreaseTime+=interval;
			}
		}
		else
			DecreaseTime=now;
	}
}


db_proto::Player::Player()
{
}

db_proto::Player::Player(const Player &copy)
{
	*this=copy;
}

void db_proto::Player::operator =(const Player &copy)
{
	this->id=copy.id;
	vce::strcpy_s(this->accountName,sizeof(this->accountName),copy.accountName);
	vce::strcpy_s(this->passwordHash,sizeof(this->passwordHash),copy.passwordHash);
}

db_proto::Player::Player(vce::VUint64 id,const char *accountName,const char *passwordHash)
{
	this->id=id;
	vce::strcpy_s(this->accountName,sizeof(this->accountName),accountName);
	vce::strcpy_s(this->passwordHash,sizeof(this->passwordHash),passwordHash);
}

db_proto::PlayerCharacter::PlayerCharacter()
{
}

db_proto::PlayerCharacter::PlayerCharacter(const PlayerCharacter &copy)
{
	*this=copy;
}

void db_proto::PlayerCharacter::operator =(const PlayerCharacter &copy)
{
	this->id=copy.id;
	this->playerID=copy.playerID;
	vce::strcpy_s(this->name,sizeof(this->name),copy.name);
	this->level=copy.level;
	this->exp=copy.exp;
	this->hp=copy.hp;
	this->maxhp=copy.maxhp;
	this->floorID=copy.floorID;
	this->x=copy.x;
	this->y=copy.y;
	this->equippedItemTypeID=copy.equippedItemTypeID;
}

db_proto::PlayerCharacter::PlayerCharacter(vce::VUint64 id,vce::VUint64 playerID,const char *name,vce::VUint16 level,vce::VUint32 exp,vce::VUint32 hp,vce::VUint32 maxhp,vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y,vce::VUint32 equippedItemTypeID)
{
	this->id=id;
	this->playerID=playerID;
	vce::strcpy_s(this->name,sizeof(this->name),name);
	this->level=level;
	this->exp=exp;
	this->hp=hp;
	this->maxhp=maxhp;
	this->floorID=floorID;
	this->x=x;
	this->y=y;
	this->equippedItemTypeID=equippedItemTypeID;
}

db_proto::CharacterItem::CharacterItem()
{
}

db_proto::CharacterItem::CharacterItem(const CharacterItem &copy)
{
	*this=copy;
}

void db_proto::CharacterItem::operator =(const CharacterItem &copy)
{
	this->id=copy.id;
	this->characterID=copy.characterID;
	this->typeID=copy.typeID;
	this->num=copy.num;
}

db_proto::CharacterItem::CharacterItem(vce::VUint64 id,vce::VUint64 characterID,vce::VUint32 typeID,vce::VUint32 num)
{
	this->id=id;
	this->characterID=characterID;
	this->typeID=typeID;
	this->num=num;
}

db_proto::CharacterSkill::CharacterSkill()
{
}

db_proto::CharacterSkill::CharacterSkill(const CharacterSkill &copy)
{
	*this=copy;
}

void db_proto::CharacterSkill::operator =(const CharacterSkill &copy)
{
	this->id=copy.id;
	this->characterID=copy.characterID;
	this->typeID=copy.typeID;
	this->level=copy.level;
}

db_proto::CharacterSkill::CharacterSkill(vce::VUint64 id,vce::VUint64 characterID,vce::VUint32 typeID,vce::VUint32 level)
{
	this->id=id;
	this->characterID=characterID;
	this->typeID=typeID;
	this->level=level;
}

db_proto::PlayerLock::PlayerLock()
{
}

db_proto::PlayerLock::PlayerLock(const PlayerLock &copy)
{
	*this=copy;
}

void db_proto::PlayerLock::operator =(const PlayerLock &copy)
{
	this->playerID=copy.playerID;
	this->state=copy.state;
	this->ownerServerID=copy.ownerServerID;
}

db_proto::PlayerLock::PlayerLock(vce::VUint64 playerID,vce::VUint8 state,vce::VUint32 ownerServerID)
{
	this->playerID=playerID;
	this->state=state;
	this->ownerServerID=ownerServerID;
}

void db_proto::RecvEnable(FUNCTIONS func,bool enable)
{
	RecvFunctionEnable[func]=enable;
}

void db_proto::SendEnable(FUNCTIONS func,bool enable)
{
	SendFunctionEnable[func]=enable;
}

void db_proto::SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime)
{
	FunctionStatus[func].RecvCallLimit=limit;
	FunctionStatus[func].RecvCallLimitTime=limittime;
	FunctionStatus[func].DecreaseTime=vce::GetTime();
}

void db_proto::StatusUpdate()
{
	for(int i=0;i<FUNCTIONS_END;i++)
		FunctionStatus[i].Update();
}

void db_proto::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc ping
	case 101:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PING];
		if(!RecvFunctionEnable[FUNCTION_PING]){
			RecvException(FUNCTION_PING,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PING,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint64> timestamp(work);
		if(!vce_gen_serialize::Pull(timestamp.var,buf)){RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto::recv_ping( "<<"timestamp="<<timestamp<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_ping(timestamp);
	break;}
	default:{
		RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);
		}break;
	}
}

void db_proto::RecvException(FUNCTIONS id,RECVERROR code)
{
	if(log)
		*log<<vce::NowTimeString()<<" db_proto::RecvException(function="<<id<<",code="<<code<<") close...\r\n";
	ForceClose();
}

void db_proto::Connected()
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" db_proto::Connected ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void db_proto::Closed(vce::VCE_CLOSEREASON)
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" db_proto::Closed ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void db_proto::Attached()
{
	vce::Codec::Attached();
}

bool db_proto::send_ping(vce::VUint64 timestamp)
{
	if(!SendFunctionEnable[FUNCTION_PING])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=101;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(timestamp,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto::send_ping( "<<"timestamp="<<timestamp<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void db_proto_server::Attached()
{
	db_proto::Attached();
}

void db_proto_server::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc getNewID
	case 102:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GETNEWID];
		if(!RecvFunctionEnable[FUNCTION_GETNEWID]){
			RecvException(FUNCTION_GETNEWID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GETNEWID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> num(work);
		if(!vce_gen_serialize::Pull(num.var,buf)){RecvException(FUNCTION_GETNEWID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GETNEWID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_getNewID( "<<"num="<<num<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_getNewID(num);
	break;}
	//recvfunc put_Player
	case 104:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYER];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYER]){
			RecvException(FUNCTION_PUT_PLAYER,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYER,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_Player));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<Player> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_Player( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_Player(sessionID,data);
	break;}
	//recvfunc get_Player_by_id
	case 106:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_BY_ID]){
			RecvException(FUNCTION_GET_PLAYER_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_Player_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_by_id(sessionID,id);
	break;}
	//recvfunc get_Player_ranking_by_id
	case 108:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_RANKING_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ID]){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_Player_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_ranking_by_id(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_Player_by_accountName
	case 110:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME]){
			RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_string*(50)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *accountName=(char*)work;
		work+=sizeof(char)*(50);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)accountName,buf,sizeof(char)*(50))){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_Player_by_accountName( "<<"sessionID="<<sessionID<<" "<<","<<"accountName=\""<<accountName<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_by_accountName(sessionID,accountName);
	break;}
	//recvfunc get_Player_ranking_by_accountName
	case 112:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME]){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_Player_ranking_by_accountName( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_ranking_by_accountName(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc put_PlayerCharacter
	case 114:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERCHARACTER];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERCHARACTER]){
			RecvException(FUNCTION_PUT_PLAYERCHARACTER,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERCHARACTER,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerCharacter));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERCHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerCharacter> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERCHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERCHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_PlayerCharacter( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerCharacter(sessionID,data);
	break;}
	//recvfunc get_PlayerCharacter_by_id
	case 116:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_ID]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_id(sessionID,id);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_id
	case 118:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_id(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_PlayerCharacter_by_playerID
	case 120:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> playerID(work);
		if(!vce_gen_serialize::Pull(playerID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"playerID="<<playerID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_playerID(sessionID,playerID);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_playerID
	case 122:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_ranking_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_playerID(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_PlayerCharacter_by_name
	case 124:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_NAME];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_NAME]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_string*(50)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *name=(char*)work;
		work+=sizeof(char)*(50);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)name,buf,sizeof(char)*(50))){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_by_name( "<<"sessionID="<<sessionID<<" "<<","<<"name=\""<<name<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_name(sessionID,name);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_name
	case 126:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_ranking_by_name( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_name(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_PlayerCharacter_by_level
	case 128:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_word));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint16> level(work);
		if(!vce_gen_serialize::Pull(level.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_by_level( "<<"sessionID="<<sessionID<<" "<<","<<"level="<<level<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_level(sessionID,level);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_level
	case 130:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerCharacter_ranking_by_level( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_level(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc put_CharacterItem
	case 132:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_CHARACTERITEM];
		if(!RecvFunctionEnable[FUNCTION_PUT_CHARACTERITEM]){
			RecvException(FUNCTION_PUT_CHARACTERITEM,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_CHARACTERITEM,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_CharacterItem));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_CHARACTERITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<CharacterItem> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_CHARACTERITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_CHARACTERITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_CharacterItem( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_CharacterItem(sessionID,data);
	break;}
	//recvfunc get_CharacterItem_by_id
	case 134:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_ID]){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterItem_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_by_id(sessionID,id);
	break;}
	//recvfunc get_CharacterItem_ranking_by_id
	case 136:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID]){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterItem_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_ranking_by_id(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_CharacterItem_by_characterID
	case 138:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID]){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> characterID(work);
		if(!vce_gen_serialize::Pull(characterID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterItem_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"characterID="<<characterID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_by_characterID(sessionID,characterID);
	break;}
	//recvfunc get_CharacterItem_ranking_by_characterID
	case 140:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID]){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterItem_ranking_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_ranking_by_characterID(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc put_CharacterSkill
	case 142:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_CHARACTERSKILL];
		if(!RecvFunctionEnable[FUNCTION_PUT_CHARACTERSKILL]){
			RecvException(FUNCTION_PUT_CHARACTERSKILL,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_CHARACTERSKILL,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_CharacterSkill));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_CHARACTERSKILL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<CharacterSkill> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_CHARACTERSKILL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_CHARACTERSKILL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_CharacterSkill( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_CharacterSkill(sessionID,data);
	break;}
	//recvfunc get_CharacterSkill_by_id
	case 144:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_ID]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterSkill_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_by_id(sessionID,id);
	break;}
	//recvfunc get_CharacterSkill_ranking_by_id
	case 146:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterSkill_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_ranking_by_id(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc get_CharacterSkill_by_characterID
	case 148:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> characterID(work);
		if(!vce_gen_serialize::Pull(characterID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterSkill_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"characterID="<<characterID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_by_characterID(sessionID,characterID);
	break;}
	//recvfunc get_CharacterSkill_ranking_by_characterID
	case 150:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_CharacterSkill_ranking_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_ranking_by_characterID(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc put_PlayerLock
	case 152:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERLOCK];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERLOCK]){
			RecvException(FUNCTION_PUT_PLAYERLOCK,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERLOCK,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerLock));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerLock> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERLOCK,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_PlayerLock( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerLock(sessionID,data);
	break;}
	//recvfunc get_PlayerLock_by_playerID
	case 154:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID]){
			RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> playerID(work);
		if(!vce_gen_serialize::Pull(playerID.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerLock_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"playerID="<<playerID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerLock_by_playerID(sessionID,playerID);
	break;}
	//recvfunc get_PlayerLock_ranking_by_playerID
	case 156:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID]){
			RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<SortType> sorttype(work);
		if(!vce_gen_serialize::Pull(sorttype.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> limit(work);
		if(!vce_gen_serialize::Pull(limit.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> offset(work);
		if(!vce_gen_serialize::Pull(offset.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_get_PlayerLock_ranking_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerLock_ranking_by_playerID(sessionID,sorttype,limit,offset);
	break;}
	//recvfunc put_PlayerLock_if_state
	case 158:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERLOCK_IF_STATE];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERLOCK_IF_STATE]){
			RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerLock)+AdjustAlign(size_of_byte));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerLock> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint8> state_test(work);
		if(!vce_gen_serialize::Pull(state_test.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_server::recv_put_PlayerLock_if_state( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<","<<"state_test="<<state_test<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerLock_if_state(sessionID,data,state_test);
	break;}
	default:{
		db_proto::Parsed(p,sz);
		}break;
	}
}

bool db_proto_server::send_getNewIDResult(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
	if(!SendFunctionEnable[FUNCTION_GETNEWIDRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_qword*(MAXIDSET)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=103;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(NULL==idarray||idarray_qt>MAXIDSET)return false;
	if(!vce_gen_serialize::Push(idarray_qt,buf))return false;
	for(vce::VUint32 i=0;i<idarray_qt;i++)
		if(!vce_gen_serialize::Push(idarray[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_getNewIDResult( "<<"idarray["<<(unsigned int)idarray_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_Player_result(vce::VUint32 sessionID,ResultCode result,Player data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYER_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=105;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_Player_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_Player_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=107;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_Player_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_Player_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=109;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_Player_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_Player_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=111;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_Player_by_accountName_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_Player_ranking_by_accountName_result(vce::VUint32 sessionID,ResultCode result,const Player *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=113;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_Player_ranking_by_accountName_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_PlayerCharacter_result(vce::VUint32 sessionID,ResultCode result,PlayerCharacter data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERCHARACTER_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=115;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_PlayerCharacter_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=117;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=119;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=121;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=123;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_ranking_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=125;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_by_name_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_ranking_by_name_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=127;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_ranking_by_name_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=129;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_by_level_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerCharacter_ranking_by_level_result(vce::VUint32 sessionID,ResultCode result,const PlayerCharacter *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=131;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerCharacter_ranking_by_level_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_CharacterItem_result(vce::VUint32 sessionID,ResultCode result,CharacterItem data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_CHARACTERITEM_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=133;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_CharacterItem_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterItem_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=135;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterItem_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterItem_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=137;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterItem_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterItem_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=139;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterItem_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterItem_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterItem *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=141;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterItem_ranking_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_CharacterSkill_result(vce::VUint32 sessionID,ResultCode result,CharacterSkill data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_CHARACTERSKILL_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=143;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_CharacterSkill_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterSkill_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=145;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterSkill_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterSkill_ranking_by_id_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=147;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterSkill_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterSkill_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=149;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterSkill_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_CharacterSkill_ranking_by_characterID_result(vce::VUint32 sessionID,ResultCode result,const CharacterSkill *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=151;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_CharacterSkill_ranking_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_PlayerLock_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERLOCK_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=153;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_PlayerLock_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerLock_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=155;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerLock_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_get_PlayerLock_ranking_by_playerID_result(vce::VUint32 sessionID,ResultCode result,const PlayerLock *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=157;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_get_PlayerLock_ranking_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_server::send_put_PlayerLock_if_state_result(vce::VUint32 sessionID,ResultCode result,PlayerLock data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=159;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_server::send_put_PlayerLock_if_state_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void db_proto_client::Attached()
{
	db_proto::Attached();
}

void db_proto_client::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc getNewIDResult
	case 103:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GETNEWIDRESULT];
		if(!RecvFunctionEnable[FUNCTION_GETNEWIDRESULT]){
			RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_qword*(MAXIDSET)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::VUint32 idarray_qt;
		if(!vce_gen_serialize::Pull(idarray_qt,buf)){RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(idarray_qt>(MAXIDSET)){RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<vce::VUint64> idarray(work,idarray_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<idarray_qt;i++)
			if(!vce_gen_serialize::Pull(idarray.var[i],buf)){RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GETNEWIDRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_getNewIDResult( "<<"idarray["<<(unsigned int)idarray_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_getNewIDResult(idarray,idarray_qt);
	break;}
	//recvfunc put_Player_result
	case 105:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYER_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYER_RESULT]){
			RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<Player> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_Player_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_Player_result(sessionID,result,data);
	break;}
	//recvfunc get_Player_by_id_result
	case 107:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<Player> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_Player_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_Player_ranking_by_id_result
	case 109:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<Player> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_Player_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_ranking_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_Player_by_accountName_result
	case 111:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT]){
			RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<Player> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_Player_by_accountName_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_by_accountName_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_Player_ranking_by_accountName_result
	case 113:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT]){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_Player*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<Player> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_Player_ranking_by_accountName_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_Player_ranking_by_accountName_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc put_PlayerCharacter_result
	case 115:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERCHARACTER_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERCHARACTER_RESULT]){
			RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerCharacter> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERCHARACTER_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_PlayerCharacter_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerCharacter_result(sessionID,result,data);
	break;}
	//recvfunc get_PlayerCharacter_by_id_result
	case 117:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_id_result
	case 119:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_by_playerID_result
	case 121:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_playerID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_playerID_result
	case 123:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_ranking_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_playerID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_by_name_result
	case 125:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_by_name_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_name_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_name_result
	case 127:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_ranking_by_name_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_name_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_by_level_result
	case 129:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_by_level_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_by_level_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerCharacter_ranking_by_level_result
	case 131:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT]){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerCharacter*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerCharacter> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerCharacter_ranking_by_level_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerCharacter_ranking_by_level_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc put_CharacterItem_result
	case 133:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_CHARACTERITEM_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_CHARACTERITEM_RESULT]){
			RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<CharacterItem> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_CHARACTERITEM_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_CharacterItem_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_CharacterItem_result(sessionID,result,data);
	break;}
	//recvfunc get_CharacterItem_by_id_result
	case 135:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterItem> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterItem_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterItem_ranking_by_id_result
	case 137:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterItem> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterItem_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_ranking_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterItem_by_characterID_result
	case 139:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterItem> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterItem_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_by_characterID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterItem_ranking_by_characterID_result
	case 141:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterItem> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterItem_ranking_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterItem_ranking_by_characterID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc put_CharacterSkill_result
	case 143:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_CHARACTERSKILL_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_CHARACTERSKILL_RESULT]){
			RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<CharacterSkill> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_CHARACTERSKILL_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_CharacterSkill_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_CharacterSkill_result(sessionID,result,data);
	break;}
	//recvfunc get_CharacterSkill_by_id_result
	case 145:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterSkill> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterSkill_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterSkill_ranking_by_id_result
	case 147:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterSkill> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterSkill_ranking_by_id_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_ranking_by_id_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterSkill_by_characterID_result
	case 149:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterSkill> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterSkill_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_by_characterID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_CharacterSkill_ranking_by_characterID_result
	case 151:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT]){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterSkill*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterSkill> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_CharacterSkill_ranking_by_characterID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_CharacterSkill_ranking_by_characterID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc put_PlayerLock_result
	case 153:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERLOCK_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERLOCK_RESULT]){
			RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerLock> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERLOCK_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_PlayerLock_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerLock_result(sessionID,result,data);
	break;}
	//recvfunc get_PlayerLock_by_playerID_result
	case 155:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerLock> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERLOCK_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerLock_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerLock_by_playerID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc get_PlayerLock_ranking_by_playerID_result
	case 157:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT]){
			RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<PlayerLock> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_get_PlayerLock_ranking_by_playerID_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_get_PlayerLock_ranking_by_playerID_result(sessionID,result,data,data_qt);
	break;}
	//recvfunc put_PlayerLock_if_state_result
	case 159:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT];
		if(!RecvFunctionEnable[FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT]){
			RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_PlayerLock));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> sessionID(work);
		if(!vce_gen_serialize::Pull(sessionID.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<PlayerLock> data(work);
		if(!vce_gen_serialize::Pull(data.var,buf)){RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PUT_PLAYERLOCK_IF_STATE_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->db_proto_client::recv_put_PlayerLock_if_state_result( "<<"sessionID="<<sessionID<<" "<<","<<"result"<<","<<"data"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_put_PlayerLock_if_state_result(sessionID,result,data);
	break;}
	default:{
		db_proto::Parsed(p,sz);
		}break;
	}
}

bool db_proto_client::send_getNewID(vce::VUint32 num)
{
	if(!SendFunctionEnable[FUNCTION_GETNEWID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=102;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(num,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_getNewID( "<<"num="<<num<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_Player(vce::VUint32 sessionID,Player data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYER])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_Player);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=104;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_Player( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_Player_by_id(vce::VUint32 sessionID,vce::VUint64 id)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=106;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_Player_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_Player_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=108;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_Player_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_Player_by_accountName(vce::VUint32 sessionID,const char *accountName)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_BY_ACCOUNTNAME])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_string*(50)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=110;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(accountName,buf,50))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_Player_by_accountName( "<<"sessionID="<<sessionID<<" "<<","<<"accountName=\""<<accountName<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_Player_ranking_by_accountName(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYER_RANKING_BY_ACCOUNTNAME])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=112;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_Player_ranking_by_accountName( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_PlayerCharacter(vce::VUint32 sessionID,PlayerCharacter data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERCHARACTER])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerCharacter);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=114;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_PlayerCharacter( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_by_id(vce::VUint32 sessionID,vce::VUint64 id)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=116;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=118;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_PLAYERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=120;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(playerID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"playerID="<<playerID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_PLAYERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=122;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_ranking_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_by_name(vce::VUint32 sessionID,const char *name)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_NAME])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_string*(50)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=124;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(name,buf,50))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_by_name( "<<"sessionID="<<sessionID<<" "<<","<<"name=\""<<name<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_ranking_by_name(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_NAME])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=126;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_ranking_by_name( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_by_level(vce::VUint32 sessionID,vce::VUint16 level)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_BY_LEVEL])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_word);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=128;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(level,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_by_level( "<<"sessionID="<<sessionID<<" "<<","<<"level="<<level<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerCharacter_ranking_by_level(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERCHARACTER_RANKING_BY_LEVEL])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=130;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerCharacter_ranking_by_level( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_CharacterItem(vce::VUint32 sessionID,CharacterItem data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_CHARACTERITEM])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_CharacterItem);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=132;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_CharacterItem( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterItem_by_id(vce::VUint32 sessionID,vce::VUint64 id)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=134;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterItem_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterItem_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=136;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterItem_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterItem_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_BY_CHARACTERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=138;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(characterID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterItem_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"characterID="<<characterID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterItem_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERITEM_RANKING_BY_CHARACTERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=140;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterItem_ranking_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_CharacterSkill(vce::VUint32 sessionID,CharacterSkill data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_CHARACTERSKILL])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_CharacterSkill);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=142;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_CharacterSkill( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterSkill_by_id(vce::VUint32 sessionID,vce::VUint64 id)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=144;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterSkill_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterSkill_ranking_by_id(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_ID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=146;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterSkill_ranking_by_id( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterSkill_by_characterID(vce::VUint32 sessionID,vce::VUint64 characterID)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_BY_CHARACTERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=148;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(characterID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterSkill_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"characterID="<<characterID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_CharacterSkill_ranking_by_characterID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_CHARACTERSKILL_RANKING_BY_CHARACTERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=150;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_CharacterSkill_ranking_by_characterID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_PlayerLock(vce::VUint32 sessionID,PlayerLock data)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERLOCK])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerLock);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=152;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_PlayerLock( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerLock_by_playerID(vce::VUint32 sessionID,vce::VUint64 playerID)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERLOCK_BY_PLAYERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=154;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(playerID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerLock_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"playerID="<<playerID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_get_PlayerLock_ranking_by_playerID(vce::VUint32 sessionID,SortType sorttype,vce::VUint32 limit,vce::VUint32 offset)
{
	if(!SendFunctionEnable[FUNCTION_GET_PLAYERLOCK_RANKING_BY_PLAYERID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_SortType)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=156;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(sorttype,buf))return false;
	if(!vce_gen_serialize::Push(limit,buf))return false;
	if(!vce_gen_serialize::Push(offset,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_get_PlayerLock_ranking_by_playerID( "<<"sessionID="<<sessionID<<" "<<","<<"sorttype"<<","<<"limit="<<limit<<" "<<","<<"offset="<<offset<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool db_proto_client::send_put_PlayerLock_if_state(vce::VUint32 sessionID,PlayerLock data,vce::VUint8 state_test)
{
	if(!SendFunctionEnable[FUNCTION_PUT_PLAYERLOCK_IF_STATE])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_PlayerLock)+AdjustAlign(size_of_byte);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=158;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(sessionID,buf))return false;
	if(!vce_gen_serialize::Push(data,buf))return false;
	if(!vce_gen_serialize::Push(state_test,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-db_proto_client::send_put_PlayerLock_if_state( "<<"sessionID="<<sessionID<<" "<<","<<"data"<<","<<"state_test="<<state_test<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}


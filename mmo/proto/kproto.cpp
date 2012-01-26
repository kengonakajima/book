#include "kproto.h"

#define AdjustAlign(n) ((((std::size_t)n)+3)/4*4)

k_proto::k_proto()
{
	log=NULL;
	for(int i=0;i<FUNCTIONS_END;i++)
	{
		SendFunctionEnable[i]=true;
		RecvFunctionEnable[i]=true;
	}
}

k_proto::_FunctionStatus::_FunctionStatus()
{
	RecvCallCount=0;
	RecvCallLimit=0;
	RecvCallLimitTime=0;
}

void k_proto::_FunctionStatus::Update()
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


k_proto::CharacterStatus::CharacterStatus()
{
}

k_proto::CharacterStatus::CharacterStatus(const CharacterStatus &copy)
{
	*this=copy;
}

void k_proto::CharacterStatus::operator =(const CharacterStatus &copy)
{
	vce::strcpy_s(this->name,sizeof(this->name),copy.name);
	this->hp=copy.hp;
	this->maxhp=copy.maxhp;
	this->level=copy.level;
	this->exp=copy.exp;
	this->mapID=copy.mapID;
	this->x=copy.x;
	this->y=copy.y;
}

k_proto::CharacterStatus::CharacterStatus(const char *name,vce::VUint32 hp,vce::VUint32 maxhp,vce::VUint32 level,vce::VUint32 exp,vce::VUint32 mapID,vce::VUint32 x,vce::VUint32 y)
{
	vce::strcpy_s(this->name,sizeof(this->name),name);
	this->hp=hp;
	this->maxhp=maxhp;
	this->level=level;
	this->exp=exp;
	this->mapID=mapID;
	this->x=x;
	this->y=y;
}

k_proto::CharacterItem::CharacterItem()
{
}

k_proto::CharacterItem::CharacterItem(const CharacterItem &copy)
{
	*this=copy;
}

void k_proto::CharacterItem::operator =(const CharacterItem &copy)
{
	this->slotID=copy.slotID;
	this->typeID=copy.typeID;
	this->num=copy.num;
}

k_proto::CharacterItem::CharacterItem(vce::VUint32 slotID,vce::VUint32 typeID,vce::VUint32 num)
{
	this->slotID=slotID;
	this->typeID=typeID;
	this->num=num;
}

void k_proto::RecvEnable(FUNCTIONS func,bool enable)
{
	RecvFunctionEnable[func]=enable;
}

void k_proto::SendEnable(FUNCTIONS func,bool enable)
{
	SendFunctionEnable[func]=enable;
}

void k_proto::SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime)
{
	FunctionStatus[func].RecvCallLimit=limit;
	FunctionStatus[func].RecvCallLimitTime=limittime;
	FunctionStatus[func].DecreaseTime=vce::GetTime();
}

void k_proto::StatusUpdate()
{
	for(int i=0;i<FUNCTIONS_END;i++)
		FunctionStatus[i].Update();
}

void k_proto::Parsed(const vce::VUint8 *p,size_t sz)
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
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto::recv_ping( "<<"timestamp="<<timestamp<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_ping(timestamp);
	break;}
	default:{
		RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);
		}break;
	}
}

void k_proto::RecvException(FUNCTIONS id,RECVERROR code)
{
	if(log)
		*log<<vce::NowTimeString()<<" k_proto::RecvException(function="<<id<<",code="<<code<<") close...\r\n";
	ForceClose();
}

void k_proto::Connected()
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" k_proto::Connected ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void k_proto::Closed(vce::VCE_CLOSEREASON)
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" k_proto::Closed ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void k_proto::Attached()
{
	vce::Codec::Attached();
}

bool k_proto::send_ping(vce::VUint64 timestamp)
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
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto::send_ping( "<<"timestamp="<<timestamp<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void k_proto_server::Attached()
{
	k_proto::Attached();
}

void k_proto_server::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc signup
	case 102:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SIGNUP];
		if(!RecvFunctionEnable[FUNCTION_SIGNUP]){
			RecvException(FUNCTION_SIGNUP,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SIGNUP,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_string*(100)));
		vce::VCE::AutoPop autopop(api);
		char *accountname=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)accountname,buf,sizeof(char)*(100))){RecvException(FUNCTION_SIGNUP,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *password=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)password,buf,sizeof(char)*(100))){RecvException(FUNCTION_SIGNUP,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SIGNUP,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_signup( "<<"accountname=\""<<accountname<<"\" "<<","<<"password=\""<<password<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_signup(accountname,password);
	break;}
	//recvfunc authentication
	case 104:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_AUTHENTICATION];
		if(!RecvFunctionEnable[FUNCTION_AUTHENTICATION]){
			RecvException(FUNCTION_AUTHENTICATION,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_AUTHENTICATION,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_string*(100)));
		vce::VCE::AutoPop autopop(api);
		char *accountname=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)accountname,buf,sizeof(char)*(100))){RecvException(FUNCTION_AUTHENTICATION,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *password=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)password,buf,sizeof(char)*(100))){RecvException(FUNCTION_AUTHENTICATION,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_AUTHENTICATION,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_authentication( "<<"accountname=\""<<accountname<<"\" "<<","<<"password=\""<<password<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_authentication(accountname,password);
	break;}
	//recvfunc viewmode
	case 106:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_VIEWMODE];
		if(!RecvFunctionEnable[FUNCTION_VIEWMODE]){
			RecvException(FUNCTION_VIEWMODE,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_VIEWMODE,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_byte));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> floorID(work);
		if(!vce_gen_serialize::Pull(floorID.var,buf)){RecvException(FUNCTION_VIEWMODE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> x(work);
		if(!vce_gen_serialize::Pull(x.var,buf)){RecvException(FUNCTION_VIEWMODE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> y(work);
		if(!vce_gen_serialize::Pull(y.var,buf)){RecvException(FUNCTION_VIEWMODE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint8> mode(work);
		if(!vce_gen_serialize::Pull(mode.var,buf)){RecvException(FUNCTION_VIEWMODE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_VIEWMODE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_viewmode( "<<"floorID="<<floorID<<" "<<","<<"x="<<x<<" "<<","<<"y="<<y<<" "<<","<<"mode="<<mode<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_viewmode(floorID,x,y,mode);
	break;}
	//recvfunc listCharacter
	case 108:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LISTCHARACTER];
		if(!RecvFunctionEnable[FUNCTION_LISTCHARACTER]){
			RecvException(FUNCTION_LISTCHARACTER,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LISTCHARACTER,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LISTCHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_listCharacter( "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_listCharacter();
	break;}
	//recvfunc createCharacter
	case 110:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CREATECHARACTER];
		if(!RecvFunctionEnable[FUNCTION_CREATECHARACTER]){
			RecvException(FUNCTION_CREATECHARACTER,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CREATECHARACTER,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_string*(100)));
		vce::VCE::AutoPop autopop(api);
		char *characterName=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)characterName,buf,sizeof(char)*(100))){RecvException(FUNCTION_CREATECHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CREATECHARACTER,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_createCharacter( "<<"characterName=\""<<characterName<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_createCharacter(characterName);
	break;}
	//recvfunc login
	case 112:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LOGIN];
		if(!RecvFunctionEnable[FUNCTION_LOGIN]){
			RecvException(FUNCTION_LOGIN,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LOGIN,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_string*(100)));
		vce::VCE::AutoPop autopop(api);
		char *characterName=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)characterName,buf,sizeof(char)*(100))){RecvException(FUNCTION_LOGIN,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LOGIN,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_login( "<<"characterName=\""<<characterName<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_login(characterName);
	break;}
	//recvfunc logout
	case 114:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LOGOUT];
		if(!RecvFunctionEnable[FUNCTION_LOGOUT]){
			RecvException(FUNCTION_LOGOUT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LOGOUT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LOGOUT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_logout( "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_logout();
	break;}
	//recvfunc chat
	case 115:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CHAT];
		if(!RecvFunctionEnable[FUNCTION_CHAT]){
			RecvException(FUNCTION_CHAT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CHAT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_string*(2000)));
		vce::VCE::AutoPop autopop(api);
		char *text=(char*)work;
		work+=sizeof(char)*(2000);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)text,buf,sizeof(char)*(2000))){RecvException(FUNCTION_CHAT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CHAT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_chat( "<<"text=\""<<text<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_chat(text);
	break;}
	//recvfunc move
	case 117:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_MOVE];
		if(!RecvFunctionEnable[FUNCTION_MOVE]){
			RecvException(FUNCTION_MOVE,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_MOVE,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> toX(work);
		if(!vce_gen_serialize::Pull(toX.var,buf)){RecvException(FUNCTION_MOVE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> toY(work);
		if(!vce_gen_serialize::Pull(toY.var,buf)){RecvException(FUNCTION_MOVE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_MOVE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_move( "<<"toX="<<toX<<" "<<","<<"toY="<<toY<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_move(toX,toY);
	break;}
	//recvfunc characterStatus
	case 120:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CHARACTERSTATUS];
		if(!RecvFunctionEnable[FUNCTION_CHARACTERSTATUS]){
			RecvException(FUNCTION_CHARACTERSTATUS,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CHARACTERSTATUS,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_CHARACTERSTATUS,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CHARACTERSTATUS,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_characterStatus( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_characterStatus(movableID);
	break;}
	//recvfunc useSkill
	case 122:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_USESKILL];
		if(!RecvFunctionEnable[FUNCTION_USESKILL]){
			RecvException(FUNCTION_USESKILL,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_USESKILL,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> skillID(work);
		if(!vce_gen_serialize::Pull(skillID.var,buf)){RecvException(FUNCTION_USESKILL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_USESKILL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_useSkill( "<<"skillID="<<skillID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_useSkill(skillID);
	break;}
	//recvfunc attack
	case 124:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_ATTACK];
		if(!RecvFunctionEnable[FUNCTION_ATTACK]){
			RecvException(FUNCTION_ATTACK,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_ATTACK,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_ATTACK,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_ATTACK,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_attack( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_attack(movableID);
	break;}
	//recvfunc item
	case 126:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_ITEM];
		if(!RecvFunctionEnable[FUNCTION_ITEM]){
			RecvException(FUNCTION_ITEM,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_ITEM,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_ITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_item( "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_item();
	break;}
	//recvfunc equip
	case 128:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_EQUIP];
		if(!RecvFunctionEnable[FUNCTION_EQUIP]){
			RecvException(FUNCTION_EQUIP,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_EQUIP,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> slotID(work);
		if(!vce_gen_serialize::Pull(slotID.var,buf)){RecvException(FUNCTION_EQUIP,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_EQUIP,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_equip( "<<"slotID="<<slotID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_equip(slotID);
	break;}
	//recvfunc landscape
	case 130:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LANDSCAPE];
		if(!RecvFunctionEnable[FUNCTION_LANDSCAPE]){
			RecvException(FUNCTION_LANDSCAPE,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LANDSCAPE,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> floorID(work);
		if(!vce_gen_serialize::Pull(floorID.var,buf)){RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> x1(work);
		if(!vce_gen_serialize::Pull(x1.var,buf)){RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> y1(work);
		if(!vce_gen_serialize::Pull(y1.var,buf)){RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> x2(work);
		if(!vce_gen_serialize::Pull(x2.var,buf)){RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> y2(work);
		if(!vce_gen_serialize::Pull(y2.var,buf)){RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LANDSCAPE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_landscape( "<<"floorID="<<floorID<<" "<<","<<"x1="<<x1<<" "<<","<<"y1="<<y1<<" "<<","<<"x2="<<x2<<" "<<","<<"y2="<<y2<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_landscape(floorID,x1,y1,x2,y2);
	break;}
	//recvfunc useItem
	case 132:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_USEITEM];
		if(!RecvFunctionEnable[FUNCTION_USEITEM]){
			RecvException(FUNCTION_USEITEM,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_USEITEM,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> slotID(work);
		if(!vce_gen_serialize::Pull(slotID.var,buf)){RecvException(FUNCTION_USEITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_USEITEM,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_useItem( "<<"slotID="<<slotID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_useItem(slotID);
	break;}
	//recvfunc buy
	case 136:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_BUY];
		if(!RecvFunctionEnable[FUNCTION_BUY]){
			RecvException(FUNCTION_BUY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_BUY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> shopMovableID(work);
		if(!vce_gen_serialize::Pull(shopMovableID.var,buf)){RecvException(FUNCTION_BUY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> shopSlotID(work);
		if(!vce_gen_serialize::Pull(shopSlotID.var,buf)){RecvException(FUNCTION_BUY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_BUY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_buy( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"shopSlotID="<<shopSlotID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_buy(shopMovableID,shopSlotID);
	break;}
	//recvfunc sell
	case 138:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SELL];
		if(!RecvFunctionEnable[FUNCTION_SELL]){
			RecvException(FUNCTION_SELL,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SELL,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> shopMovableID(work);
		if(!vce_gen_serialize::Pull(shopMovableID.var,buf)){RecvException(FUNCTION_SELL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> shopSlotID(work);
		if(!vce_gen_serialize::Pull(shopSlotID.var,buf)){RecvException(FUNCTION_SELL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SELL,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_server::recv_sell( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"shopSlotID="<<shopSlotID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_sell(shopMovableID,shopSlotID);
	break;}
	default:{
		k_proto::Parsed(p,sz);
		}break;
	}
}

bool k_proto_server::send_signupResult(ResultCode result)
{
	if(!SendFunctionEnable[FUNCTION_SIGNUPRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=103;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_signupResult( "<<"result"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_authenticationResult(ResultCode result)
{
	if(!SendFunctionEnable[FUNCTION_AUTHENTICATIONRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=105;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_authenticationResult( "<<"result"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_viewmodeResult(ResultCode result)
{
	if(!SendFunctionEnable[FUNCTION_VIEWMODERESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=107;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_viewmodeResult( "<<"result"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_listCharacterResult(ResultCode result,const CharacterStatus *stat,vce::VUint32 stat_qt)
{
	if(!SendFunctionEnable[FUNCTION_LISTCHARACTERRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterStatus*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=109;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(NULL==stat||stat_qt>100)return false;
	if(!vce_gen_serialize::Push(stat_qt,buf))return false;
	for(vce::VUint32 i=0;i<stat_qt;i++)
		if(!vce_gen_serialize::Push(stat[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_listCharacterResult( "<<"result"<<","<<"stat["<<(unsigned int)stat_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_createCharacterResult(ResultCode result)
{
	if(!SendFunctionEnable[FUNCTION_CREATECHARACTERRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=111;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_createCharacterResult( "<<"result"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_loginResult(ResultCode result,vce::VUint32 movableID)
{
	if(!SendFunctionEnable[FUNCTION_LOGINRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=113;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(result,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_loginResult( "<<"result"<<","<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_chatNotify(vce::VSint32 talkerID,const char *name,const char *text)
{
	if(!SendFunctionEnable[FUNCTION_CHATNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_string*(1000))+AdjustAlign(size_of_string*(1000));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=116;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(talkerID,buf))return false;
	if(!vce_gen_serialize::Push(name,buf,1000))return false;
	if(!vce_gen_serialize::Push(text,buf,1000))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_chatNotify( "<<"talkerID="<<talkerID<<" "<<","<<"name=\""<<name<<"\" "<<","<<"text=\""<<text<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_moveNotify(vce::VSint32 movableID,MovableType typeID,const char *name,vce::VSint32 x,vce::VSint32 y,vce::VSint32 floorID)
{
	if(!SendFunctionEnable[FUNCTION_MOVENOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_MovableType)+AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=118;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(!vce_gen_serialize::Push(typeID,buf))return false;
	if(!vce_gen_serialize::Push(name,buf,100))return false;
	if(!vce_gen_serialize::Push(x,buf))return false;
	if(!vce_gen_serialize::Push(y,buf))return false;
	if(!vce_gen_serialize::Push(floorID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_moveNotify( "<<"movableID="<<movableID<<" "<<","<<"typeID"<<","<<"name=\""<<name<<"\" "<<","<<"x="<<x<<" "<<","<<"y="<<y<<" "<<","<<"floorID="<<floorID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_disappearNotify(vce::VSint32 movableID)
{
	if(!SendFunctionEnable[FUNCTION_DISAPPEARNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=119;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_disappearNotify( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_characterStatusResult(vce::VSint32 movableID,CharacterStatus charstat)
{
	if(!SendFunctionEnable[FUNCTION_CHARACTERSTATUSRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_CharacterStatus);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=121;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(!vce_gen_serialize::Push(charstat,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_characterStatusResult( "<<"movableID="<<movableID<<" "<<","<<"charstat"<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_useSkillResult(vce::VSint32 movableID,vce::VSint32 skillID)
{
	if(!SendFunctionEnable[FUNCTION_USESKILLRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=123;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(!vce_gen_serialize::Push(skillID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_useSkillResult( "<<"movableID="<<movableID<<" "<<","<<"skillID="<<skillID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_attackNotify(vce::VSint32 attackerMovableID,vce::VSint32 attackedMovableID,vce::VSint32 damage)
{
	if(!SendFunctionEnable[FUNCTION_ATTACKNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=125;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(attackerMovableID,buf))return false;
	if(!vce_gen_serialize::Push(attackedMovableID,buf))return false;
	if(!vce_gen_serialize::Push(damage,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_attackNotify( "<<"attackerMovableID="<<attackerMovableID<<" "<<","<<"attackedMovableID="<<attackedMovableID<<" "<<","<<"damage="<<damage<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_itemNotify(const CharacterItem *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_ITEMNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_CharacterItem*(100)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=127;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(NULL==data||data_qt>100)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_itemNotify( "<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_equipResult(vce::VSint32 resultCode,vce::VSint32 movableID,vce::VSint32 itemType)
{
	if(!SendFunctionEnable[FUNCTION_EQUIPRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=129;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(resultCode,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(!vce_gen_serialize::Push(itemType,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_equipResult( "<<"resultCode="<<resultCode<<" "<<","<<"movableID="<<movableID<<" "<<","<<"itemType="<<itemType<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_landscapeResult(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2,const TileType *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_LANDSCAPERESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_TileType*(1024)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=131;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(floorID,buf))return false;
	if(!vce_gen_serialize::Push(x1,buf))return false;
	if(!vce_gen_serialize::Push(y1,buf))return false;
	if(!vce_gen_serialize::Push(x2,buf))return false;
	if(!vce_gen_serialize::Push(y2,buf))return false;
	if(NULL==data||data_qt>1024)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_landscapeResult( "<<"floorID="<<floorID<<" "<<","<<"x1="<<x1<<" "<<","<<"y1="<<y1<<" "<<","<<"x2="<<x2<<" "<<","<<"y2="<<y2<<" "<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_useItemNotify(vce::VSint32 resultCode,vce::VSint32 itemUserMovableID,vce::VSint32 itemType)
{
	if(!SendFunctionEnable[FUNCTION_USEITEMNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=133;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(resultCode,buf))return false;
	if(!vce_gen_serialize::Push(itemUserMovableID,buf))return false;
	if(!vce_gen_serialize::Push(itemType,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_useItemNotify( "<<"resultCode="<<resultCode<<" "<<","<<"itemUserMovableID="<<itemUserMovableID<<" "<<","<<"itemType="<<itemType<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_shopNotify(vce::VSint32 shopMovableID)
{
	if(!SendFunctionEnable[FUNCTION_SHOPNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=134;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(shopMovableID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_shopNotify( "<<"shopMovableID="<<shopMovableID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_shopItemNotify(vce::VSint32 shopMovableID,vce::VSint32 itemType,vce::VSint32 price)
{
	if(!SendFunctionEnable[FUNCTION_SHOPITEMNOTIFY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=135;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(shopMovableID,buf))return false;
	if(!vce_gen_serialize::Push(itemType,buf))return false;
	if(!vce_gen_serialize::Push(price,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_shopItemNotify( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"itemType="<<itemType<<" "<<","<<"price="<<price<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_buyResult(vce::VSint32 resultCode)
{
	if(!SendFunctionEnable[FUNCTION_BUYRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=137;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(resultCode,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_buyResult( "<<"resultCode="<<resultCode<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_server::send_sellResult(vce::VSint32 resultCode)
{
	if(!SendFunctionEnable[FUNCTION_SELLRESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=139;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(resultCode,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_server::send_sellResult( "<<"resultCode="<<resultCode<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void k_proto_client::Attached()
{
	k_proto::Attached();
}

void k_proto_client::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc signupResult
	case 103:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SIGNUPRESULT];
		if(!RecvFunctionEnable[FUNCTION_SIGNUPRESULT]){
			RecvException(FUNCTION_SIGNUPRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SIGNUPRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_SIGNUPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SIGNUPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_signupResult( "<<"result"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_signupResult(result);
	break;}
	//recvfunc authenticationResult
	case 105:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_AUTHENTICATIONRESULT];
		if(!RecvFunctionEnable[FUNCTION_AUTHENTICATIONRESULT]){
			RecvException(FUNCTION_AUTHENTICATIONRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_AUTHENTICATIONRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_AUTHENTICATIONRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_AUTHENTICATIONRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_authenticationResult( "<<"result"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_authenticationResult(result);
	break;}
	//recvfunc viewmodeResult
	case 107:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_VIEWMODERESULT];
		if(!RecvFunctionEnable[FUNCTION_VIEWMODERESULT]){
			RecvException(FUNCTION_VIEWMODERESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_VIEWMODERESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_VIEWMODERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_VIEWMODERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_viewmodeResult( "<<"result"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_viewmodeResult(result);
	break;}
	//recvfunc listCharacterResult
	case 109:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LISTCHARACTERRESULT];
		if(!RecvFunctionEnable[FUNCTION_LISTCHARACTERRESULT]){
			RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_CharacterStatus*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 stat_qt;
		if(!vce_gen_serialize::Pull(stat_qt,buf)){RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(stat_qt>(100)){RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterStatus> stat(work,stat_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<stat_qt;i++)
			if(!vce_gen_serialize::Pull(stat.var[i],buf)){RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LISTCHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_listCharacterResult( "<<"result"<<","<<"stat["<<(unsigned int)stat_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_listCharacterResult(result,stat,stat_qt);
	break;}
	//recvfunc createCharacterResult
	case 111:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CREATECHARACTERRESULT];
		if(!RecvFunctionEnable[FUNCTION_CREATECHARACTERRESULT]){
			RecvException(FUNCTION_CREATECHARACTERRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CREATECHARACTERRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_CREATECHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CREATECHARACTERRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_createCharacterResult( "<<"result"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_createCharacterResult(result);
	break;}
	//recvfunc loginResult
	case 113:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LOGINRESULT];
		if(!RecvFunctionEnable[FUNCTION_LOGINRESULT]){
			RecvException(FUNCTION_LOGINRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LOGINRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_ResultCode)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<ResultCode> result(work);
		if(!vce_gen_serialize::Pull(result.var,buf)){RecvException(FUNCTION_LOGINRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_LOGINRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LOGINRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_loginResult( "<<"result"<<","<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_loginResult(result,movableID);
	break;}
	//recvfunc chatNotify
	case 116:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CHATNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_CHATNOTIFY]){
			RecvException(FUNCTION_CHATNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CHATNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_string*(1000))+AdjustAlign(size_of_string*(1000)));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> talkerID(work);
		if(!vce_gen_serialize::Pull(talkerID.var,buf)){RecvException(FUNCTION_CHATNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *name=(char*)work;
		work+=sizeof(char)*(1000);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)name,buf,sizeof(char)*(1000))){RecvException(FUNCTION_CHATNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *text=(char*)work;
		work+=sizeof(char)*(1000);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)text,buf,sizeof(char)*(1000))){RecvException(FUNCTION_CHATNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CHATNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_chatNotify( "<<"talkerID="<<talkerID<<" "<<","<<"name=\""<<name<<"\" "<<","<<"text=\""<<text<<"\" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_chatNotify(talkerID,name,text);
	break;}
	//recvfunc moveNotify
	case 118:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_MOVENOTIFY];
		if(!RecvFunctionEnable[FUNCTION_MOVENOTIFY]){
			RecvException(FUNCTION_MOVENOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_MOVENOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_MovableType)+AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<MovableType> typeID(work);
		if(!vce_gen_serialize::Pull(typeID.var,buf)){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		char *name=(char*)work;
		work+=sizeof(char)*(100);
		work=(vce::VUint8*)AdjustAlign(work);
		if(!vce_gen_serialize::Pull((char*)name,buf,sizeof(char)*(100))){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> x(work);
		if(!vce_gen_serialize::Pull(x.var,buf)){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> y(work);
		if(!vce_gen_serialize::Pull(y.var,buf)){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> floorID(work);
		if(!vce_gen_serialize::Pull(floorID.var,buf)){RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_MOVENOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_moveNotify( "<<"movableID="<<movableID<<" "<<","<<"typeID"<<","<<"name=\""<<name<<"\" "<<","<<"x="<<x<<" "<<","<<"y="<<y<<" "<<","<<"floorID="<<floorID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_moveNotify(movableID,typeID,name,x,y,floorID);
	break;}
	//recvfunc disappearNotify
	case 119:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_DISAPPEARNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_DISAPPEARNOTIFY]){
			RecvException(FUNCTION_DISAPPEARNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_DISAPPEARNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_DISAPPEARNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_DISAPPEARNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_disappearNotify( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_disappearNotify(movableID);
	break;}
	//recvfunc characterStatusResult
	case 121:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_CHARACTERSTATUSRESULT];
		if(!RecvFunctionEnable[FUNCTION_CHARACTERSTATUSRESULT]){
			RecvException(FUNCTION_CHARACTERSTATUSRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_CHARACTERSTATUSRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_CharacterStatus));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_CHARACTERSTATUSRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<CharacterStatus> charstat(work);
		if(!vce_gen_serialize::Pull(charstat.var,buf)){RecvException(FUNCTION_CHARACTERSTATUSRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_CHARACTERSTATUSRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_characterStatusResult( "<<"movableID="<<movableID<<" "<<","<<"charstat"<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_characterStatusResult(movableID,charstat);
	break;}
	//recvfunc useSkillResult
	case 123:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_USESKILLRESULT];
		if(!RecvFunctionEnable[FUNCTION_USESKILLRESULT]){
			RecvException(FUNCTION_USESKILLRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_USESKILLRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_USESKILLRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> skillID(work);
		if(!vce_gen_serialize::Pull(skillID.var,buf)){RecvException(FUNCTION_USESKILLRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_USESKILLRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_useSkillResult( "<<"movableID="<<movableID<<" "<<","<<"skillID="<<skillID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_useSkillResult(movableID,skillID);
	break;}
	//recvfunc attackNotify
	case 125:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_ATTACKNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_ATTACKNOTIFY]){
			RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> attackerMovableID(work);
		if(!vce_gen_serialize::Pull(attackerMovableID.var,buf)){RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> attackedMovableID(work);
		if(!vce_gen_serialize::Pull(attackedMovableID.var,buf)){RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> damage(work);
		if(!vce_gen_serialize::Pull(damage.var,buf)){RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_ATTACKNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_attackNotify( "<<"attackerMovableID="<<attackerMovableID<<" "<<","<<"attackedMovableID="<<attackedMovableID<<" "<<","<<"damage="<<damage<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_attackNotify(attackerMovableID,attackedMovableID,damage);
	break;}
	//recvfunc itemNotify
	case 127:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_ITEMNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_ITEMNOTIFY]){
			RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_CharacterItem*(100)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(100)){RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<CharacterItem> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_ITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_itemNotify( "<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_itemNotify(data,data_qt);
	break;}
	//recvfunc equipResult
	case 129:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_EQUIPRESULT];
		if(!RecvFunctionEnable[FUNCTION_EQUIPRESULT]){
			RecvException(FUNCTION_EQUIPRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_EQUIPRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> resultCode(work);
		if(!vce_gen_serialize::Pull(resultCode.var,buf)){RecvException(FUNCTION_EQUIPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> movableID(work);
		if(!vce_gen_serialize::Pull(movableID.var,buf)){RecvException(FUNCTION_EQUIPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> itemType(work);
		if(!vce_gen_serialize::Pull(itemType.var,buf)){RecvException(FUNCTION_EQUIPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_EQUIPRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_equipResult( "<<"resultCode="<<resultCode<<" "<<","<<"movableID="<<movableID<<" "<<","<<"itemType="<<itemType<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_equipResult(resultCode,movableID,itemType);
	break;}
	//recvfunc landscapeResult
	case 131:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_LANDSCAPERESULT];
		if(!RecvFunctionEnable[FUNCTION_LANDSCAPERESULT]){
			RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_TileType*(1024)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> floorID(work);
		if(!vce_gen_serialize::Pull(floorID.var,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> x1(work);
		if(!vce_gen_serialize::Pull(x1.var,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> y1(work);
		if(!vce_gen_serialize::Pull(y1.var,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> x2(work);
		if(!vce_gen_serialize::Pull(x2.var,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> y2(work);
		if(!vce_gen_serialize::Pull(y2.var,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(1024)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<TileType> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_LANDSCAPERESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_landscapeResult( "<<"floorID="<<floorID<<" "<<","<<"x1="<<x1<<" "<<","<<"y1="<<y1<<" "<<","<<"x2="<<x2<<" "<<","<<"y2="<<y2<<" "<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_landscapeResult(floorID,x1,y1,x2,y2,data,data_qt);
	break;}
	//recvfunc useItemNotify
	case 133:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_USEITEMNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_USEITEMNOTIFY]){
			RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> resultCode(work);
		if(!vce_gen_serialize::Pull(resultCode.var,buf)){RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> itemUserMovableID(work);
		if(!vce_gen_serialize::Pull(itemUserMovableID.var,buf)){RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> itemType(work);
		if(!vce_gen_serialize::Pull(itemType.var,buf)){RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_USEITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_useItemNotify( "<<"resultCode="<<resultCode<<" "<<","<<"itemUserMovableID="<<itemUserMovableID<<" "<<","<<"itemType="<<itemType<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_useItemNotify(resultCode,itemUserMovableID,itemType);
	break;}
	//recvfunc shopNotify
	case 134:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SHOPNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_SHOPNOTIFY]){
			RecvException(FUNCTION_SHOPNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SHOPNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> shopMovableID(work);
		if(!vce_gen_serialize::Pull(shopMovableID.var,buf)){RecvException(FUNCTION_SHOPNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SHOPNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_shopNotify( "<<"shopMovableID="<<shopMovableID<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_shopNotify(shopMovableID);
	break;}
	//recvfunc shopItemNotify
	case 135:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SHOPITEMNOTIFY];
		if(!RecvFunctionEnable[FUNCTION_SHOPITEMNOTIFY]){
			RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> shopMovableID(work);
		if(!vce_gen_serialize::Pull(shopMovableID.var,buf)){RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> itemType(work);
		if(!vce_gen_serialize::Pull(itemType.var,buf)){RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VSint32> price(work);
		if(!vce_gen_serialize::Pull(price.var,buf)){RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SHOPITEMNOTIFY,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_shopItemNotify( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"itemType="<<itemType<<" "<<","<<"price="<<price<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_shopItemNotify(shopMovableID,itemType,price);
	break;}
	//recvfunc buyResult
	case 137:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_BUYRESULT];
		if(!RecvFunctionEnable[FUNCTION_BUYRESULT]){
			RecvException(FUNCTION_BUYRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_BUYRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> resultCode(work);
		if(!vce_gen_serialize::Pull(resultCode.var,buf)){RecvException(FUNCTION_BUYRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_BUYRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_buyResult( "<<"resultCode="<<resultCode<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_buyResult(resultCode);
	break;}
	//recvfunc sellResult
	case 139:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SELLRESULT];
		if(!RecvFunctionEnable[FUNCTION_SELLRESULT]){
			RecvException(FUNCTION_SELLRESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SELLRESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> resultCode(work);
		if(!vce_gen_serialize::Pull(resultCode.var,buf)){RecvException(FUNCTION_SELLRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SELLRESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->k_proto_client::recv_sellResult( "<<"resultCode="<<resultCode<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_sellResult(resultCode);
	break;}
	default:{
		k_proto::Parsed(p,sz);
		}break;
	}
}

bool k_proto_client::send_signup(const char *accountname,const char *password)
{
	if(!SendFunctionEnable[FUNCTION_SIGNUP])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_string*(100));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=102;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(accountname,buf,100))return false;
	if(!vce_gen_serialize::Push(password,buf,100))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_signup( "<<"accountname=\""<<accountname<<"\" "<<","<<"password=\""<<password<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_authentication(const char *accountname,const char *password)
{
	if(!SendFunctionEnable[FUNCTION_AUTHENTICATION])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_string*(100))+AdjustAlign(size_of_string*(100));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=104;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(accountname,buf,100))return false;
	if(!vce_gen_serialize::Push(password,buf,100))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_authentication( "<<"accountname=\""<<accountname<<"\" "<<","<<"password=\""<<password<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_viewmode(vce::VUint32 floorID,vce::VUint32 x,vce::VUint32 y,vce::VUint8 mode)
{
	if(!SendFunctionEnable[FUNCTION_VIEWMODE])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_byte);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=106;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(floorID,buf))return false;
	if(!vce_gen_serialize::Push(x,buf))return false;
	if(!vce_gen_serialize::Push(y,buf))return false;
	if(!vce_gen_serialize::Push(mode,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_viewmode( "<<"floorID="<<floorID<<" "<<","<<"x="<<x<<" "<<","<<"y="<<y<<" "<<","<<"mode="<<mode<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_listCharacter()
{
	if(!SendFunctionEnable[FUNCTION_LISTCHARACTER])
		return false;
	const std::size_t worksize=2;
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=108;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_listCharacter( "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_createCharacter(const char *characterName)
{
	if(!SendFunctionEnable[FUNCTION_CREATECHARACTER])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_string*(100));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=110;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(characterName,buf,100))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_createCharacter( "<<"characterName=\""<<characterName<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_login(const char *characterName)
{
	if(!SendFunctionEnable[FUNCTION_LOGIN])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_string*(100));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=112;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(characterName,buf,100))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_login( "<<"characterName=\""<<characterName<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_logout()
{
	if(!SendFunctionEnable[FUNCTION_LOGOUT])
		return false;
	const std::size_t worksize=2;
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=114;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_logout( "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_chat(const char *text)
{
	if(!SendFunctionEnable[FUNCTION_CHAT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_string*(2000));
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=115;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(text,buf,2000))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_chat( "<<"text=\""<<text<<"\" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_move(vce::VSint32 toX,vce::VSint32 toY)
{
	if(!SendFunctionEnable[FUNCTION_MOVE])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=117;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(toX,buf))return false;
	if(!vce_gen_serialize::Push(toY,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_move( "<<"toX="<<toX<<" "<<","<<"toY="<<toY<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_characterStatus(vce::VSint32 movableID)
{
	if(!SendFunctionEnable[FUNCTION_CHARACTERSTATUS])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=120;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_characterStatus( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_useSkill(vce::VSint32 skillID)
{
	if(!SendFunctionEnable[FUNCTION_USESKILL])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=122;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(skillID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_useSkill( "<<"skillID="<<skillID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_attack(vce::VSint32 movableID)
{
	if(!SendFunctionEnable[FUNCTION_ATTACK])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=124;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(movableID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_attack( "<<"movableID="<<movableID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_item()
{
	if(!SendFunctionEnable[FUNCTION_ITEM])
		return false;
	const std::size_t worksize=2;
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=126;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_item( "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_equip(vce::VSint32 slotID)
{
	if(!SendFunctionEnable[FUNCTION_EQUIP])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=128;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(slotID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_equip( "<<"slotID="<<slotID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_landscape(vce::VSint32 floorID,vce::VSint32 x1,vce::VSint32 y1,vce::VSint32 x2,vce::VSint32 y2)
{
	if(!SendFunctionEnable[FUNCTION_LANDSCAPE])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=130;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(floorID,buf))return false;
	if(!vce_gen_serialize::Push(x1,buf))return false;
	if(!vce_gen_serialize::Push(y1,buf))return false;
	if(!vce_gen_serialize::Push(x2,buf))return false;
	if(!vce_gen_serialize::Push(y2,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_landscape( "<<"floorID="<<floorID<<" "<<","<<"x1="<<x1<<" "<<","<<"y1="<<y1<<" "<<","<<"x2="<<x2<<" "<<","<<"y2="<<y2<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_useItem(vce::VSint32 slotID)
{
	if(!SendFunctionEnable[FUNCTION_USEITEM])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=132;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(slotID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_useItem( "<<"slotID="<<slotID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_buy(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID)
{
	if(!SendFunctionEnable[FUNCTION_BUY])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=136;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(shopMovableID,buf))return false;
	if(!vce_gen_serialize::Push(shopSlotID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_buy( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"shopSlotID="<<shopSlotID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool k_proto_client::send_sell(vce::VSint32 shopMovableID,vce::VSint32 shopSlotID)
{
	if(!SendFunctionEnable[FUNCTION_SELL])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int)+AdjustAlign(size_of_int);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=138;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(shopMovableID,buf))return false;
	if(!vce_gen_serialize::Push(shopSlotID,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-k_proto_client::send_sell( "<<"shopMovableID="<<shopMovableID<<" "<<","<<"shopSlotID="<<shopSlotID<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}


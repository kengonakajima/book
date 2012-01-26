#include "jproto.h"

#define AdjustAlign(n) ((((std::size_t)n)+3)/4*4)

j_proto::j_proto()
{
	log=NULL;
	for(int i=0;i<FUNCTIONS_END;i++)
	{
		SendFunctionEnable[i]=true;
		RecvFunctionEnable[i]=true;
	}
}

j_proto::_FunctionStatus::_FunctionStatus()
{
	RecvCallCount=0;
	RecvCallLimit=0;
	RecvCallLimitTime=0;
}

void j_proto::_FunctionStatus::Update()
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


void j_proto::RecvEnable(FUNCTIONS func,bool enable)
{
	RecvFunctionEnable[func]=enable;
}

void j_proto::SendEnable(FUNCTIONS func,bool enable)
{
	SendFunctionEnable[func]=enable;
}

void j_proto::SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime)
{
	FunctionStatus[func].RecvCallLimit=limit;
	FunctionStatus[func].RecvCallLimitTime=limittime;
	FunctionStatus[func].DecreaseTime=vce::GetTime();
}

void j_proto::StatusUpdate()
{
	for(int i=0;i<FUNCTIONS_END;i++)
		FunctionStatus[i].Update();
}

void j_proto::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc sync
	case 106:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_SYNC];
		if(!RecvFunctionEnable[FUNCTION_SYNC]){
			RecvException(FUNCTION_SYNC,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_SYNC,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_byte*(200)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> guestid(work);
		if(!vce_gen_serialize::Pull(guestid.var,buf)){RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::VUint32 data_qt;
		if(!vce_gen_serialize::Pull(data_qt,buf)){RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(data_qt>(200)){RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<vce::VUint8> data(work,data_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<data_qt;i++)
			if(!vce_gen_serialize::Pull(data.var[i],buf)){RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_SYNC,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto::recv_sync( "<<"guestid="<<guestid<<" "<<","<<"id="<<id<<" "<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_sync(guestid,id,data,data_qt);
	break;}
	//recvfunc delete
	case 107:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_DELETE];
		if(!RecvFunctionEnable[FUNCTION_DELETE]){
			RecvException(FUNCTION_DELETE,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_DELETE,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> guestid(work);
		if(!vce_gen_serialize::Pull(guestid.var,buf)){RecvException(FUNCTION_DELETE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint32> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_DELETE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_DELETE,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto::recv_delete( "<<"guestid="<<guestid<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_delete(guestid,id);
	break;}
	default:{
		RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);
		}break;
	}
}

void j_proto::RecvException(FUNCTIONS id,RECVERROR code)
{
	if(log)
		*log<<vce::NowTimeString()<<" j_proto::RecvException(function="<<id<<",code="<<code<<") close...\r\n";
	ForceClose();
}

void j_proto::Connected()
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" j_proto::Connected ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void j_proto::Closed(vce::VCE_CLOSEREASON)
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
	if(log)*log<<vce::NowTimeString()<<" j_proto::Closed ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
}

void j_proto::Attached()
{
	vce::Codec::Attached();
}

bool j_proto::send_sync(vce::VUint32 guestid,vce::VUint32 id,const vce::VUint8 *data,vce::VUint32 data_qt)
{
	if(!SendFunctionEnable[FUNCTION_SYNC])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword)+AdjustAlign(size_of_byte*(200)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=106;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(guestid,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(NULL==data||data_qt>200)return false;
	if(!vce_gen_serialize::Push(data_qt,buf))return false;
	for(vce::VUint32 i=0;i<data_qt;i++)
		if(!vce_gen_serialize::Push(data[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto::send_sync( "<<"guestid="<<guestid<<" "<<","<<"id="<<id<<" "<<","<<"data["<<(unsigned int)data_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool j_proto::send_delete(vce::VUint32 guestid,vce::VUint32 id)
{
	if(!SendFunctionEnable[FUNCTION_DELETE])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword)+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=107;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(guestid,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto::send_delete( "<<"guestid="<<guestid<<" "<<","<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void j_proto_server::Attached()
{
	j_proto::Attached();
}

void j_proto_server::Parsed(const vce::VUint8 *p,size_t sz)
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
		vce::AutoBuf<vce::VUint64> guestclock(work);
		if(!vce_gen_serialize::Pull(guestclock.var,buf)){RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto_server::recv_ping( "<<"guestclock="<<guestclock<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_ping(guestclock);
	break;}
	//recvfunc getid
	case 103:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GETID];
		if(!RecvFunctionEnable[FUNCTION_GETID]){
			RecvException(FUNCTION_GETID,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GETID,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_word));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint16> num(work);
		if(!vce_gen_serialize::Pull(num.var,buf)){RecvException(FUNCTION_GETID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GETID,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto_server::recv_getid( "<<"num="<<num<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_getid(num);
	break;}
	default:{
		j_proto::Parsed(p,sz);
		}break;
	}
}

bool j_proto_server::send_pong(vce::VUint64 hostclock,vce::VUint64 guestclock)
{
	if(!SendFunctionEnable[FUNCTION_PONG])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_qword)+AdjustAlign(size_of_qword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=102;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(hostclock,buf))return false;
	if(!vce_gen_serialize::Push(guestclock,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto_server::send_pong( "<<"hostclock="<<hostclock<<" "<<","<<"guestclock="<<guestclock<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool j_proto_server::send_getid_result(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)
{
	if(!SendFunctionEnable[FUNCTION_GETID_RESULT])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_qword*(1000)+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=104;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(NULL==idarray||idarray_qt>1000)return false;
	if(!vce_gen_serialize::Push(idarray_qt,buf))return false;
	for(vce::VUint32 i=0;i<idarray_qt;i++)
		if(!vce_gen_serialize::Push(idarray[i],buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto_server::send_getid_result( "<<"idarray["<<(unsigned int)idarray_qt<<"] "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool j_proto_server::send_guestinfo(vce::VUint32 id)
{
	if(!SendFunctionEnable[FUNCTION_GUESTINFO])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=105;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(id,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto_server::send_guestinfo( "<<"id="<<id<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void j_proto_client::Attached()
{
	j_proto::Attached();
}

void j_proto_client::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc pong
	case 102:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PONG];
		if(!RecvFunctionEnable[FUNCTION_PONG]){
			RecvException(FUNCTION_PONG,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PONG,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_qword)+AdjustAlign(size_of_qword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint64> hostclock(work);
		if(!vce_gen_serialize::Pull(hostclock.var,buf)){RecvException(FUNCTION_PONG,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoBuf<vce::VUint64> guestclock(work);
		if(!vce_gen_serialize::Pull(guestclock.var,buf)){RecvException(FUNCTION_PONG,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PONG,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto_client::recv_pong( "<<"hostclock="<<hostclock<<" "<<","<<"guestclock="<<guestclock<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_pong(hostclock,guestclock);
	break;}
	//recvfunc getid_result
	case 104:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GETID_RESULT];
		if(!RecvFunctionEnable[FUNCTION_GETID_RESULT]){
			RecvException(FUNCTION_GETID_RESULT,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GETID_RESULT,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_qword*(1000)+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::VUint32 idarray_qt;
		if(!vce_gen_serialize::Pull(idarray_qt,buf)){RecvException(FUNCTION_GETID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(idarray_qt>(1000)){RecvException(FUNCTION_GETID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		vce::AutoVariableArrayBuf<vce::VUint64> idarray(work,idarray_qt);
		work=(vce::VUint8*)AdjustAlign(work);
		for(vce::VUint32 i=0;i<idarray_qt;i++)
			if(!vce_gen_serialize::Pull(idarray.var[i],buf)){RecvException(FUNCTION_GETID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GETID_RESULT,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto_client::recv_getid_result( "<<"idarray["<<(unsigned int)idarray_qt<<"] "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_getid_result(idarray,idarray_qt);
	break;}
	//recvfunc guestinfo
	case 105:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_GUESTINFO];
		if(!RecvFunctionEnable[FUNCTION_GUESTINFO]){
			RecvException(FUNCTION_GUESTINFO,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_GUESTINFO,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VUint32> id(work);
		if(!vce_gen_serialize::Pull(id.var,buf)){RecvException(FUNCTION_GUESTINFO,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_GUESTINFO,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->j_proto_client::recv_guestinfo( "<<"id="<<id<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_guestinfo(id);
	break;}
	default:{
		j_proto::Parsed(p,sz);
		}break;
	}
}

bool j_proto_client::send_ping(vce::VUint64 guestclock)
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
	if(!vce_gen_serialize::Push(guestclock,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto_client::send_ping( "<<"guestclock="<<guestclock<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

bool j_proto_client::send_getid(vce::VUint16 num)
{
	if(!SendFunctionEnable[FUNCTION_GETID])
		return false;
	const std::size_t worksize=2+AdjustAlign(size_of_word);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=103;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(num,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-j_proto_client::send_getid( "<<"num="<<num<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}


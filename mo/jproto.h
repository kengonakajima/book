#include "vce2.h"

#pragma once
#include "vce2serialize.h"

class j_proto
	:public vce::Codec
{
protected:
	void Parsed(const vce::VUint8 *dat,size_t sz);
public://functions
	enum FUNCTIONS
	{
		FUNCTION_PING,
		FUNCTION_PONG,
		FUNCTION_GETID,
		FUNCTION_GETID_RESULT,
		FUNCTION_GUESTINFO,
		FUNCTION_SYNC,
		FUNCTION_DELETE,
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
	j_proto();
public://enum
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
public://struct
	//struct for send
public://config
	void SendEnable(FUNCTIONS func,bool enable);
	void RecvEnable(FUNCTIONS func,bool enable);
protected://recv func
	virtual void recv_sync(vce::VUint32 guestid,vce::VUint32 id,const vce::VUint8 *data,vce::VUint32 data_qt)=0;//<  
	virtual void recv_delete(vce::VUint32 guestid,vce::VUint32 id)=0;//<  
public://send func
	bool send_sync(vce::VUint32 guestid,vce::VUint32 id,const vce::VUint8 *data,vce::VUint32 data_qt);//<  
	bool send_delete(vce::VUint32 guestid,vce::VUint32 id);//<  
public://other
	static const unsigned int xml_crc=0xdf761f74;
	static const unsigned int generated_time=0x4be6edb7;
	std::ostream *log;
};

class j_proto_server
	:public j_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_ping(vce::VUint64 guestclock)=0;//<  
	virtual void recv_getid(vce::VUint16 num)=0;//<  
public://send func
	bool send_pong(vce::VUint64 hostclock,vce::VUint64 guestclock);//<  
	bool send_getid_result(const vce::VUint64 *idarray,vce::VUint32 idarray_qt);//<  
	bool send_guestinfo(vce::VUint32 id);//<  
};

class j_proto_client
	:public j_proto
{
protected:
	void Attached();
	void Parsed(const vce::VUint8 *dat,size_t sz);
protected://recv func
	virtual void recv_pong(vce::VUint64 hostclock,vce::VUint64 guestclock)=0;//<  
	virtual void recv_getid_result(const vce::VUint64 *idarray,vce::VUint32 idarray_qt)=0;//<  
	virtual void recv_guestinfo(vce::VUint32 id)=0;//<  
public://send func
	bool send_ping(vce::VUint64 guestclock);//<  
	bool send_getid(vce::VUint16 num);//<  
};

namespace vce_gen_serialize
{
}


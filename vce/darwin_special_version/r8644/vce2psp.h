
#include "vce2.h"

namespace vce
{
	//! グループ名に使える最大文字数。
	const unsigned int GROUPNAME_LEN=8;
	struct Group
	{
		unsigned char name[GROUPNAME_LEN];
	};
	/*! アドホックネットワークグループの検索
	@param groups Group 構造体配列の先頭ポインタ。検索結果が格納されます
	@param grouplen 同配列の最大数。
	@retrun 見つかったグループ数	  
	*/
	int AdhocScan(Group *groups,int grouplen);

	/*! アドホックネットワークグループの作成
	通常の Listen に加えてグループ名を設定する必要があります。
	@param ctx 有効な VCE オブジェクトへのポインタ
	@param l リスナーへのポインタ
	@param group グループ名
	@param port Ptpポート
	@return リスナーのID
	*/
	VUint32 AdhocListen(VCE *ctx,Listener *l,const unsigned char group[GROUPNAME_LEN],VUint16 port);

	/*! アドホックネットワークグループへの参加
	通常の Connect と違い、グループ名と Ptp ポートが一致している必要があります。
	@param ctx 有効な VCE オブジェクトへのポインタ
	@param s セッションへのポインタ
	@param group グループ名
	@param port Ptpポート
	@return セッションのID
	*/
	VUint32 AdhocJoin(VCE *ctx,Session *s,const unsigned char group[GROUPNAME_LEN],VUint16 port,VUint32 timeout);

	void AdhocListenerSetSocketBufSize(Listener *l,unsigned int bufsize=1024*8);
	void AdhocListenerSetSocketRexmtInt(Listener *l,unsigned int rexmt_int=500*1000);
	void AdhocListenerSetSocketRexmtCnt(Listener *l,int rexmt_cnt=20);
	int AdhocListenerGetPTPID(Listener *l);

	void AdhocSessionSetSocketBufSize(Session *s,unsigned int bufsize=1024*8);
	void AdhocSessionSetSocketRexmtInt(Session *s,unsigned int rexmt_int=500*1000);
	void AdhocSessionSetSocketRexmtCnt(Session *s,int rexmt_cnt=20);
	int AdhocSessionGetPTPID(Session *s);

	static const unsigned int PROTOCOL_TYPE_ADHOC=0x101;
	void * SelectAdhocAlgorithm(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata);

	namespace SelectSession
	{
		void * SelectAdhocSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocLZPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocZIPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocCamelliaSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocBlowfishSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocRijndaelSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocCamelliaLZPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocCamelliaZIPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocBlowfishLZPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocBlowfishZIPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocRijndaelLZPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
		void * SelectAdhocRijndaelZIPSession(vce::VCE * api, vce::Session * s, vce::Listener * l);
	}

	enum VCE_PSP_EXCEPTION
	{
		//! アドホックネットワークの生成に失敗
		VCE_EXCEPT_ADHOC_CREATEFAIL=0x100,
		//! すでにアドホックネットワークに接続済み
		VCE_EXCEPT_ADHOC_ALREADYCONNECT,
		//! GetLocalEtherAddr 関数の呼び出しに失敗
		VCE_EXCEPT_ADHOC_GETLOCALETHERADDRFAIL,
		//! PtpListen 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_LISTENFAIL,
		//! PtpAccept 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_ACCEPTFAIL,
		
		//! GetScanInfo 関数の実行に失敗
		VCE_EXCEPT_ADHOC_GETSCANINFOFAIL,
		//! アドホックネットワークへの参加に失敗
		VCE_EXCEPT_ADHOC_JOINFAIL,
		//! PtpOpen 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_OPENFAIL,
		//! PtpConnect 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_CONNECTFAIL,
		//! PtpSend 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_SENDFAIL,
		//! PtpRecv 関数の実行に失敗
		VCE_EXCEPT_ADHOC_PTP_RECVFAIL
	};

	enum VCE_LOG_PSP
	{
		LOG_PSP_ADHOCCTL_GETSTATE,
		LOG_PSP_ADHOCCTL_SCAN,
		LOG_PSP_ADHOCCTL_GETSCANINFO,
		LOG_PSP_ADHOCCTL_GETSCANINFO_MALLOC,
		LOG_PSP_ADHOCCTL_GETSCANINFO_FREE,
		LOG_PSP_ADHOCCTL_JOIN,
		LOG_PSP_ADHOCCTL_GETPEERLIST,
		LOG_PSP_ADHOCCTL_GETPEERLIST_MALLOC,
		LOG_PSP_ADHOCCTL_GETPEERLIST_FREE,
		LOG_PSP_ADHOCCTL_CREATE,
		LOG_PSP_GETLOCALETHERADDR,
		LOG_PSP_ADHOC_PTP_OPEN,
		LOG_PSP_ADHOC_PTP_CONNECT,
		LOG_PSP_ADHOC_PTP_SEND,
		LOG_PSP_ADHOC_PTP_RECV,
		LOG_PSP_ADHOC_PTP_CLOSE,
		LOG_PSP_ADHOC_PTP_LISTEN,
		LOG_PSP_ADHOC_PTP_ACCEPT,
		LOG_PSP_ADHOCCTL_DISCONNECT,

		LOG_PSP_ADHOC_STARTSCAN,
		LOG_PSP_ADHOC_ENDSCAN,
		LOG_PSP_ADHOC_STARTJOIN,
		LOG_PSP_ADHOC_ENDJOIN,
		LOG_PSP_ADHOC_STARTCONNECT,
		LOG_PSP_ADHOC_ENDCONNECT,
		LOG_PSP_ADHOC_STARTLISTEN,
		LOG_PSP_ADHOC_ENDLISTEN,
		VCE_LOG_PSP_END
	};

	/*!	ログを受け取るコールバック関数を設定します。
	@param callback 設定する関数
	*/
	void SetPspLogCallback(void(*callback)(int type,void *ctx));

	/*! デバッグ向けの Poll 毎に出力されるログを除いた文字列化して出力
	*/
	void StringPspLogCallbackDebug(int type,void *ctx);
	/*! すべてのログを文字列化して出力
	*/
	void StringPspLogCallbackFull(int type,void *ctx);

	struct LogStructPsp_None
	{
	};

	struct LogStructPsp_VceGeneric
	{
		VCE *api;
		Session *session;
		Listener *listener;
	};


	template<int type>struct LogStruct;

	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETSTATE>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int state;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_SCAN>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETSCANINFO>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int buflen;
		const void *info;// SceNetAdhocctlScanInfo
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETSCANINFO_MALLOC>:public LogStructPsp_VceGeneric{
		const void *buf;
		int size;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETSCANINFO_FREE>:public LogStructPsp_VceGeneric{
		const void *buf;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_JOIN>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		const void *group;// SceNetAdhocctlScanInfo
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETPEERLIST>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int buflen;
		const void *info;// SceNetAdhocctlPeerInfo
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETPEERLIST_MALLOC>:public LogStructPsp_VceGeneric{
		const void *buf;
		int size;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_GETPEERLIST_FREE>:public LogStructPsp_VceGeneric{
		const void *buf;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_CREATE>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		const void *group;// SceNetAdhocctlGroupName
	};
	template<>struct LogStruct<LOG_PSP_GETLOCALETHERADDR>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		const void *mac;// SceNetEtherAddr
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_OPEN>:public LogStructPsp_VceGeneric{
		int ptp_id;// sce_errorcode
		const void *src_mac;// SceNetEtherAddr
		int src_port;
		const void *dst_mac;// SceNetEtherAddr
		int dst_port;
		int socket_bufsize;
		int socket_rexmt_int;
		int socket_rexmt_cnt;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_CONNECT>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int ptp_id;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_SEND>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int ptp_id;
		const void *buf;
		int buflen;
		int sended;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_RECV>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int ptp_id;
		const void *buf;
		int buflen;
		int recved;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_CLOSE>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
		int ptp_id;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_LISTEN>:public LogStructPsp_VceGeneric{
		int ptp_id;// sce_errorcode
		const void *src_mac;// SceNetEtherAddr
		int src_port;
		int socket_bufsize;
		int socket_rexmt_int;
		int socket_rexmt_cnt;
		int backlog;
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_PTP_ACCEPT>:public LogStructPsp_VceGeneric{
		int accept_ptp_id;// sce_errorcode
		int listen_ptp_id;
		const void *dst_mac;// SceNetEtherAddr
		int dst_port;
	};
	template<>struct LogStruct<LOG_PSP_ADHOCCTL_DISCONNECT>:public LogStructPsp_VceGeneric{
		int sce_errorcode;
	};

	template<>struct LogStruct<LOG_PSP_ADHOC_STARTSCAN>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_ENDSCAN>:public LogStructPsp_VceGeneric{
		const void *group;// SceNetAdhocctlScanInfo
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_STARTJOIN>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_ENDJOIN>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_STARTCONNECT>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_ENDCONNECT>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_STARTLISTEN>:public LogStructPsp_VceGeneric{
	};
	template<>struct LogStruct<LOG_PSP_ADHOC_ENDLISTEN>:public LogStructPsp_VceGeneric{
	};

};


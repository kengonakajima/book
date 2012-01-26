#ifndef VCE_VCE2LOG_H
#define VCE_VCE2LOG_H

#include "vce2.h"

namespace vce
{
	//!@ja ログの種類 @endja
	enum VCE_LOG_TYPE
	{
		VCE_LOG_TYPE_START,

		VCE_LOG_TYPE_SOCKET_REUSE_ADDRESS,
		VCE_LOG_TYPE_SOCKET_SET_NONBLOCKING,
		VCE_LOG_TYPE_SOCKET_ACCEPT_TCP_SOCKET,
		VCE_LOG_TYPE_SOCKET_BIND_SOCKET,
		VCE_LOG_TYPE_SOCKET_CLOSE_SOCKET,
		VCE_LOG_TYPE_SOCKET_CONNECT_SOCKET,
		VCE_LOG_TYPE_SOCKET_CREATE_TCP_SOCKET,
		VCE_LOG_TYPE_SOCKET_GET_LOCAL_ADDRESS,
		VCE_LOG_TYPE_SOCKET_LISTEN_SOCKET,
		VCE_LOG_TYPE_SOCKET_RECV_TCP_SOCKET,
		VCE_LOG_TYPE_SOCKET_SEND_TCP_SOCKET,
		VCE_LOG_TYPE_SOCKET_SET_NODELAY,
		VCE_LOG_TYPE_SOCKET_SHUTDOWN_RECV_SOCKET,
		VCE_LOG_TYPE_SOCKET_SHUTDOWN_SEND_SOCKET,
		VCE_LOG_TYPE_SOCKET_SHUTDOWN_SOCKET,
		VCE_LOG_TYPE_SOCKET_CREATE_UDP_SOCKET,
		VCE_LOG_TYPE_SOCKET_GET_SEND_QUEUE_SIZE,
		VCE_LOG_TYPE_SOCKET_SEND_TO,
		VCE_LOG_TYPE_SOCKET_RECV_FROM,
		VCE_LOG_TYPE_SOCKET_SET_MULTICAST_INTERFACE,
		VCE_LOG_TYPE_SOCKET_POLL_CHECK,

		VCE_LOG_TYPE_CODEC_SENDED_PING,
		VCE_LOG_TYPE_CODEC_RECEIVED_PING_AND_SENDED_PONG,
		VCE_LOG_TYPE_CODEC_RECEIVED_PONG,
		VCE_LOG_TYPE_CODEC_TERMINATED,
		VCE_LOG_TYPE_CODEC_TIMEOUT,
		VCE_LOG_TYPE_CODEC_SENDED_PUBLIC_KEY,
		VCE_LOG_TYPE_CODEC_GENERATED_PRIVATE_KEY,
		VCE_LOG_TYPE_CODEC_GENERATED_PUBLIC_KEY,
		VCE_LOG_TYPE_CODEC_TIMEOUT_KEY_EXCHANGE,
		VCE_LOG_TYPE_CODEC_RECEIVED_PUBLIC_KEY,
		VCE_LOG_TYPE_CODEC_GENERATED_CRYPTIC_SHARED_KEY,
		VCE_LOG_TYPE_CODEC_RECEIVED_CRYPTIC_SHARED_KEY,
		VCE_LOG_TYPE_CODEC_SENDED_ENCRYPTION_KEY,
		VCE_LOG_TYPE_CODEC_SENDED_DECRYPTION_KEY,
		VCE_LOG_TYPE_CODEC_RECEIVED_ENCRYPTION_KEY,
		VCE_LOG_TYPE_CODEC_RECEIVED_DECRYPTION_KEY,
		VCE_LOG_TYPE_CODEC_WAITING_KEY_EXCHANGE,
		VCE_LOG_TYPE_CODEC_INVALID_CODEC,

		VCE_LOG_TYPE_LISTENER_CREATED,
		VCE_LOG_TYPE_LISTENER_RETRY,
		VCE_LOG_TYPE_LISTENER_ACCEPTED_SESSION,
		VCE_LOG_TYPE_LISTENER_UDP_RECEIVED_SYN,
		VCE_LOG_TYPE_LISTENER_UDP_SENDED_SYNACK,
		VCE_LOG_TYPE_LISTENER_UDP_RECEIVED_SYNACKACK,
		VCE_LOG_TYPE_LISTENER_SENDED_RECONNECTION_REQUEST,
		VCE_LOG_TYPE_LISTENER_OVER_BACKLOG,
		VCE_LOG_TYPE_LISTENER_DESTROYED,

		VCE_LOG_TYPE_SESSION_CREATED,
		VCE_LOG_TYPE_SESSION_RECV_BUFFER_EMPTY,
		VCE_LOG_TYPE_SESSION_FORCE_CLOSE,
		VCE_LOG_TYPE_SESSION_CLOSE,
		VCE_LOG_TYPE_SESSION_WAITING_TO_CLOSE_BECAUSE_BUFFER_EMPTY,
		VCE_LOG_TYPE_SESSION_TRYING_NAME_RESOLUTION,
		VCE_LOG_TYPE_SESSION_COMPLETE_NAME_RESOLUTION,
		VCE_LOG_TYPE_SESSION_CONNECTION_CANCELED,
		VCE_LOG_TYPE_SESSION_CONNECTION_TIMEOUT,
		VCE_LOG_TYPE_SESSION_CONNECTION_RETRIED,
		VCE_LOG_TYPE_SESSION_CONNECTION_RESTARTED,
		VCE_LOG_TYPE_SESSION_CHECKING_CONNECTION_REFUSING,
		VCE_LOG_TYPE_SESSION_CONNECTION_REFUSED,
		VCE_LOG_TYPE_SESSION_CONNECTSOCKET_IGNORED_ERROR,
		VCE_LOG_TYPE_SESSION_RECV_FUNC_FAILED,
		VCE_LOG_TYPE_SESSION_CONNECTION_COMPLETE,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_SYN,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_SYNACK,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_SYNACKACK,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_RECONNECTION_REQUEST,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_HANDSHAKING_PING,
		VCE_LOG_TYPE_SESSION_UDP_COMPLETE_HANDSHAKING,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_HANDSHAKING_PING_AND_SENDED_PONG,
		VCE_LOG_TYPE_SESSION_UDP_TIMEOUT_RECEIVING,
		VCE_LOG_TYPE_SESSION_UDP_CONNECTION_COMPLETE,
		VCE_LOG_TYPE_SESSION_UDP_CONNECTED_SEQUENCE,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_CLOSE_COMMAND,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_CLOSE_COMMAND,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_PING,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_PING,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_DATA,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_DATA,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_UNRELIABLE_DATA,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_UNRELIABLE_DATA,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_FLAGMENT_COMMAND,
		VCE_LOG_TYPE_SESSION_UDP_RECEIVED_FLAGMENT_COMMAND,
		VCE_LOG_TYPE_SESSION_UDP_SENDED_ACK,
		VCE_LOG_TYPE_SESSION_DESTROYED,

		VCE_LOG_TYPE_VCE_CREATED,
		VCE_LOG_TYPE_VCE_POLLING,
		VCE_LOG_TYPE_VCE_OS_IS_USING_PORT,
		VCE_LOG_TYPE_VCE_SELECT_ALGORITHM_CALLBACK_IS_NULL,

		VCE_LOG_TYPE_VCE_POLLING_TIME_IS_TOO_LONG,
		VCE_LOG_TYPE_TIME_OF_CLOSING_SOCKET_IS_TOO_LONG,

		VCE_LOG_TYPE_START_DNS_THREAD,
		VCE_LOG_TYPE_END_DNS_THREAD,

		VCE_LOG_TYPE_PROXY_LISTENER_CATCHED_EXCEPTION_ON_CONNECTION_TO_PROXYTOOL,
		VCE_LOG_TYPE_PROXY_LISTENER_CONNECTED_TO_PROXYTOOL,
		VCE_LOG_TYPE_PROXY_LISTENER_CLOSED_CONNECTION_PROXYTOOL,

		VCE_LOG_TYPE_UPNP_PREP,
		VCE_LOG_TYPE_UPNP_CHECK_ROUTER,
		VCE_LOG_TYPE_UPNP_CONNECTING_ROUTER_PREP,
		VCE_LOG_TYPE_UPNP_CONNECTING_TO_ROUTER,
		VCE_LOG_TYPE_UPNP_GET_ROUTER_SPEC,
		VCE_LOG_TYPE_UPNP_DISCONNECT_FROM_ROUTER,
		VCE_LOG_TYPE_UPNP_COMPLETE_SPEC,
		VCE_LOG_TYPE_UPNP_ACTION_CONNECTING,
		VCE_LOG_TYPE_UPNP_ACTION_WAIT_RESPONSE,
		VCE_LOG_TYPE_UPNP_ACTION_COMPLETE,
		VCE_LOG_TYPE_UPNP_FAIL,
		VCE_LOG_TYPE_UPNP_UNKNOWN_SEQUENCE,
		VCE_LOG_TYPE_UPNP_EXCEPTION,
		VCE_LOG_TYPE_UPNP_FAILED_RESPONSE,
		VCE_LOG_TYPE_UPNP_MAYBE_NOT_SUPPORT_PORTMAPPING,
		VCE_LOG_TYPE_UPNP_MAYBE_NOT_SUPPORT_QOS,

		VCE_LOG_TYPE_HTTP_SESSION_FAILED_TO_OPEN_FILE,
		VCE_LOG_TYPE_HTTP_SESSION_SUCCEEDED_TO_OPEN_FILE,
		VCE_LOG_TYPE_HTTP_LISTENER_ADDED_AUTHOR,

		VCE_LOG_TYPE_STREAM_MULTIPLEXER_PORT_IS_ALREADY_LISTENED,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_PORT_IS_NOT_LISTENED,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_WILD_IS_ALREADY_LISTENED,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_MAINSTREAM_IS_NOT_BOUND,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_MESSAGE_CANNOT_FORMATTED,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_MUXFLOWCONTROL_CANNOT_FORMATTED,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_PURGE_CLOSED_INTERNAL_ERROR,
		VCE_LOG_TYPE_STREAM_MULTIPLEXER_HAS_NO_LISTENER,
		VCE_LOG_TYPE_STREAM_CONNECTION_TO_PORT_IS_REJECTED,
		VCE_LOG_TYPE_STREAM_UNEXPECTED_MESSAGE_TO_MISSING_PORT,
		VCE_LOG_TYPE_STREAM_RECEIVED_MESSAGE_TO_CLOSE_PORT,
		VCE_LOG_TYPE_STREAM_MESSAGE_CONTENT_IS_BROKEN,

		VCE_LOG_TYPE_END
	};

	struct LogStructContext;

	/*!@ja
	ログを受け取るコールバック関数の型です。
	@param type ログの種類
	@param context ログの内容。ログの種類によって内容が違います。
	@endja
	*/
	typedef void(*LogCallbackFunc)(VCE_LOG_TYPE type, LogStructContext * context);

	/*!@ja
	ログを受け取るコールバック関数を設定します。
	@param callback 設定する関数
	@endja
	*/
	void SetLogCallback(LogCallbackFunc callback);

	/*!@ja
	過去のログ設定の互換性を保ったログを出力するコールバック関数です。
	vce::SetLogCallback() でこの関数を設定してください。
	Windows, Linux, Solaris 版ではこの関数はデフォルトで設定されていますが、
	組み込み版では使用したいときに設定を行ってください。設定を行わない場合はログコールバックはありません。
	@sa vce::SetLogCallback()
	@endja
	*/
	void DefaultLogCallbackFunc(vce::VCE_LOG_TYPE type, LogStructContext * context);

	/*!@ja
	ログの文字列化を行います。
	@param type ログの種類
	@param context ログの内容
	@endja
	*/
	vce::g_string ToStringLogCallback(VCE_LOG_TYPE type, LogStructContext * context);

	/*!@ja
	ログレベルの取得を行います。
	@param type ログの種類
	@endja
	*/
	VCE_LOGLEVEL GetLogLevel(VCE_LOG_TYPE type);

	/*!@ja
	ログのフィルタを行います。
	@param type ログの種類
	@param off true にすることで対象のログのフィルタを行います。
	@endja
	*/
	void SetLogCallbackFilter(VCE_LOG_TYPE type,bool off);

	/*!@ja
	ログのフィルタの設定を取得します。
	@param type ログの種類
	@return フィルタの有無
	@endja
	*/
	bool GetLogCallbackFilter(VCE_LOG_TYPE type);

	/*!@ja
	ログのフィルタを一括して行います。
	@param type_start ログの種類先頭
	@param type_end この値も含む
	@param off true にすることで対象のログのフィルタを行います。
	@endja
	*/
	void SetLogCallbackFilterRange(VCE_LOG_TYPE type_start,VCE_LOG_TYPE type_end,bool off);

	/*!@ja
	実行速度重視のお勧めの設定でログのフィルタを行います。
	@endja
	*/
	void SetLogCallbackFilterRecommend();


	/*!@ja
	ログ構造体
	@endja
	*/
	struct LogStructContext
	{
	};

	/*!@ja
	ログ構造体（VCEでの一般的なログの形式）
	@endja
	*/
	struct LogStruct_VceGeneric
		: public LogStructContext
	{
		//!@ja VCE オブジェクトへのポインタ。存在しない場合は NULL。 @endja
		VCE * api;
		//!@ja セッションオブジェクトへの識別子。存在しない場合は 0。 @endja
		VUint32 session_id;
		//!@ja リスナーオブジェクトへの識別子。存在しない場合は 0。 @endja
		VUint32 listener_id;

		// constructor
		LogStruct_VceGeneric();
	};

	/*!@ja
	ログの構造体（ソケット操作の一般的なログの形式）
	@endja
	*/
	struct LogStruct_SocketGeneric
		: public LogStruct_VceGeneric
	{
		//!@ja セッションやリスナーの uID 値 @endja
		vce::VUint32 uid;

		//!@ja ソケットの番号 @endja
		int socket;

		//!@ja エラーの番号。値はプラットフォームごとに異なります。 @endja
		int error;

		//!@ja 成功・失敗 @endja
		bool result;

		// constructor
		LogStruct_SocketGeneric();
	};



	/*!@ja
	ログ構造体
	@endja
	*/
	template<int type>
	struct LogStruct;


	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_REUSE_ADDRESS>
		: public LogStruct_SocketGeneric
	{
		//!@ja 再使用するかどうか @endja
		bool reuse;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SET_NONBLOCKING>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_ACCEPT_TCP_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja 接続要求が来たか @endja
		bool accepted;
		//!@ja 要求が来た場合の新しいソケット番号。accepted == false のときは -1 となります。 @endja
		int newsocket;
		//!@ja リモートアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int remote_address;
		//!@ja リモートのポート番号 @endja
		unsigned short int remote_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_BIND_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja バインドしようとしたアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
		//!@ja バインドしようとしたポート番号 @endja
		unsigned short int port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_CLOSE_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_CONNECT_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja 接続しようとしたアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
		//!@ja 接続しようとしたポート番号 @endja
		unsigned short int port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_CREATE_TCP_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_GET_LOCAL_ADDRESS>
		: public LogStruct_SocketGeneric
	{
		//!@ja 取得したローカルのアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
		//!@ja 取得したローカルのポート番号 @endja
		unsigned short int port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_LISTEN_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja TCP リッスンのバックログ数 @endja
		unsigned int backlog;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_RECV_TCP_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja 受信バッファのポインタ @endja
		const void * buffer;
		//!@ja 最大受信バイト数 @endja
		std::size_t max_size;
		//!@ja 受信したサイズ @endja
		int received_size;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SEND_TCP_SOCKET>
		: public LogStruct_SocketGeneric
	{
		//!@ja 送信バッファのポインタ @endja
		const void * buffer;
		//!@ja 送信するバイト数 @endja
		std::size_t size;
		//!@ja 実際に送信したバイト数 @endja
		std::size_t sended_size;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SET_NODELAY>
		: public LogStruct_SocketGeneric
	{
		//!@ja nodelay 設定を有効にするかしないか @endja
		bool enable;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SHUTDOWN_RECV_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SHUTDOWN_SEND_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SHUTDOWN_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_CREATE_UDP_SOCKET>
		: public LogStruct_SocketGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_GET_SEND_QUEUE_SIZE>
		: public LogStruct_SocketGeneric
	{
		//!@ja nodelay 取得したキューのサイズ @endja
		std::size_t size;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SEND_TO>
		: public LogStruct_SocketGeneric
	{
		//!@ja 送信バッファのポインタ @endja
		const void * buffer;
		//!@ja 送信する長さ @endja
		int length;
		//!@ja 送信先のアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
		//!@ja 送信先のポート番号 @endja
		unsigned short int port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_RECV_FROM>
		: public LogStruct_SocketGeneric
	{
		//!@ja 受信バッファのポインタ @endja
		void * buffer;
		//!@ja 受信する長さ @endja
		int length;
		//!@ja 受信したサイズ @endja
		int received_size;
		//!@ja 受信相手のアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
		//!@ja 受信相手のポート番号 @endja
		unsigned short int port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_POLL_CHECK>
		: public LogStruct_SocketGeneric
	{
		// ++ ものすごく大量に出る。 ++

		//!@ja ソケットの非同期状態 @endja
		int flags;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SOCKET_SET_MULTICAST_INTERFACE>
		: public LogStruct_SocketGeneric
	{
		//!@ja 参加するマルチキャストグループのアドレス。vce::AddrToString() で文字列に変換できます。 @endja
		unsigned int address;
	};


	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_SENDED_PING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_PING_AND_SENDED_PONG>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_PONG>
		: public LogStruct_VceGeneric
	{
		//!@ja ping の遅延時間 @endja
		VUint64 lag;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_TERMINATED>
		: public LogStruct_VceGeneric
	{
		/*!
		@ja Terminate された理由。TERMINATE_REASON とそのユーザ拡張が得られます。
		@sa TERMINATE_REASON
		@endja*/
		unsigned int reason;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_TIMEOUT>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_GENERATED_PRIVATE_KEY>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_GENERATED_PUBLIC_KEY>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_SENDED_PUBLIC_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 送信された公開鍵の内容 @endja
		vce::g_string public_key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_PUBLIC_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 受信された公開鍵の内容 @endja
		vce::g_string public_key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_GENERATED_CRYPTIC_SHARED_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 生成された暗号化済み共通鍵の内容 @endja
		vce::g_string shared_key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_CRYPTIC_SHARED_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 受信された暗号化済み共通鍵の内容 @endja
		vce::g_string shared_key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_WAITING_KEY_EXCHANGE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_TIMEOUT_KEY_EXCHANGE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_SENDED_ENCRYPTION_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 送信された暗号化用の秘密鍵 @endja
		vce::g_string key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_SENDED_DECRYPTION_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 送信された復号化用の秘密鍵 @endja
		vce::g_string key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_ENCRYPTION_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 受信された暗号化用の秘密鍵 @endja
		vce::g_string key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_RECEIVED_DECRYPTION_KEY>
		: public LogStruct_VceGeneric
	{
		//!@ja 受信された暗号化用の秘密鍵 @endja
		vce::g_string key;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_CODEC_INVALID_CODEC>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_CREATED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_ACCEPTED_SESSION>
		: public LogStruct_VceGeneric
	{
		//!@ja 承認したセッションのソケット番号 @endja
		int newsocket;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_RETRY>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_SENDED_RECONNECTION_REQUEST>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_OVER_BACKLOG>
		: public LogStruct_VceGeneric
	{
		//!@ja バックログ @endja
		int backlog;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_DESTROYED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CREATED>
		: public LogStruct_VceGeneric
	{
		//!@ja このセッションを生成したリスナー。自分で生成した場合は NULL となる。 @endja
		VUint32 parent_listener_id;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_DESTROYED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_RECV_BUFFER_EMPTY>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_FORCE_CLOSE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CLOSE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_WAITING_TO_CLOSE_BECAUSE_BUFFER_EMPTY>
		: public LogStruct_VceGeneric
	{
		//!@ja クローズを開始したソケット番号 @endja
		int socket;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_TRYING_NAME_RESOLUTION>
		: public LogStruct_VceGeneric
	{
		//!@ja 名前解決しようとしているホスト名 @endja
		const char * hostname;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_COMPLETE_NAME_RESOLUTION>
		: public LogStruct_VceGeneric
	{
		//!@ja 名前解決により得られた IP アドレス @endja
		vce::VUint32 address;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_CANCELED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_TIMEOUT>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_RETRIED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_RESTARTED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_REFUSED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CHECKING_CONNECTION_REFUSING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTSOCKET_IGNORED_ERROR>
		: public LogStruct_VceGeneric
	{
		//!@ja 無視したエラー @endja
		int error;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_RECV_FUNC_FAILED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_CONNECTION_COMPLETE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_SYN>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_UDP_RECEIVED_SYN>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_UDP_SENDED_SYNACK>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_SYNACK>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_SYNACKACK>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_LISTENER_UDP_RECEIVED_SYNACKACK>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_RECONNECTION_REQUEST>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_HANDSHAKING_PING>
		: public LogStruct_VceGeneric
	{
		//!@ja 送信を開始した時刻 @endja
		vce::VUint64 now_time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_CONNECTION_COMPLETE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_CONNECTED_SEQUENCE>
		: public LogStruct_VceGeneric
	{
		//!@ja 送信シーケンス番号 @endja
		unsigned int sequence_num_send;
		//!@ja 受信シーケンス番号 @endja
		unsigned int sequence_num_recv;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_COMPLETE_HANDSHAKING>
		: public LogStruct_VceGeneric
	{
		//!@ja 完了した時刻 @endja
		vce::VUint64 now_time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_HANDSHAKING_PING_AND_SENDED_PONG>
		: public LogStruct_VceGeneric
	{
		//!@ja ハンドシェイキング PING を受信し、PONG を送信した時刻 @endja
		vce::VUint64 now_time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_TIMEOUT_RECEIVING>
		: public LogStruct_VceGeneric
	{
		//!@ja タイムアウトしたときの時刻 @endja
		vce::VUint64 now_time;
		//!@ja 最後に受信したときの時刻 @endja
		vce::VUint64 last_recv_time;
		//!@ja タイムアウト時間 @endja
		vce::VUint64 timeout_time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_CLOSE_COMMAND>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_CLOSE_COMMAND>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_PING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_PING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_DATA>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_DATA>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_UNRELIABLE_DATA>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_UNRELIABLE_DATA>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_FLAGMENT_COMMAND>
		: public LogStruct_VceGeneric
	{
		//!@ja 変更するウィンドウサイズ @endja
		unsigned short int window_size;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_RECEIVED_FLAGMENT_COMMAND>
		: public LogStruct_VceGeneric
	{
		//!@ja 変更するウィンドウサイズ @endja
		unsigned short int window_size;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_SESSION_UDP_SENDED_ACK>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_VCE_CREATED>
		: public LogStruct_VceGeneric
	{
		//!@ja VCE のバージョン番号 @endja
		const char * version_number;
		//!@ja VCE のビルド番号 @endja
		const char * build_number;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_VCE_POLLING>
		: public LogStruct_VceGeneric
	{
		//!@ja 前回のログから今回までに Poll をした回数 @endja
		vce::VUint32 polling_count;
		//!@ja この VCE オブジェクトが管理しているリスナーの数 @endja
		int num_of_listeners;
		//!@ja この VCE オブジェクトが管理しているセッションの数 @endja
		int num_of_sessions;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_VCE_OS_IS_USING_PORT>
		: public LogStruct_VceGeneric
	{
		//!@ja リッスンしようとした OS が使用中のポート番号 @endja
		vce::VUint16 port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_VCE_POLLING_TIME_IS_TOO_LONG>
		: public LogStruct_VceGeneric
	{
		//!@ja 1回のポールにかかった時間 @endja
		vce::VUint64 time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_TIME_OF_CLOSING_SOCKET_IS_TOO_LONG>
		: public LogStruct_VceGeneric
	{
		//!@ja ソケットのクローズにかかった時間。
		// このログが出た場合は弊社にご報告いただけると幸いです。
		// @endja
		vce::VUint64 time;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_VCE_SELECT_ALGORITHM_CALLBACK_IS_NULL>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_PROXY_LISTENER_CATCHED_EXCEPTION_ON_CONNECTION_TO_PROXYTOOL>
		: public LogStruct_VceGeneric
	{
		//!@ja ProxyListener と ProxyTool 間での通信で発生した Exception @endja
		vce::VCE_EXCEPTION exception;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_PROXY_LISTENER_CONNECTED_TO_PROXYTOOL>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_PROXY_LISTENER_CLOSED_CONNECTION_PROXYTOOL>
		: public LogStruct_VceGeneric
	{
		//!@ja ProxyListener が ProxyTool 間での通信で切断された理由 @endja
		vce::VCE_CLOSEREASON close_reason;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_PREP>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_CHECK_ROUTER>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_CONNECTING_ROUTER_PREP>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_CONNECTING_TO_ROUTER>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_GET_ROUTER_SPEC>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_DISCONNECT_FROM_ROUTER>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_COMPLETE_SPEC>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_ACTION_CONNECTING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_ACTION_WAIT_RESPONSE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_ACTION_COMPLETE>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_FAIL>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_UNKNOWN_SEQUENCE>
		: public LogStruct_VceGeneric
	{
		//!@ja 不明なシーケンス番号 @endja
		int sequence;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_EXCEPTION>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_FAILED_RESPONSE>
		: public LogStruct_VceGeneric
	{
		//!@ja HTTP ステータス番号 @endja
		int status;

		//!@ja HTTP レスポンスの内容 @endja
		const char * response;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_MAYBE_NOT_SUPPORT_PORTMAPPING>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_UPNP_MAYBE_NOT_SUPPORT_QOS>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_HTTP_SESSION_FAILED_TO_OPEN_FILE>
		: public LogStruct_VceGeneric
	{
		//!@ja 開くのに失敗したファイル名 @endja
		const char * filename;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_HTTP_SESSION_SUCCEEDED_TO_OPEN_FILE>
		: public LogStruct_VceGeneric
	{
		//!@ja 開くのに成功したファイル名 @endja
		const char * filename;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_HTTP_LISTENER_ADDED_AUTHOR>
		: public LogStruct_VceGeneric
	{
		//!@ja 追加した認証ユーザの本当のディレクトリ名 @endja
		const char * real_directory;

		//!@ja 追加した認証ユーザのパスワード @endja
		const char * password;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_PORT_IS_ALREADY_LISTENED>
		: public LogStruct_VceGeneric
	{
		//!@ja すでに使われていて割り当てられなかったポート番号 @endja
		vce::VUint32 port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_PORT_IS_NOT_LISTENED>
		: public LogStruct_VceGeneric
	{
		//!@ja 使用していないのにリスナーから取り除こうとしたポート番号 @endja
		vce::VUint32 port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_WILD_IS_ALREADY_LISTENED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_MAINSTREAM_IS_NOT_BOUND>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_MESSAGE_CANNOT_FORMATTED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_MUXFLOWCONTROL_CANNOT_FORMATTED>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_PURGE_CLOSED_INTERNAL_ERROR>
		: public LogStruct_VceGeneric
	{
		//!@ja 追放できなかったローカルのポート番号 @endja
		vce::VUint32 local_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MULTIPLEXER_HAS_NO_LISTENER>
		: public LogStruct_VceGeneric
	{
		//!@ja リスナーがなかったローカルのポート番号 @endja
		vce::VUint32 local_port;

		//!@ja リスナーがなかったリモートのポート番号 @endja
		vce::VUint32 remote_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_CONNECTION_TO_PORT_IS_REJECTED>
		: public LogStruct_VceGeneric
	{
		//!@ja 接続が拒否されたローカルのポート番号 @endja
		vce::VUint32 local_port;

		//!@ja 接続が拒否されたリモートのポート番号 @endja
		vce::VUint32 remote_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_UNEXPECTED_MESSAGE_TO_MISSING_PORT>
		: public LogStruct_VceGeneric
	{
		//!@ja 予期しないメッセージが発生したローカルのポート番号 @endja
		vce::VUint32 local_port;

		//!@ja 予期しないメッセージが発生したリモートのポート番号 @endja
		vce::VUint32 remote_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_RECEIVED_MESSAGE_TO_CLOSE_PORT>
		: public LogStruct_VceGeneric
	{
		//!@ja 切断メッセージを受信したローカルのポート番号 @endja
		vce::VUint32 local_port;

		//!@ja 切断メッセージを受信したリモートのポート番号 @endja
		vce::VUint32 remote_port;
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_STREAM_MESSAGE_CONTENT_IS_BROKEN>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_START_DNS_THREAD>
		: public LogStruct_VceGeneric
	{
	};

	template<>
	struct LogStruct<VCE_LOG_TYPE_END_DNS_THREAD>
		: public LogStruct_VceGeneric
	{
	};




	void CallbackAllLog();

}


#endif

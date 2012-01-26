
#ifndef VCE_VCE2STREAM_H
#define VCE_VCE2STREAM_H

#include "vce2.h"

//#ifndef VCE_HAS_STREAM_EXT
//# error "Stream Extension is disable on this platform" 
//#endif

/**
 * @defgroup vcesx
 *
 * @ja 
 * ストリーム拡張 API を定義します。
 *
 * @todo 説明
 * - 目的
 * - 寿命管理
 *   - アロケータ
 * @endja 
 *
 * @{
 */
namespace vce
{

	/* ------------------------------------------------------------
	 *
	 * ストリーム拡張のフレームワークが定義するインターフェイス
	 *
	 * ------------------------------------------------------------ */

	/**
	 * @ja 参照カウントプロトコルのインターフェイス
	 */
	class Any
	{
	public:
		virtual ~Any() {}

		/** @ja 参照カウントをインクリメントします */
		virtual void Acquire() = 0;
		/** @ja 参照カウントをデクリメントします */
		virtual void Release() = 0;
	};

	/**
	 * Unreact() を実装するためのインターフェイスです。
	 */
	template<class T>
	class Unreactable
	{
	public:
		virtual void Unreact(T* source) = 0;
	};

	/**
	 * @ja
	 * VCE 内部の接続オブジェクトに対するインターフェイス。
	 * LoopReaction に対して公開されます。 
	 *
	 * @see Reaction
	 * @endja
	 */
	class LoopBinding : virtual public Any
	{
	public:
		friend class LoopReaction;

		virtual ~LoopBinding() {}

		/**
		 * @ja
		 * 該当する接続オブジェクトを破棄します。
		 * Unbind() の呼び出し後は Reaction::Unound()が通知され、接続は破棄されます。
		 * @endja
		 */
		virtual void Unbind() = 0;

		virtual void Unreact(LoopReaction* reac) = 0;
	};

	/**
	 * @ja
	 * ログ出力を支援するためのインターフェイスです。
	 * @endja
	 */
	class Inspectable
	{
	public:
		virtual ~Inspectable() {} // to supress compiler warning...
		/**
		 * ログ行のヘッダに挿入する文字列を出力します。
		 * ログを出力するオブジェクトを特定する最低限の情報を出力する必要があります。
		 *
		 * @param out 出力先ストリーム
		 */
		virtual void InspectName(std::ostream& out) = 0;
	};

	/**
	 * @ja
	 * ストリームに対する入出力バッファ。
	 * ストリーム拡張では、データ交換を送信バッファと受信バッファの対として表現しています。
	 * StreamBuffer は送受信いずれかのバッファに該当し、 StreamBinding インターフェイスを通じて公開されます。
	 *
	 * 定義されているメソッド全てが常に利用できるわけではない点には注意が必要です。
	 * 送信バッファからはデータを読み出すことができず、また受信バッファにはデータを書き込むことができません。
	 *
	 * @see StreamBinding::GetRecvBuffer()
	 * @see StreamBinding::GetSendBuffer()
	 * @see MakeMemoryStreamBuffer()
	 * @endja
	 */
	class StreamBuffer : virtual public Any
	{
	public:
		virtual ~StreamBuffer() {}

		/**
		 * @ja
		 * バッファ末尾にデータを書き込みます。Session::Send() に該当します。
		 *
		 * @param p 書き込むデータの先頭バイト
		 * @param size 書き込むデータのバイト数
		 * @return 書き込みに成功すると true。 バッファの容量が飽和すると false 
		 * @endja
		 */
		virtual bool Write(const void *p,size_t size) = 0;

		/**
		 * @ja
		 * バッファを先頭から削除します。
		 * データを読み出して解釈したあと、利用者は参照しないデータを Shrink() で削除する必要があります。
		 * Session::EraseReadbuf() に該当します。
		 *
		 * @param size 削除するバイト数。存在するバイト数以下を指定する必要があります。
		 * @endja
		 *
		 */
		virtual void Shrink(size_t size) = 0;

		/**
		 * @ja
		 * バッファ容量や、容量不足時の振舞いを指定します。
		 * 
		 * @param size 現在のバッファのバイト数
		 * @param variable バッファを可変長にするなら true。可変長バッファは容量不足にサイズが拡大されます。
		 * @param capacity 可変長バッファの最大バイト数
		 * @return 変更に失敗すると false
		 * @todo どんな場合にバッファサイズ変更は失敗するのか？
		 * @endja
		 */
		virtual bool SetSize(size_t size,bool variable,size_t capacity) = 0;

		/**
		 * @ja
		 * バッファ内部の有効なバイト列を取得します。
		 * 取得したバイト列を書き換えることはできません。読み出し目的にのみ利用できます。
		 * Session::GetReadbuf() に該当します。
		 *
		 * @param size 取得したバイト列の有効バイト数です
		 * @return バイト列へのポインタです。
		 * @endja
		 */
		virtual const unsigned char*Peek(size_t &size) const = 0;
		
		template<class T>
		const T* PeekAs(size_t* size) const
		{ 
			size_t bytes = 0;
			const T* ret = reinterpret_cast<const T*>(Peek(bytes));
			*size = bytes/sizeof(T);
			return ret;
		}

		/** @ja @return バッファに書き込み可能なバイト数 */
		virtual size_t GetAvailable() const = 0;
		/** @ja @return バッファ長をあらわすバイト数 */
		virtual size_t GetSize() const = 0;
		/** @ja @return 可変長バッファの最大容量 */
		virtual size_t GetCapacity() const = 0;
		/** @ja @return 可変長バッファなら非ゼロ */
		virtual int GetVariable() const = 0;
		/** @ja @return バッファに書き込まれているデータのバイト数 */
		virtual size_t GetUsed() const = 0;
	};

	/**
	 * @ja
	 *
	 * オブジェクトと "関連づける" ためのインターフェイスです。
	 * 寿命を通知するために用います。 Any と異り、登録したオブジェクトは
	 * Bound() と Unbound() を一度だけ呼び出します。
	 *
	 * @param B 対応づけるオブジェクト (binding) の型
	 * @endja
	 */
	template<class B>
	class Reaction
	{
	public:
		virtual ~Reaction() {}
		/**
		 * @ja
		 * B のインスタンスが関連づけれらた時に通知されます。
		 * @param source 関連付けされたオブジェクト
		 * @endja
		 */
		virtual void Bound(B* source) = 0;
		/**
		 * @ja
		 * B のインスタンスとの関連づけが外れた時に通知されます。
		 * @param source 関連付けされたオブジェクト
		 * @endja
		 */
		virtual void Unbound(B* source) = 0;
	};

	/**
	 * @ja
	 * VCE 内部からの通知をうけとるインターフェイス。
	 *
	 * VCE 内部の接続オブジェクトから Session に対する呼び出しをフックします。
	 * このインターフェイスを VCE::BindStream() に指定することで、VCE から Session への呼び出しを横取りできます。
	 * 横取りしたコールバックを Session が受け取ることはありません。
	 *
	 * @see VCE::BindStream()
	 * @endja
	 */
	class LoopReaction : public Reaction<LoopBinding>
	{
	public:
		virtual ~LoopReaction() {}

		/**
		 * @ja
		 * 接続オブジェクトが IO 処理を行うべきタイミングが通知されます。
		 * Session には直接該当する API はありませんが、
		 * 通常の Session では IO 処理の結果として Session::Recv() や Session::Closed() など 
		 * IO 関係の通知が行われます。
		 * 従って、利用者が Session のコールバックを仮想化する場合、
		 * IO 関係の通知はこの Polled() 内から行うのが適切です。
		 * @endja
		 */
		virtual void Polled(LoopBinding* source) = 0;

		/**
		 * @ja
		 * 利用者が適当な間隔で何らかの処理を行うための通知です。
		 * Session::Think() に相当します。
		 *
		 * @see Session::Think()
		 * @endja
		 */
		virtual void Engaging(LoopBinding* source) = 0;
	};

	class AnyLoopReaction  : virtual public Any,
							 public LoopReaction
	{
	public:
		virtual void Bound(LoopBinding* /*adaptor*/) { Acquire(); }
		virtual void Unbound(LoopBinding* /*source*/) { Release(); }
	};

	/**
	 * @ja
	 * 内部にある接続オブジェクトの状態をあらわすインターフェイスです。
	 * @endja
	 *
	 * @see StreamBinding::GetState()
	 */
	class StreamState : virtual public Any
	{
	public:
		/** @ja @return Session::GetState() に該当する状態 */
		virtual VCE_STATE GetState() const = 0;
		/** @ja @return Session::GetKeyexState() に該当する状態 */
		virtual Session::KEYEX_STATE GetKeyexState() const = 0;
	};

	/**
	 * @ja
	 * 外部から状態を指定できる StreamState の拡張インターフェイスです。
	 * 主に利用者が StreamState を実装する便宜ためのために用意されています。
	 * MakeMutableStreamState() でインタンスを生成することができます。
	 *
	 * @see MakeMutableStreamState()
	 * @endja
	 */
	class MutableStreamState : public StreamState
	{
	public:
		/**
		 * @ja
		 * @param s StreamState::GetState() の戻り値を指定します。
		 * @endja
		 */
        virtual void SetState(vce::VCE_STATE s) = 0;

		/**
		 * @ja
		 * @param s StreamState::GetKeyexState() の戻り値を指定します。
		 * @endja
		 */
		virtual void SetKeyexState(Session::KEYEX_STATE s) = 0;
	};

	/**
	 * @ja
	 * MutableStreamState のインスタンスを生成します。
	 *
	 * @param ma インスタンスを確保するためのメモリアロケータ
	 * @return 生成されたインスタンス
	 * @endja
	 */
	MutableStreamState* MakeMutableStreamState(MemoryAllocator* ma);

	/**
	 * @ja
	 * 接続オブジェクトの TPC/IP レベルでの属性をあらわすインターフェイスです。
	 *
	 * @see StreamBinding::GetProperties()
	 * @endja
	 */
	class StreamProperties : virtual public Any
	{
	public:
		/** @ja @return Session::GetRemoteAddress() の addr 変数に該当 */
		virtual VUint32 GetRemoteAddress() const = 0;
		/** @ja @return Session::GetRemoteAddress() の port 変数に該当 */
		virtual VUint16 GetRemotePort() const = 0;
		/** @ja @return Session::GetRemoteHostname() に該当 */
		virtual const char* GetRemoteHostname() const = 0;
		/** @ja @return Session::GetLocalAddress() の addr 変数に該当 */
		virtual VUint32 GetLocalAddress() const = 0;
		/** @ja @return Session::GetLocalAddress() の port 変数に該当 */
		virtual VUint16 GetLocalPort() const = 0;
	};


	class StreamReaction; // for cyclic ref on StreamBinding

	/**
	 * @ja
	 * データ交換を行う双方向ストリームあらわすインターフェイスです。
	 *
	 * 利用者から Session オブジェクトへの API 呼び出しをフックします。
	 * このインターフェイスを VCE::BindStream() に指定することで、
	 * 利用者が Session に対して行った API 呼び出しを横取りできます。
	 * 横取りした API を VCE が受け取ることはありません。
	 * 
	 * @see VCE::BindStream()
	 * @endja
	 */
	class StreamBinding : virtual public Any,
						  public Unreactable<StreamReaction>,
						  public Inspectable
	{
	public:
		virtual ~StreamBinding() {}

		/**
		 * @ja
		 * ストリームを切断します。
		 * Session::Close() に相当します。
		 * @endja
		 */
		virtual void Close() = 0;

		/** Close() の別名です */
		void Unbind() { Close(); }

		/**
		 * @ja
		 * ストリームを切断します。
		 * Session::ForceClose() に相当します。
		 * @endja
		 */
		virtual void ForceClose() = 0;

		/** @ja @return 内部状態をあらわすオブジェクト */
        virtual StreamState* GetState() const = 0;
		/** @ja @return 接続属性をあらわすオブジェクト */
        virtual StreamProperties* GetProperties()= 0;
		/** @ja @return 送信バッファ */
		virtual StreamBuffer* GetSendBuffer() = 0;
		/** @ja @return 受信バッファ */
		virtual StreamBuffer* GetRecvBuffer() = 0;
		/** @ja Session::SetNodelay() に該当 */
		virtual void SetNodelay() = 0;
		/** @ja 送信バッファかネットワークにデータを書き込みます */
		virtual bool Send(const void* buf, size_t size);
		/** @ja プラットホームの IO をあらわすデスクリプタです。存在しない場合は -1 を返します。 */
		virtual int GetDescriptor() const { return -1; } // return -1 for backword compatibility of subclasses.
		/** @ja Inspectable のデフォルト実装です。何もしません。 */
		virtual void InspectName(std::ostream& /*out*/) { /* */ }
	};

	/**
	 * @ja
	 * VCE 内部からの通知をうけとるインターフェイス。
	 *
	 * Reaction 同様、 VCE 内部の接続オブジェクトから Session オブジェクトに対する通知を抽象化しています。
	 * ただし IO 処理のコールバックを定義している点が Reaction とは異ります。
	 * Reaction が主に Session へのコールバックを仮想化するために用いるのに対し、
	 *
	 * StreamReaction はストリーム拡張で Session を代替するためのインターフェイスです。
	 * 継承指向の Session に対し、 StreamReaction は利用者にインターフェイス指向の API を提供します。
	 *
	 * VCE::ConnectAsStream() や Multiplexer を利用する際に定義する必要があります。
	 * 
	 * @endja
	 */
	class StreamReaction : public Reaction<StreamBinding>
	{
	public:
		/**
		 * @ja
		 * Session::Recv() に該当します。
		 * @see Session::Recv()
		 * @endja
		 */
		virtual size_t Received(StreamBinding* source, const VUint8 *p,size_t sz) = 0;

		/**
		 * @ja
		 * Session::Connected() に該当します。
		 * @see Session::Connected()
		 * @endja
		 */
		virtual void Connected(StreamBinding* source) = 0;

		/**
		 * @ja
		 * Session::Closed() に該当します。
		 * @see Session::Closed()
		 * @endja
		 */
		virtual void Closed(StreamBinding* source, VCE_CLOSEREASON type) = 0;
		/**
		 * @ja
		 * Session::Exception() に該当します。
		 * @see Session::Exception()
		 * @endja
		 */
		virtual void Caught(StreamBinding* source, VCE_EXCEPTION type) = 0;

		/**
		 * @ja
		 * Session::Think() に該当します。
		 * @see Session::Exception()
		 * @endja
		 */
		virtual void Engaging(StreamBinding* source) = 0;
	};

	class AnyStreamReaction : virtual public Any,
							  public StreamReaction
	{
	public:
		virtual void Bound(StreamBinding* /*source*/) { Acquire(); }
		virtual void Unbound(StreamBinding* /*source*/) { Release(); }
	};

	/**
	 * 何もしない StreamReaction のインスタンスを返します。
	 * 受信したバイト列はただ破棄されます。
	 *
	 * @param ma インスタンスを確保するメモリアロケータ
	 * @return StreamReaction NullStreamReaction のインスタンス
	 */
	StreamReaction* MakeNullStreamReaction(MemoryAllocator* /*ma*/);

	/**
	 * @ja 
	 * クライアントからの接続要求の到着をあらわすインターフェイスです。
	 * ListenerReaction から参照されます。
	 *
	 * StreamArrival は Any を継承しません。したがって StreamArrival の寿命は
	 * ListenerReaction::Arrived() の呼び出し中のみ有効で、延長することはできません。
	 *
	 * @see ListenerReaction::Arrived()
	 * @endja
	 */
	class StreamArrival
	{
	public:
		/** @ja @return 接続元ホストの IP アドレス */
		virtual VUint32 GetHost() const = 0;

		/** @ja @return 接続元ホストのポート番号 */
		virtual VUint32 GetPort() const = 0;

		/**
		 * @ja 
		 * 接続要求を受け付けます。
		 *
		 * @param reaction 接続の確立に応じて作られる接続ストリームを処理するオブジェクト。
		 * @endja
		 */
		virtual void Accept(StreamReaction* reaction) = 0;
	};

	/**
	 * ListenerReaction から下部の実装にアクセスするための API です。
	 * ListenerReaction に引き渡されます。
	 *
	 */
	class ListenerBinding : virtual public Any
	{
	public:
		friend class ListenerReaction;
		/**
		 * ListenerReaction を VCE から切り離します。
		 * 通常の終了シーケンスを開始し、最終的には Unbound() が呼び出されます。
		 */
		virtual void Unbind() = 0;

		/**
		 * ListenerReaction を即座に VCE から切り離します。 Unbound() は呼び出されません。
		 */
		virtual void Unreact(ListenerReaction* lisn) = 0;
		virtual VUint32 GetPort() const = 0;
	};

	/**
	 * @ja
	 * 接続要求の待ち受けを行うインターフェイスです。
	 * ストリーム拡張 API で Listener を代替します。
	 * @endja
	 */
	class ListenerReaction : public Reaction<ListenerBinding>
	{
	public:
		/**
		 * @ja
		 * 接続要求の到着を通知します。
		 * 
		 * @param incoming 到着した接続要求
		 * @endja
		 */
		virtual void Arrived(ListenerBinding* source, StreamArrival* incoming) = 0;

		virtual void Caught(ListenerBinding* source, vce::VCE_EXCEPTION type) = 0;
	};

	class AnyListenerReaction : virtual public Any,
								public ListenerReaction
	{
	public:
		virtual void Bound(ListenerBinding* /*source*/) { Acquire(); }
		virtual void Unbound(ListenerBinding* /*source*/) { Release(); }
	};

	/* ------------------------------------------------------------
	 *
	 * ストリーム拡張を利用する組込みオブジェクト
	 *
	 * ------------------------------------------------------------ */

	/**
	 * @ja
	 * オンメモリで動作する StreamBuffer の実装 MemoryStreamBuffer のインスタンスを
	 * 新たに生成します。独自の StreamBinding を実装する際の利用を想定しています。
	 *
	 *
	 * @bufsize 送受信の各バッファの初期サイズを指示するバイト数
	 * @ma インスタンスを確保するメモリアロケータ
	 * @endja
	 */
	StreamBuffer* MakeMemoryStreamBuffer(MemoryAllocator *ma, size_t bufsize);

	/**
	 * @ja
	 * オンメモリで動作する StreamBinding をあらわす拡張インターフェイスです。
	 * MemoryStreamBinding オブジェクトはメモリ上にバッファを持っています。
	 * そのためユーザは受信バッファに書き込んだり、送信バッファを読み出すことができます。
	 * 
	 * Session に基く StreamBinding は入出力でソケットを操作しますが、
	 * MemoryStreamBinding はデータを内部の送信バッファに留保します。
	 * また、内部の受信バッファへ自動的にデータが到着するとはなく、
	 * 利用者が送信バッファの中身を仮想的に作りだす必要があります。
	 *
	 *
	 * 通信を用いない単体テストでの利用などを意図しています。
	 *
	 * @see MakeMemoryStreamBinding()
	 * @endja
	 */
	class MemoryStreamBinding : public StreamBinding
	{
	public:
		/**
		 * @ja
		 * オブジェクトが切断されたかどうかを返します。
		 * StreamBinding::Close() が呼ばると true になり、
		 * MemoryStreamBinding::ClearClosing() が呼ばれると false に戻ります。
		 *
		 * @return クローズされていれば true
		 * @endja
		 */
		virtual bool IsClosing() const = 0;

		/**
		 * @ja
		 * オブジェクトの切断状態をリセットします。
		 *
		 * @see MemoryStreamBinding::IsClosing()
		 * @see StreamBinding::Close()
		 * @endja
		 */
		virtual void ClearClosing() = 0;

		/**
		 * @ja
		 * ストリームの状態をあらわすオブジェクトを返します。
		 * StreamBinding::GetState() と異り、利用者が状態を変更できます。
		 *
		 * @return 状態オブジェクト
		 * @see StreamBinding::GetState()
		 * @endja
		 */
		virtual MutableStreamState* GetMutableState() = 0;

		/**
		 * 登録されている reaction を返します。
		 *
		 * @return StreamReaction
		 */
		virtual StreamReaction* GetReaction() = 0;
		
		/**
		 * 登録されている reaction に Unboud() を通知し、登録を解除します。
		 */
		virtual void Unbind() = 0;

		/** @param ins ログ出力に利用する Inspectable を設定します */
		virtual void SetInspectable(Inspectable* ins) = 0;
		/** @return ログ出力に利用する Inspectable */
		virtual Inspectable* GetInspectable() = 0;
	};

	/**
	 * MemoryStreamBinding のインスタンスを生成します。
	 *
	 * @param ma インスタンスを確保するメモリアロケータ
	 * @return 生成したインスタンス
	 */
	MemoryStreamBinding* MakeMemoryStreamBinding(MemoryAllocator* ma, StreamReaction* reaction=0);

	/**
	 * @ja
	 *
	 * MemoryStreamBinding のインスタンスを生成します。
	 * バッファや状態を外部から与えることができます。
	 *
	 * @param ma インスタンスを確保するメモリアロケータ
	 * @param recv 受信バッファ
	 * @param send 送信バッファ
	 * @param state 状態オブジェクト
	 * @param prop 属性オブジェクト
	 * @param reaction 関連づけられる reaction。 MemoryStreamReaction など
	 *                 コールバック通知を行うオブジェクトから参照されます。
	 * @return 生成したインスタンス
	 *
	 * @endja
	 */
	MemoryStreamBinding* MakeMemoryStreamBinding(MemoryAllocator* ma, 
												 StreamBuffer* recv, StreamBuffer* send,
												 MutableStreamState* state, StreamProperties* prop,
												 StreamReaction* reaction=0);



	/**
	 * @ja
	 *
	 * BufferingStreamReaction のインスタンスを生成します。
	 * BufferingStreamReaction はバッファを持たない StreamBinding に対し
	 * バッファを提供するアタプタです。
	 *
	 * @param ma インスタンスを確保するメモリアロケータ
	 * @param reaction 関連づけられる reaction
	 * @return 生成したインスタンス
	 *
	 * @endja
	 */
	AnyStreamReaction* MakeBufferingStreamReaction(MemoryAllocator* ma, StreamReaction* reaction0);

	/**
	 * vce::Codec のように Write() で書き込んだ際のサイズを保存し、
	 * 受信にそれを復元して通知する StreamBinding の実装です。
	 */
	class PacketStreamBinding : public StreamBinding
	{
	public:
		virtual AnyStreamReaction* GetSourceReaction() = 0; 
		virtual StreamReaction* GetTargetReaction() = 0;
		virtual StreamBinding* GetSourceBinding() = 0;
	};

	/**
	 * PacketStreamBinding のインスタンスを生成します。
	 * @param ma インスタンスを確保するメモリアロケータ
	 * @return 生成したインスタンス
	 */
	PacketStreamBinding* MakePacketStreamBinding(vce::MemoryAllocator* ma, StreamReaction* reaction);

	/**
	 * @ja
	 * MemoryStreamBinding の状態を、指定した StreamReaction に通知するための Reaction 実装です。
	 * MemoryStreamBinding と併用することで、Session をオンメモリのバッファ上に仮想化することができます。
	 *
	 * MemoryStreamBinder は、 MemoryStreamBinding の状態と、
	 * 仮想的な接続元の状態を同期するように振舞います。たとえば接続元状態が VCE_STATE_CONNECTING
	 * になれば MemoryStreamBinding 自身の状態も VCE_STATE_CONNECTING や VCE_STATE_ESTABLISHED に変化し、
	 * StreamReaction::Connected() など該当する StreamReaction のコールバックを通知します。
	 * 同様に接続元状態が VCE_STATE_CLOSED となれば自身は VCE_STATE_CLOSED に遷移します。
	 *
	 * @see MakeMemoryStreamBinding()
	 * @endja
	 */
	class MemoryStreamBinder : virtual public Any
	{
	public:
		/**
		 * @ja
		 * 仮想化したストリーム上で例外を発生させます。例外は関連づけられた StreamReaction に通知されます。
		 * 例外発生後、ストリームは切断します。
		 * 
		 * @param ex 発生させる例外の種別
		 * @param cr 例外発生後に行うクローズの種別
		 * @endja
		 */
		virtual void NotifyRaised(VCE_EXCEPTION ex, VCE_CLOSEREASON cr) = 0;

		/**
		 * @ja
		 * 仮想化したストリームを切断します。 関連づけられた StreamReaction に通知を行い、
		 * クローズのシーケンスを開始します。
		 *
		 * @param cr クローズの種別
		 * @endja
		 */
		virtual void NotifyClosedWith(VCE_CLOSEREASON cr) = 0;

		/** @ja 対象の MemoryStreamBinding */
		virtual MemoryStreamBinding* GetBinding() const = 0;

		/** @ja 登録に用いる LoopReaction */
		virtual LoopReaction* GetBound() = 0;
	};

	/**
	 * @ja
	 * MemoryStreamBinder のインスタンスを生成します。
	 * 
	 * @param ma インスタンスを確保するアロケータ
	 * @param binding0 インスタンスが監視する MemoryStreamBinding
	 * @param reaction0 インスタンスが通知を行う StreamReaction
	 * @param remote_state 仮想的な通信元の状態
	 * @return 生成したインスタンス
	 * @endja
	 */
	MemoryStreamBinder* 
	MakeMemoryStreamBinder(MemoryAllocator* ma, MemoryStreamBinding* binding0, StreamState*remote_state);


	/**
	 * @ja
	 * 何もしない Reaction オブジェクトを生成します。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @return 生成したインスタンス
	 * @endja
	 */
	LoopReaction* MakeNullReaction(MemoryAllocator* ma);

	/**
	 * @ja
	 * VCE を使ったデータ交換を、Session を介してストリームフレームワークの API で利用する変換オブジェクトです。
	 * MakeSessionStreamBinder() 経由で構築した Session を VCE::Connect() などで登録すると、
	 * 登録した Session を介して 指定の StreamReaction にコールバックが通知されます。
	 * また StreamReaction に関連づけられた StreamBinding に対して IO 操作を行うと、
	 * それらの IO は Session を介して VCE に移譲されます。
	 * つまり、 StreamBinding と StreamReaction を Session でラップします。
	 *
	 * 従来どおり Session を利用してプログラミングをする場合、 SessionStreamBinder を利用する必要はありません。
	 * ストリーム拡張の API を通じて VCE を利用するために用意されています。
	 *
	 * @endja
	 */
	class SessionStreamBinder : virtual public Any
	{
	public:
		/** @ja @return VCE に登録するための Session */
		virtual Session* GetBound() = 0;
		/** @ja @return Session に対するストリーム抽象 */
		virtual StreamBinding* GetBinding() = 0;
	};

	/**
	 * @ja
	 * SesesionStreamBinder のインスタンスを生成します。
	 * ここで生成される SessionStreamBinder は Session を利用しており,
	 * Session::Recv() や Session::Send() が バイト列の入出力に利用されます。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param reaction Session に関連づけるリアクションオブジェクト
	 * @param llflags Session に設定する暗号化/複合化方式などのフラグ。Session::llflags に該当。
	 * @return 生成されたインスタンス
	 * @see Session::llflags
	 * @endja
	 */
	SessionStreamBinder* MakeSessionStreamBinder(MemoryAllocator* ma, StreamReaction* reaction,
												 unsigned int llflags=Session::LLF_NONE);

	/**
	 * @ja
	 * SesesionStreamBinder のインスタンスを生成します。
	 * ここで生成される SessionStreamBinder は Codec を利用しており,
	 * Codec::Parsed() や Codec::Merge() が バイト列の入出力に利用されます。
	 *
	 * 制限事項: 
	 * この SessionStreamBinder に与えられる StreamReaction は StreamReaction::Received() を
	 * 呼び出された際、全てのデータを消化する必要があります。これは Codec の実装に起因する制限です。
	 * また同様の制限により、GetBinding() で取得できる StreamBinding は、
	 * StreamBuffer::Peek() でバッファを参照したり, 
	 * StreamBuffer::Shrink() でバッファを削除することはできません。
	 *
	 * Codec に関連づけられた SessionStreamBinder は、サイズ付きバイト列を用いた RPC 通信に
	 * 利用されることを想定しています。
	 * より柔軟なデータ管理を実現したい場合は MakeSessionStreamBinder() を利用してください。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param reaction Session に関連づけるリアクションオブジェクト
	 * @param llflags Session に設定する暗号化/複合化方式などのフラグ。Session::llflags に該当。
	 * @return 生成されたインスタンス
	 * @see Session::llflags
	 * @endja
	 */
	SessionStreamBinder* MakeCodecStreamBinder(MemoryAllocator* ma, StreamReaction* reaction,
											   unsigned int llflags=0);

	/**
	 * @ja
	 * VCE から Listener への接続要求に ListenerReaction で応じるための変換オブジェクトです。
	 * MakeSessionListenerBinder() 経由で構築した Listener を VCE::Listen() などで登録すると、
	 * 登録した Listener を介して 指定の ListenerReaction にコールバックが通知されます。
	 *
	 * SessionStreamBinder 同様、ストリーム拡張の API を通じて VCE を利用するために用意されています。
	 * @endja
	 */
	class ListenerBinder : virtual public Any
	{
	public:
		/** @ja @return VCE に登録するための Listener */
		virtual Listener* GetBound() = 0;
	};

	/**
	 * @ja
	 * Session を作成する ListenerBinder のインスタンスを生成します。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param lisn Listener に関連づける ListenerReaction オブジェクト
	 * @param llflags オブジェクトが接続受付に構築する Session に指定する llflags
	 * @return 生成されたインスタンス
	 *
	 * @see Session::llflags
	 *
	 * @endja
	 */
	ListenerBinder* MakeSessionListenerBinder(MemoryAllocator* ma, ListenerReaction* lisn, 
											  unsigned int llflags=Session::LLF_NONE);

	/**
	 * @ja
	 * Codec を作成する ListenerBinder のインスタンスを生成します。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param lisn Listener に関連づける ListenerReaction オブジェクト
	 * @param llflags オブジェクトが接続受付に構築する Session に指定する llflags
	 * @return 生成されたインスタンス
	 *
	 * @see Session::llflags
	 *
	 * @endja
	 */
	ListenerBinder* MakeCodecListenerBinder(MemoryAllocator* ma, ListenerReaction* lisn, 
											unsigned int llflags=Session::LLF_NONE);

	/**
	 * @ja
	 * 指定した Session に結びついた StreamBinding のインスタンスを生成します。
	 * 生成した StreamBinding への API 呼び出しは、 tobound に指定した Session に移譲されます。
	 * SessionStreamBinder とは逆に、Session を StreamBinding でラップすることができます。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param tobound API の移譲先 Session
	 * @return 生成されたインスタンス
	 * @endja
	 */
	StreamBinding* MakeSessionStreamBinding(MemoryAllocator* ma, Session* tobound, Unreactable<StreamReaction>* unr);

	/**
	 * @ja
	 * 指定した Session に結びついた StreamReaction のインスタンスを生成します。
	 * 生成した StreamReaction への API 呼び出しは、 tobound に指定した Session に移譲されます。
	 * SessionStreamBinder とは逆に、Session を StreamReaction でラップすることができます。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param tobound API の移譲先 Session
	 * @return 生成されたインスタンス
	 * @endja
	 */
	AnyStreamReaction* MakeSessionStreamReaction(MemoryAllocator* ma, Session* target);

	/**
	 * @ja
	 * 指定した Listener に結びついた ListenerReaction のインスタンスを生成します。
	 * 生成した StreamReaction への API 呼び出しは、 tobound に指定した Listener に移譲されます。
	 * ListenerBinder とは逆に、Listener を ListenerReaction でラップすることができます。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param tobound API の移譲先 Listener
	 * @return 生成されたインスタンス
	 *
	 * @endja
	 */
	AnyListenerReaction* MakeSessionListenerReaction(VCE* api, Listener* tobound);

	/**
	 * @ja
	 * 指定した Session に結びついた StreamReaction のインスタンスを生成します。
	 * MakeSessionStreamReaction() とよく似ていますが、 
	 * MakeSessionReaction() で生成するのは Reaction オブジェクトです。
	 * したがって IO に関係する API は移譲されず、 Engaging() など限られた API だけが移譲されます。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @param tobound API の移譲先 Session
	 * @return 生成されたインスタンス
	 * @endja
	 */
	LoopReaction* MakeSessionReaction(MemoryAllocator* ma, Session* target);

	/**
	 * @ja
	 * 二つの Session lead と peer を短絡するためのオブジェクトです。
	 * 短絡された一方の Sesion からの送信は、もう一方に受信として通知されます。
	 * 同様に Close() も一方からもう一方へ伝播します。
	 *
	 * 短絡は、簡易形式のループバック機構です。インプロセスサーバを実現するための、
	 * 代替手段を提供します。ループバックと比べ、仕組みが局所的という利点があります。
	 * 本来の Session の動作との互換性はループバックより下がります。
	 *
	 * @endja
	 */
	class SessionShortcut : virtual public Any
	{
	public:
		/**
		 * @ja
		 * 二つの Session の間で接続を確立します。
		 * @endja
		 */
		virtual void Connect() = 0;

		/**
		 * lead の Session で例外を発生します。例外発生後、ストリームは切断されます。
		 * @param ex 発生させる例外の種別
		 * @param local_cr lead に通知される切断種別
		 * @param remote_cr peer に通知される切断種別
		 */
		virtual void RaiseToLead(VCE_EXCEPTION ex, VCE_CLOSEREASON local_cr, VCE_CLOSEREASON remote_cr) = 0;

		/**
		 * peer の Session で例外を発生します。例外発生後、ストリームは切断されます。
		 * @param ex 発生させる例外の種別
		 * @param local_cr peer に通知される切断種別
		 * @param remote_cr lead に通知される切断種別
		 */
		virtual void RaiseToPeer(VCE_EXCEPTION ex, VCE_CLOSEREASON local_cr, VCE_CLOSEREASON remote_cr) = 0;

		/** @ja @return lead 側の Session */
		virtual Session* GetLead() = 0;
		/** @ja @return peer 側の Session */
		virtual Session* GetPeer() = 0;
		/** @ja @return lead 側の Session に結びついた StreamBinding */
		virtual StreamBinding* GetLeadBinding() = 0;
		/** @ja @return peer 側の Session に結びついた StreamBinding */
		virtual StreamBinding* GetPeerBinding() = 0;
	};
	
	/**
	 * @ja
	 * SessionShortcut のインスタンスを生成します
	 * 
	 * @param ma インスタンスを確保するアロケータ
	 * @param lead lead 側で短絡する Session
	 * @param peer peer 側で短絡する Session
	 * @return 生成されたインスタンス
	 *
	 * @endja
	 */
	SessionShortcut* ShortcutSessions(VCE* api, Session* lead, Session* peer);

	/**
	 * @ja
	 * ローカルとリモートのポート対です。
	 * Multiplexer 上での接続を識別するために使います。
	 * @endja
	 */
	struct PortPair
	{
		explicit PortPair(VUint32 local0=0, VUint32 remote0=0) : local(local0), remote(remote0) {}
		/** @todo */
		VUint32 local;
		/** @todo */
		VUint32 remote;
	};

	/**
	 * @ja
	 * PortPair を std::map や std::set のキーとして登録するための比較演算子。
	 * @endja
	 */
	inline bool operator<(const PortPair& x, const PortPair& y)
	{
		if (x.local < y.local) {
			return true;
		} else if (x.local > y.local) {
			return false;
		} else if (x.remote < y.remote) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @ja
	 * Multiplexer 内部で発生する例外。 MultiplexerReaction に通知されます。
	 * 例外の種別によってはサブクラスが定義され、詳細な情報をとりだすことができます。
	 *
	 * @see Multiplexer
	 * @see MultiplexerReaction::Caught()
	 * @endja
	 */
	class MuxException
	{
	public:
		/** 
		 * @ja
		 * 例外種別
		 *
		 * @see MuxException::GetType()
		 * @endja
		 */
		enum Type
		{
			/**
			 * @ja 
			 * 要求されたポートに何も登録されていない
			 * @endja
			 */
			TYPE_MISSING_PORT,
			/**
			 * @ja
			 * 仮想経路上のデータの書式が不正 
			 * @endja
			 */
			TYPE_WRONG_MESSAGE,
			/**
			 * @ja
			 * VCE 内部での不整合を検出。発生すべきでない。
			 * @endja
			 */
			TYPE_INTERNAL,
			/**
			 * @ja 
			 * ホスト経路上で VCE の例外が発生。
			 * @see MuxVCEException
			 * @endja
			 */
			TYPE_VCE_EXCEPTION,
			/** @internal */
			TYPES
		};

		/** @internal */
		explicit MuxException(Type type0, const PortPair& where0=PortPair(0, 0)) 
			: type(type0), where(where0)
		{}

		/** @ja @return 例外種別 */
		Type GetType() const { return this->type; }
		/** @ja @return 例外の発生した仮想経路。特定できない場合は (0,0) */
		const PortPair& GetWhere() const { return this->where; }
	private:
		Type type;
		PortPair where;
	};

	/**
	 * @ja
	 * MuxException::TYPE_VCE_EXCEPTION に対応する MuxException のサブクラスです。
	 * @endja
	 */
	class MuxVCEException : public MuxException
	{
	public:
		/** @internal */
		MuxVCEException(VCE_EXCEPTION raised0)
			: MuxException(TYPE_VCE_EXCEPTION, PortPair(0, 0)), raised(raised0)
		{}

		/** @ja @return 発生した VCE 例外の種別 */
		VCE_EXCEPTION GetRaised() const { return this->raised; }
	private:
		VCE_EXCEPTION raised;
	};

	class MultiplexerReaction; // for cyclic ref by Unreactable.

	/**
	 * @ja
	 * 一つのストリーム上で複数のストリームがデータ交換をするために
	 * MakeMultiplexer() の reaction  引数に指定したホストストリームを多重化します。
	 *
	 * ストリームを多重化するオブジェクトです。多重化した個々のストリームをサブストリームと呼びます。
	 * @endja
	 */
	class Multiplexer : virtual public Any,
						public Unreactable<MultiplexerReaction>,
						public Inspectable
	{
	public:
		/**
		 * @ja
		 * 多重化した経路上での接続を要求します。
		 * 
		 * @param port 接続先のポート番号。接続元のポート番号は自動的に割り振られます。
		 * @param reaction 接続した経路を扱う StreamReaction
		 * @endja
		 */
		virtual bool ConnectTo(VUint32 port, StreamReaction* reaction) = 0;

		/**
		 * @ja
		 * 多重化した経路上での接続要求を待ち受けます。
		 * 
		 * @param port 待ち受けるローカルのポート番号
		 * @param listener 該当ポートへの接続要求を扱う ListenerReaction
		 * @endja
		 */
		virtual void ListenAt(VUint32 port, ListenerReaction* listener) = 0;

		/**
		 * @ja
		 * Multiplexer::ListenAt() で登録した接続待ちを解除します。
		 * 既に登録したポート番号を指定する必要があります。
		 *
		 * @param port 接続待ちを解除するローカルのポート番号
		 * @endja
		 */
		virtual void UnlistenAt(VUint32 port) = 0;

		/**
		 * @ja
		 * 多重化した経路上での接続要求を待ち受けます。
		 * Listen() で登録した ListenerReaction は
		 * ListenAt() で登録した待ち受けと一致しなかった場合に利用されます。
		 *
		 * @endja
		 */
		virtual void Listen(ListenerReaction* listener) = 0;

		/**
		 * @ja
		 * Multiplexer::ListenAt() で登録した全ての接続待ちを解除します。
		 *
		 * @endja
		 */
		virtual void Unlisten() = 0;

		/** @internal */
		virtual AnyStreamReaction* GetTrunkReaction() = 0;

		/** @ja @return 多重化した経路上で接続の確立したサブストリームの数 */
		virtual size_t GetSubstreamSize() const = 0;
		/** @ja @return 多重化した経路上のサブストリーム接続に対し待ち受けをしているリスナーの数 */
		virtual size_t GetSublistenerSize() const = 0;

		/**
		 * @ja
		 * ホストストリームを切断します。
		 * @endja
		 */
		virtual void Close() = 0;

		/** Close() の別名です */
		void Unbind() { Close(); }

		/**
		 * @ja
		 * ホストストリームを切断します。
		 * @endja
		 */
		virtual void ForceClose() = 0;

		/** @ja @return ホストストリームの内部状態をあらわすオブジェクト */
        virtual StreamState* GetState() const = 0;
		/** @ja @return ホストストリームの接続属性をあらわすオブジェクト */
        virtual StreamProperties* GetProperties()= 0;
		/** @ja プラットホームの IO をあらわすデスクリプタです。存在しない場合は -1 を返します。 */
		virtual int GetDescriptor() const { return -1; } // has default to keep  backword compatibility
	};

	/**
	 * @ja
	 * Multiplexer から、多重化したストリームに移譲されない通知をうけとるためのインターフェイスです。
	 * Multiplexer はホストストリームからの通知を解釈し、適切なサブストリームに配信します。
	 * MultiplexerReaction はサブストリームに配信するのが適切でない通知を受け取ります。
	 * @endja
	 */
	class MultiplexerReaction : public Reaction<Multiplexer>
	{
	public:
		/**
		 * @ja
		 * ホストストリームの接続が確立したことを通知します。
		 * SessionReaction::Connected() に該当します。
		 * @endja
		 */
		virtual void Connected(Multiplexer* source) = 0;

		/**
		 * @ja
		 * ホストストリームが切断されたことを通知します。
		 * 切断通知は各サブストリームに配信されたあと、このコールバックに通知されます。
		 * SessionReaction::Closed() に該当します。
		 *
		 * @param type 切断種別
		 * @endja
		 */
		virtual void Closed(Multiplexer* source, VCE_CLOSEREASON type) = 0;

		/**
		 * ホストストリームで発生した例外を通知します。
		 *
		 * @param ex 発生した例外
		 */
		virtual void Caught(Multiplexer* source, const MuxException& ex) = 0;

		/**
		 * @ja
		 * VCE の Poll ループから定期的に通知されます。
		 * Session::Think() に該当します。
		 * @endja
		 */
		virtual void Engaging(Multiplexer* source) = 0;
	};

	class AnyMultiplexerReaction : virtual public Any,
								   public MultiplexerReaction
	{
	public:
		virtual void Bound(Multiplexer* /*source*/) { Acquire(); }
		virtual void Unbound(Multiplexer* /*source*/) { Release(); }
	};

	/**
	 * @ja
	 * 何もしない MultiplexerReaction オブジェクトを生成します。
	 *
	 * @param ma インスタンスを確保するアロケータ
	 * @return 生成したインスタンス
	 * @endja
	 */
	MultiplexerReaction* MakeNullMultiplexerReaction(MemoryAllocator* /*ma*/);

	/**
	 * @ja
	 * Multiplexer のインスタンスを生成する
	 * 
	 * @param api Multiplexer が内部で利用する Session を登録する VCE オブジェクト
	 * @param reaction 多重化するストリームに関連づけられた StreamReaction
	 * @return 生成したインスタンス
	 * @endja
	 */
	Multiplexer* MakeMultiplexer(VCE* api, MultiplexerReaction* reaction);


	/**
	 * @ja
	 * Any の参照カウントプロトコルを実装するための補助オブジェクト。
	 * Any を思想するクラスが継承して用いることができます。
	 *
	 * @see Any
	 * @endja
	 */
	class CountedHelper
	{
	public:
		CountedHelper() : count(0) {}
		virtual ~CountedHelper() {}

		/**
		 * @ja
		 * 参照カウントを増やします。 利用するクラスが Acquire() から呼び出すと想定しています。
		 *
		 * @see Any::Acquire()
		 * @endja
		 */
		void DoAcquire() { count++; }

		/**
		 * @ja
		 * 参照カウントを減らします。利用するクラスは Release() から呼び出すと想定しています。
		 *
		 * @see Any::Release()
		 * @endja
		 */
		void DoRelease() { if (0 == --count) { delete this; } }

		/**
		 * @ja
		 * 参照カウントを減らします。引数なしの DoRelease() と異り、
		 * この関数では参照カウントがゼロになったオブジェクトの解放に
		 * 引数 a で指定したアロケータを用います。
		 *
		 * 利用者は解放するインスタンスのポインタ ptr を実クラスとして渡す必要があります。
		 * 多態した親クラスの型のとしてポインタを渡すと、メモリ確保時とアドレスが異なりエラーとなります。
		 *
		 * @param a インスタンス解放に用いるアロケータ
		 * @param ptr 解放するオブジェクト。
		 * @endja
		 */
		template<class T>
		void DoRelease(MemoryAllocator* a, T* ptr) { if (0 == --count) { ptr->~T(); a->free(ptr); } }
	private:
		CountedHelper(const CountedHelper&);
		const CountedHelper& operator=(const CountedHelper&);
	private:
		int count;
	};

	/**
	 * StreamBinding を結びつける Session サブクラスをあらわす列挙子です。
	 * @see StreamConnectTCP(), StreamListenTCP()
	 */
	enum StreamSessionTypes
	{
		/** vce::Session */
		STREAM_SESSION_SESSION,
		/** vce::Codec */
		STREAM_SESSION_CODEC,
		/* 番兵 */
		STREAM_SESSION_TYPES
	};

	/**
	 * @ja
	 *
	 * Session や Codec を介して StreamReaction を TCP ソケットに接続します。
	 * VCE::Connect() へのショートカットです。
	 *
	 * @param Session を登録する VCE オブジェクト
	 * @param Session に関連づけるコールバック
	 * @param hostname 接続先ホスト名
	 * @param port 接続先ポート
	 * @param timeout 接続のタイムアウト
	 * @param type 利用する Session の種別
	 * @param llflags Session に設定する暗号化などのフラグ
	 * @return 登録に使った Session を保持する binder
	 *
	 * @see VCE::Connect()
	 * @see Sesssion::llflags
	 *
	 * @endja
	 */
	SessionStreamBinder*
	StreamConnectTCP(VCE* api, StreamReaction *reaction, 
					 const std::string& hostname, VUint16 port,unsigned int timeout,
					 StreamSessionTypes type, unsigned int llflags=vce::Session::LLF_NONE);

	/**
	 * @ja
	 *
	 * Listener を介して ListenerReaction への TCP 接続受け付けます。
	 * VCE::Listen() へのショートカットです。
	 * 
	 * @param api Listener を登録する VCE オブジェクト
	 * @param listener Listener に登録する ListenerReaction
	 * @param port 接続待ちポート
	 * @param bindaddress 接続待ちアドレス
	 * @param type 利用する Session の種別
	 * @param llflags Session に設定する暗号化などのフラグ
	 * @return 登録に使った Listener を保持する binder
	 *
	 * @see VCE::Listen()
	 * @endja
	 */
	ListenerBinder*
	StreamListenTCP(VCE* api, ListenerReaction* listener, 
					VUint16 port,const std::string& bindaddress,
					StreamSessionTypes type, unsigned int llflags=vce::Session::LLF_NONE);


	/*
	 * dummy Reaction implementation that does nothing.
	 */
	template<class B>
	class NullReaction : public Reaction<B>
	{
	public:
		static NullReaction<B>* GetInstance()
		{
			static NullReaction<B> s_instance;
			return &s_instance;
		}

		virtual void Bound(B* source) {}
		virtual void Unbound(B* source) {}
	};

	/*
	 * copied from boost/intrusiveptr.hpp
	 * Distributed under the Boost Software License, Version 1.0. (See
	 * accompanying file LICENSE_1_0.txt or copy at
	 * http://www.boost.org/LICENSE_1_0.txt)
	 */
	template<class T> class IntrusivePtr
	{
	private:

		typedef IntrusivePtr this_type;

	public:

		typedef T element_type;

		IntrusivePtr(): p_(0)
		{
		}

		IntrusivePtr(T * p, bool add_ref = true): p_(p)
		{
			if(p_ != 0 && add_ref) IntrusivePtrAddRef(p_);
		}

		template<class U> IntrusivePtr(IntrusivePtr<U> const & rhs): p_(rhs.Get())
		{
			if(p_ != 0) IntrusivePtrAddRef(p_);
		}

		IntrusivePtr(IntrusivePtr const & rhs): p_(rhs.p_)
		{
			if(p_ != 0) IntrusivePtrAddRef(p_);
		}

		~IntrusivePtr()
		{
			if(p_ != 0) IntrusivePtrRelease(p_);
		}

		template<class U> IntrusivePtr & operator=(IntrusivePtr<U> const & rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		IntrusivePtr & operator=(IntrusivePtr const & rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		IntrusivePtr & operator=(T * rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		T * Get() const
		{
			return p_;
		}

		T & operator*() const
		{
			return *p_;
		}

		T * operator->() const
		{
			return p_;
		}

		operator bool () const
		{
			return p_ != 0;
		}

		typedef T * (this_type::*unspecified_bool_type)() const;
    
		operator unspecified_bool_type() const // never throws
		{
			return p_ == 0? 0: &this_type::Get;
		}

		bool operator! () const
		{
			return p_ == 0;
		}

		void swap(IntrusivePtr & rhs)
		{
			T * tmp = p_;
			p_ = rhs.p_;
			rhs.p_ = tmp;
		}

		this_type Clear()
		{
			this_type ret(0);
			ret.swap(*this);
			return ret;
		}

		void Unbound()
		{
			if (p_)
			{
				IntrusiveUnbound(Clear().p_);
			}
		}

		template<class S>
		void Unbound(S* source)
		{
			if (p_)
			{
				IntrusiveUnbound(Clear().p_, source);
			}
		}

		void Unreact()
		{
			if (p_)
			{
				IntrusiveUnreact(Clear().p_);
			}
		}

		template<class S>
		void Unreact(S* source)
		{
			if (p_)
			{
				IntrusiveUnreact(Clear().p_, source);
			}
		}

	private:

		T * p_;
	};

	template<class T, class U> inline bool operator<(IntrusivePtr<T> const & a, IntrusivePtr<U> const & b)
	{
		return a.Get() < b.Get();
	}

	template<class T, class U> inline bool operator==(IntrusivePtr<T> const & a, IntrusivePtr<U> const & b)
	{
		return a.Get() == b.Get();
	}

	template<class T, class U> inline bool operator!=(IntrusivePtr<T> const & a, IntrusivePtr<U> const & b)
	{
		return a.Get() != b.Get();
	}

	template<class T, class U> inline bool operator==(IntrusivePtr<T> const & a, U * b)
	{
		return a.Get() == b;
	}

	template<class T, class U> inline bool operator!=(IntrusivePtr<T> const & a, U * b)
	{
		return a.Get() != b;
	}

	template<class T, class U> inline bool operator==(T * a, IntrusivePtr<U> const & b)
	{
		return a == b.Get();
	}

	inline void IntrusivePtrAddRef(Any* ptr) { ptr->Acquire(); }
	inline void IntrusivePtrRelease(Any* ptr) { ptr->Release(); }

	template<class T>
	inline void IntrusiveUnbound(T* ptr) { ptr->Unbound(); }
	template<class T, class S>
	inline void IntrusiveUnbound(T* ptr, S* src) { ptr->Unbound(src); }
	template<class T>
	inline void IntrusiveUnreact(T* ptr) { ptr->Unreact(); }
	template<class T, class S>
	inline void IntrusiveUnreact(T* ptr, S* src) { ptr->Unreact(src); }

	template<class S, class Arg>
	inline void UnbindReaction(S** ptr, Arg* arg)
	{
		if (*ptr) {
			S* p = *ptr;
			*ptr = 0;
			p->Unbound(arg);
		}
	}

	template<class S, class Arg>
	inline void BindReaction(S** ptr, S* fresh, Arg* arg)
	{
		UnbindReaction(ptr, arg);
		*ptr = fresh;
		if (fresh)
		{
			fresh->Bound(arg);
		}
	}

	template<class S>
	inline void FlashReaction(S** ptr)
	{
		if (*ptr)
		{
			S* p = *ptr;
			*ptr = 0;
			p->Bound(0);
			p->Unbound(0);
		}
	}

	template<class BPtr>
	inline void UnbindBinding(BPtr* binding)
	{
		if (*binding) {
			(*binding)->Unbind();
			*binding = 0;
		}
	}

	template<class BPtr, class RPtr>
	inline void UnreactBinding(BPtr* binding, RPtr reaction)
	{
		if (*binding) {
			(*binding)->Unreact(reaction);
			*binding = 0;
		}
	}

	/*
	 * typedefs for ptr
	 */
	typedef IntrusivePtr<StreamBuffer> StreamBufferPtr;
	typedef IntrusivePtr<LoopBinding> LoopBindingPtr;
	typedef IntrusivePtr<StreamState> StreamStatePtr;
	typedef IntrusivePtr<MutableStreamState> MutableStreamStatePtr;
	typedef IntrusivePtr<StreamProperties> StreamPropertiesPtr;
	typedef IntrusivePtr<StreamBinding> StreamBindingPtr;
	typedef IntrusivePtr<PacketStreamBinding> PacketStreamBindingPtr;
	typedef IntrusivePtr<MemoryStreamBinding> MemoryStreamBindingPtr;
	typedef IntrusivePtr<MemoryStreamBinder> MemoryStreamBinderPtr;
	typedef IntrusivePtr<SessionStreamBinder> SessionStreamBinderPtr;
	typedef IntrusivePtr<ListenerBinder> ListenerBinderPtr;
	typedef IntrusivePtr<ListenerBinding> ListenerBindingPtr;
	typedef IntrusivePtr<SessionShortcut> SessionShortcutPtr;
	typedef IntrusivePtr<Multiplexer> MultiplexerPtr;

	typedef IntrusivePtr<AnyLoopReaction> AnyLoopReactionPtr;
	typedef IntrusivePtr<AnyStreamReaction> AnyStreamReactionPtr;	
	typedef IntrusivePtr<AnyListenerReaction> AnyListenerReactionPtr;
	typedef IntrusivePtr<AnyMultiplexerReaction> AnyMultiplexerReactionPtr;
}

/**
 * @}
 */

#endif//VCE_VCE2STREAM_H


#ifndef VCE_VCE2_H
#define VCE_VCE2_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "vce2config.h"
#include "vce2types.h"
#include "vce2wchar.h"
#include "vce2defs.h"

#if defined(VCE_OS_WINDOWS)

#include <windows.h>

#elif defined(VCE_OS_UNIX)

#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#endif

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <stddef.h>
#include <cstring>

namespace vce
{

	//! @ja バージョン番号 @endja
    extern const char *version_number;
	//! @ja ビルド番号 @endja
    extern const char *build_number;

	class DefaultAllocator
		:public MemoryAllocator
	{
		void *alloc(size_t sz);
		void free(void *p);
	};

	extern DefaultAllocator defaultallocator;
	extern MemoryAllocator *globalallocator;

	template <class T>struct GlobalAltr
	{
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		template <class U>struct rebind{typedef GlobalAltr<U> other;};
		GlobalAltr()throw(){}
		GlobalAltr(const GlobalAltr&)throw(){}
		template <class U> GlobalAltr(const GlobalAltr<U>&)throw(){}
		~GlobalAltr()throw(){}
		pointer allocate(size_type num){return (pointer)globalallocator->alloc(num*sizeof(T));}
		pointer allocate(size_type num,std::allocator<void>::const_pointer){return allocate(num);}
		void construct(pointer p,const T& value){new((void*)p)T(value);}
		void deallocate(pointer p){globalallocator->free(p);}
		void deallocate(pointer p,size_type){deallocate(p);}
		void destroy(pointer p){p->T::~T();}
		pointer address(reference value){return &value;}
		const_pointer address(const_reference value)const{return &value;}
		size_type max_size() const throw(){return 0xffffffff/sizeof(T);}
		void operator=(const GlobalAltr<T>&)throw(){}
	};

	template <class T>struct Altr
	{
		MemoryAllocator *mm;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		template <class U>struct rebind{typedef Altr<U> other;};
		explicit Altr(MemoryAllocator *&ma)throw():mm(ma){}
		Altr(const Altr&a)throw():mm(a.mm){}
		template <class U> Altr(const Altr<U>&a)throw():mm(a.mm){}
		~Altr()throw(){}
		pointer allocate(size_type num){return (pointer)mm->alloc(num*sizeof(T));}
		pointer allocate(size_type num,std::allocator<void>::const_pointer){return allocate(num);}
		void construct(pointer p,const T& value){new((void*)p)T(value);}
		void deallocate(pointer p){mm->free(p);}
		void deallocate(pointer p,size_type){deallocate(p);}
		void destroy(pointer p){p->T::~T();}
		pointer address(reference value){return &value;}
		const_pointer address(const_reference value)const{return &value;}
		size_type max_size() const throw(){return 0xffffffff/sizeof(T);}
		void operator=(const Altr<T>&a)throw(){this->mm=a.mm;}
	};

	template <class T1, class T2>bool operator==(const Altr<T1>&, const Altr<T2>&) throw() { return true; }
	template <class T1, class T2>bool operator!=(const Altr<T1>&, const Altr<T2>&) throw() { return false; }
	template <class T1, class T2>bool operator==(const GlobalAltr<T1>&, const GlobalAltr<T2>&) throw() { return true; }
	template <class T1, class T2>bool operator!=(const GlobalAltr<T1>&, const GlobalAltr<T2>&) throw() { return false; }

	template<typename T>Altr<T>Altring(MemoryAllocator *&ma){Altr<T> a(ma);return a;}

	template<typename T>
	struct l_vector
		:public std::vector<T,Altr<T> >
	{
		l_vector(MemoryAllocator *&ma)
			:std::vector<T,Altr<T> >(Altring<T>(ma))
		{
		}
	};

	template<typename T>
	struct l_basic_string
		:public std::basic_string<T,std::char_traits<T>,Altr<T> >
	{
		l_basic_string(MemoryAllocator *&ma)
			:std::basic_string<T,std::char_traits<T>,Altr<T> >(Altring<T>(ma))
		{
		}
	};

	typedef l_basic_string<char> l_string;


	template<typename A,typename B>
	struct l_map
		:public std::map<A,B,std::less<A>,Altr<std::pair<A,B> > >
	{
		MemoryAllocator *&localallocator;
		typedef std::map<A,B,std::less<A>,Altr<std::pair<A,B> > > Super;

		explicit l_map(MemoryAllocator *&ma)
			:localallocator(ma)
			,Super(std::less<A>(),Altring<std::pair<A,B> >(ma))
		{
		}
		template<typename AA,typename AB>
		l_map(MemoryAllocator *&ma,const std::map<AA,AB> &copy)
			:localallocator(ma)
			,Super(std::less<A>(),Altring<std::pair<A,B> >(ma))
		{
			typename Super::iterator to=Super::begin();
			for(typename std::map<AA,AB>::const_iterator from=copy.begin();from!=copy.end();from++)
			{
				to=Super::insert(to,typename Super::value_type(A(localallocator,from->first),B(localallocator,from->second)));
				to++;
			}
		}
		template<typename AA,typename AB>
		void operator=(const std::map<AA,AB> &copy)
		{
			Super::erase();
			typename Super::iterator to=Super::begin();
			for(typename std::map<AA,AB>::const_iterator from=copy.begin();from!=copy.end();from++)
			{
				to=Super::insert(to,typename Super::value_type(A(localallocator,from->first),B(localallocator,from->second)));
				to++;
			}
		}
		template<typename T>
		B&operator[](const T &key)
		{
			return this->operator[](A(localallocator,key));
		}
		B&operator[](const A &key)
		{
			typename l_map<A,B>::iterator i=lower_bound(key);
			if(i==Super::end())
				i=this->insert(i,typename Super::value_type(key,B(localallocator)));
			return ((*i).second);
		}
		void operator=(const l_map &copy)
		{
			insert(copy.begin(),copy.end());
		}
	};


	template <typename A,typename B>struct g_map:public std::map<A,B,std::less<A>,GlobalAltr<std::pair<A,B> > >{};

	template<typename T>struct g_vector:public std::vector<T,GlobalAltr<T> >{};

	template<typename T>struct g_basic_string:public std::basic_string<T,std::char_traits<T>,GlobalAltr<T> >
	{
		typedef std::basic_string<T,std::char_traits<T>,GlobalAltr<T> > Super;
		g_basic_string():Super(){}
		g_basic_string(const T*c):Super(c){}
		template<typename Any>
		g_basic_string(const std::basic_string<T,std::char_traits<T>,Any> &c):Super(c.c_str()){}
		template<typename Any>
		operator std::basic_string<T,std::char_traits<T>,Any>()
		{
			return Super::c_str();
		}
	};
	typedef g_basic_string<char> g_string;
	typedef g_basic_string<vce::wchar> g_wstring;

	template<typename T>struct g_basic_stringstream:public std::basic_stringstream<T,std::char_traits<T>,GlobalAltr<T> >{};
	typedef g_basic_stringstream<char> g_stringstream;

	//!@ja クリティカルでない例外の種類 @endja
    enum VCE_EXCEPTION
    {
		//!@ja 不明 @endja
        VCE_EXCEPT_UNKNOWN=0,
		//!@ja Socket作成失敗 @endja
        VCE_EXCEPT_CREATESOCKETFAIL,
		//!@ja ポート割り当て失敗 @endja
        VCE_EXCEPT_BINDFAIL,
		//!@ja Listen失敗 @endja
        VCE_EXCEPT_LISTENINGFAIL,
		//!@ja ノンブロッキング化失敗 @endja
		VCE_EXCEPT_NONBLOCKING,
		//!@ja 接続失敗 @endja
        VCE_EXCEPT_CONNECTFAIL,
		//!@ja 接続時間切れ @endja
		VCE_EXCEPT_CONNECTTIMEOUT,
		//!@ja 接続が制限されている @endja
		VCE_EXCEPT_CONNECTREFUSED,
		//!@ja 名前解決失敗 @endja
        VCE_EXCEPT_NAMERESOLUTIONFAIL,
		//!@ja 暗号化鍵交換失敗 @endja
        VCE_EXCEPT_KEYEXCHANGEFAIL,
		//!@ja 受信バッファオーバー @endja
        VCE_EXCEPT_RECVBUFFER_FULL,
		//!@ja 送信バッファオーバー @endja
        VCE_EXCEPT_SENDBUFFER_FULL,
		//!@ja SIGPIPE @endja
		VCE_EXCEPT_SIGPIPE,
		//!@ja 不正データ @endja
        VCE_EXCEPT_INVALIDDATA,
		//!@ja セッションタイムアウト @endja
		VCE_EXCEPT_TIMEOUT,
		//!@ja ソケットやファイルハンドルの過多 @endja
		VCE_EXCEPT_TOOMANYFILES,
		//!@ja すでに使われているポート番号 @endja
		VCE_EXCEPT_ALREADYLISTEN,
		//!@ja コネクションリセット @endja
		VCE_EXCEPT_CONNECTIONRESET,
		//!@ja 送信キュー溢れ @endja
		VCE_EXCEPT_OVERFLOWSENDQUEUE,
		//!@ja 送信バッファ溢れ　@endja
		VCE_EXCEPT_OVERFLOWSENDBUFFER
    };

	//!@ja セッションやリスナーの状態 @endja
    enum VCE_STATE
    {
		//!@ja 不明 @endja
        VCE_STATE_UNKNOWN=0,
		//!@ja 接続中 @endja
        VCE_STATE_CONNECTING,
		//!@ja セッションが作成されてから接続するまで、および接続が完了しセッションが有効になるまで @endja
        VCE_STATE_PREPROCESS,
		//!@ja セッションが有効のとき @endja
        VCE_STATE_ESTABLISHED,
		//!@ja リスナーがリスニング状態のとき @endja
        VCE_STATE_LISTENING,
		//!@ja 閉じられた後。リスナーやセッションは閉じられると削除されるため、実際に CLOSED になる時間は非常に短く、その後 UNKNOWN に移行します。 @endja
        VCE_STATE_CLOSED
    };

	//!@ja 切断されたときの原因の種類 @endja
    enum VCE_CLOSEREASON
    {
		//!@ja 不明 @endja
        VCE_CLOSE_UNKNOWN=0,
		//!@ja 切断した @endja
        VCE_CLOSE_LOCAL,
		//!@ja 切断された @endja
        VCE_CLOSE_REMOTE,
		//!@ja 受信バッファフル @endja
        VCE_CLOSE_RECVBUFFERFULL,
		//!@ja 受信エラー @endja
        VCE_CLOSE_RECVERROR,
		//!@ja 不正データ受信 @endja
        VCE_CLOSE_INVALIDDATA
    };

	//!@ja ログレベルフラグ @endja
	enum VCE_LOGLEVEL
	{
		//!@ja 少なめ、重要なログ @endja
		VCE_LOGLEVEL_LOW=1,
		//!@ja やや多め、VCEの内部動作のログ @endja
		VCE_LOGLEVEL_MEDIUM=2,
		//!@ja 多め、OSレベルのAPI呼び出しのログ @endja
		VCE_LOGLEVEL_HIGH=4
	};

    class Base;
    class Session;
    class Listener;
	class Finder;
	class MonitorListener;
	typedef std::vector<MonitorListener*> MonitorListenerList;

	static const size_t VCE_BUFFER_ERROR=(size_t)-1;

    //!@ja  VCEメインクラス @endja
    /*!@ja 
    VCEオブジェクトはこのクラスから直接生成せず、VCECreate関数の返り値を使うようにしてください。
    使い終わって削除するときはVCEDelete関数に渡します。
    複数生成することもできます。
    @endja
    */

    class VCE//abstract
    {
	protected:
		VCE();
		virtual ~VCE();
    public:
		//!@ja アロケータ @endja
		vce::MemoryAllocator *ma;

        //!@ja VCEのメッセージポーリング @endja
        /*!@ja 
        定期的に呼び出すことによりVCE内部メッセージを取り出します。
        このメソッドを呼び出したスレッドから各クラスのユーザ定義仮想関数が呼び出されます。
        @endja
        */
        virtual void Poll()=0;

        //!@ja VCE のメッセージポーリング時の Wait @endja
        /*!@ja 
		Poll を連続で呼び出したときに CPU を使い切らないようにします。
		例えば 10 ミリ秒と設定した場合は、 1 秒間で 100 回程度 Poll が呼ばれるように Sleep します。負荷が高く、100 回を下回る間隔のときは Sleep しません。
		Sleep の誤差により正確な呼び出し回数にならないことがあります。
        @param wait ミリ秒単位で、 wait 時間内に再度 Poll が呼ばれたときに Sleep します。デフォルトは 0 です。
		@endja
		*/
        virtual void SetPollingWait(int wait=10)=0;

		//!@ja 1回のPollで処理するリスナーとセッションの数を制限します @endja
		/*!@ja
		1回のPollで処理するリスナーとセッションの数を制限します。
		セッション数が多くなるほどPollの呼び出しから処理が戻るまでに時間がかかるようになります。
		1回のPollで処理するリスナーとセッションの数を調整することで、処理を分散できます。
		小さくするとPollから処理が戻るのが早くなりますが、通信のレスポンスが悪くなります。
		大きくするとPollから処理が戻るのに時間がかかりますが、通信のレスポンスが良くなります。
		デフォルトは64になっており、Windowsでは64以下に制限されます。
		@param limit 制限値
		@endja
		*/
		virtual void SetSelectLimit(unsigned int limit)=0;


        //!@ja 接続します。 @endja
        /*!@ja 
        セッションsを使いhostname:portに接続します。
        @param s Session派生クラスへのポインタを渡します。渡したポインタは内部で管理され、内部で使用されなくなると切り離されます。既に使われているオブジェクトを設定することはできません。
        @param hostname 接続先ホスト名もしくはIPアドレスを記述した文字列。
        @param port 接続先TCPポート番号。
        @param timeout 接続を試みるミリ秒時間。これを過ぎてもつながらない場合はException関数がコールされます。0を指定した場合は、つながるかキャンセルされるまで試行します。
        @return ユニークID、失敗した場合は0
        @sa Session
		@sa Session::CancelConnecting
		@sa Listen
        @endja
        */
		virtual VUint32 Connect(Session *s,const std::string& hostname,VUint16 port,unsigned int timeout=0)=0;
        virtual VUint32 Connect(Session *s,const vwstring& hostname,VUint16 port,unsigned int timeout=0)=0;
		//!@ja UDPで接続します。 @endja
		/*!@ja
		TCPの代わりにUDPで接続します。相手はUdpListenしているサーバです。ポート番号はUDPポートを指定します。その他の使い方はConnect関数と同じになります。
		@sa Connect
		@sa UdpListen
		@endja
		*/
		virtual VUint32 UdpConnect(Session *s,const std::string& hostname,VUint16 port,unsigned int timeout=0)=0;
		virtual VUint32 UdpConnect(Session *s,const vwstring& hostname,VUint16 port,unsigned int timeout=0)=0;

		//!@ja LoopbackListenerに接続する @endja
		/*!@ja
        同じVCEオブジェクトでLoopbackListenしているポート番号を指定し、接続します。
		@param port ポート番号。
		@return セッションのID。失敗すると0が返ります。
		@endja
		*/
		virtual VUint32 LoopbackConnect(Session *s,VUint16 port)=0;

		//!@ja 接続を待ち受けます。 @endja
        /*!@ja 
        portをリスニング(待ちうけ)します。
        @param l リスニングに使うリスナーオブジェクトへのポインタを渡します。渡したポインタは内部で管理され、使用されなくなると切り離されます。既に使われているオブジェクトを設定することはできません。
        @param port リスニングするTCPポート番号を指定します。
        @param bindaddress リスニングするIPアドレスを指定します。NULLなら自分の持つ全アドレスになります。
        @return ユニークID
        @endja
        */
		virtual VUint32 Listen(Listener *l,VUint16 port,const std::string& bindaddress="")=0;

        //!@ja UDP接続を待ち受けます。 @endja
		/*!@ja
		TCPの代わりにUDPポートで待ち受けます。使い方はListen関数と同じになります。
		@sa Listen
		@sa UdpConnect
		@endja
		*/
		virtual VUint32 UdpListen(Listener *l,VUint16 port,const std::string& bindaddress="")=0;

		//!@ja Proxy接続用のListen @endja
		/*!@ja
		ProxyListen関数はProxyプログラムに接続し、Proxyプログラムに対してListen要求を出します。
		ProxyListen関数を使ってもすぐに接続可能な状態にはならず、Proxyプログラムの準備ができてから
		初めて接続可能な状態になります。
		Proxyプログラムが終了したり、Proxyプログラムとの接続が切れたりするとリスナーも閉じられます。
		@param l リスナーオブジェクト
		@param proxyport Proxyプログラムのbackendport
		@param proxyaddress Proxyプログラムを実行しているPCのアドレス
		@param password Proxyプログラムに接続するためのパスワード。設定していない場合は省略できます
		@endja
		*/
		virtual VUint32 ProxyListen(Listener *l,VUint16 proxyport,const std::string& proxyaddress,const std::string& password="")=0;

		//!@ja Loopback Listen @endja
		/*!@ja
		同じVCEオブジェクト内でのみ利用可能です。
		@param port リスニングするポート番号。TCPやUDPのポートとは別になります。
		@return 成功した場合はリスナーのID。失敗すると0が返ります。
		@endja
		*/
		virtual VUint32 LoopbackListen(Listener *l,VUint16 port)=0;
		
		//!@ja Finderを設定します @endja
		/*!@ja
		@param f 設定したいFinderインターフェイスへのポインタ。
		@param port Finderに使うUDPポート番号。
		@param address バインドするアドレス。
		@param range portが使えなかったときにport+range内で再試行します。
		@endja
		*/
		virtual vce::VUint32 CreateFinder(vce::Finder *f,vce::VUint16 port=59820,const std::string& bindaddress="",vce::VUint16 range=10)=0;

        //!@ja インターフェイスオブジェクトを取得します。 @endja
        /*!@ja 
        @param uID セッションまたはリスナーのuID。
        @return 有効なセッションまたはリスナー、無効な場合はNULL。
        @endja
        */
		virtual Session* GetSession(VUint32 uID)=0;
		virtual Listener* GetListener(VUint32 uID)=0;

		//!@jaすべてのセッションを取得します。 @endja
		/*!@ja
		すべてのセッションを取得します。
		@param v セッションを確保するためのSession*のvector。自動的にセッション数にresizeされます。
		@endja
		*/
		virtual void GetAllSession(std::vector<Session*> &v)=0;

		//!@ja Listenするときのアドレス/ポートを再利用します。 @endja
		/*!@ja
		Windowsの場合、再利用するよう設定すると、サーバのポートが何らかの不具合により閉じられていないときでも、
		Listenには成功するようになります。ただ、OSがそのポートを閉じられるようになるまで接続を受け付けることができません。
		また、すでにそのポートが使われていても失敗となりません。
		Unix系OSの場合、成功するのはリスナーが閉じられているときのみです。リスナーの派生セッションが残っていてもかまいません。
		すでにリスニング状態のアドレス/ポートへのバインドは失敗します。
		@param use 再利用するならtrue。初期値はfalse。
		@sa Listen
		@endja
		*/
		virtual void ReUseAddress(bool use)=0;

		//!@ja TCPでListenするときのAccept待ち数を設定します。 @endja
		/*!@ja
		TCPでListenするときのAccept待ち数を設定します。短い時間で多くの接続をしたり、
		ConnectされているときにPollの呼び出し間隔が遅かったりするとAccept待ち数が溜まってしまいます。
		同時に接続しようとしたときに接続失敗してしまうときはこの関数で
		待ち数を増やしてみてください。初期値は100です。
		この関数はListenする前に呼び出す必要があります。
		Unix系OSでは、OSによる最大数の制限があります。CentOSでは/proc/sys/net/core/somaxconnの値に制限されます。
		Solarisでは/usr/sbin/ndd -set /dev/tcp tcp_conn_req_max_qと/usr/sbin/ndd -set /dev/tcp tcp_conn_req_max_q0コマンドにより制限を変更できます。
		Windowsではこの値は効力を持ちません。
		@param backlog Accept待ち数
		@sa Listen
		@endja
		*/
		virtual void SetTCPListenBacklog(unsigned int backlog)=0;

        //!@ja 使用中のリスナーとセッションの数を返します。 @endja
        virtual unsigned int GetUsed()=0;

		//!@ja インターフェイスをVCEが使用中か調べます。 @endja
		/*!@ja
		ConnectしたセッションやListenしたリスナーがまだ使われているかを調べることができます。使われていない場合は再利用することや、deleteすることもできます。
		@param iface 調べる対象のインターフェイス
		@return trueの場合は使用中です
		@endja
		*/
		virtual bool IsUsing(Base *iface)=0;


        //!@ja ログ出力先関数を設定します @endja
		/*!@ja
		SetLogger との併用はできません。
		callback を NULL 以外に設定すると ostream よりこちらが優先されます。
		@endja
		*/
		virtual void SetLogCallback(void(*callback)(const char *log,int level))=0;

        //!@ja ログ出力先ストリームを設定します @endja
        /*!@ja 
		出力先とレベルを指定します。すべてのログを見るには、それぞれのレベルで出力先を設定しなければなりません。
		レベル毎に別々の出力にすることもできます。
		出力先はそれぞれのレベルにつき 1 つのみで、他の VCE オブジェクトとも共有されます。
		SetLogCallback との併用はできません。
		@param target 出力先のストリームへのポインタ。
		@param level 設定するレベルフラグの組み合わせ。
		@sa VCE_LOGLEVEL
        @endja
        */
        virtual void SetLogger(std::ostream *target,int level=VCE_LOGLEVEL_LOW)=0;

		//!@ja ログの出力先ストリームを取得します @endja
		/*!@ja
		ログの出力先ストリームを取得します。
		@param level 出力先ストリームを取り出すログレベル。
		@return 取り出したログ出力先ストリーム。設定されていない場合は0です。
		@endja
		*/
        virtual std::ostream *Log(int level=0)=0;

		virtual void GetMonitorListener(MonitorListenerList &ml)=0;

		//!@ja オブジェクトがすべて処理されるのを待ちます @endja
		/*!@ja
		この関数を使うと、ConnectやListenしたオブジェクトがすべて閉じられるまでPollして待つので、
		安全にVCEDeleteできるようになります。
		@param timeout 処理を待つミリ秒時間。
		@endja
		*/
		virtual void SafeDelete(vce::VUint64 timeout)=0;

		//!@ja 一時的なバッファを確保します。 @endja
		/*!@ja
		size で指定したバイト数のバッファを確保します。足りないときは拡張されます。
		VCEDeleteのときにすべて解放されます。
		主にgenのデータ構築時に、スタックの代わりに使われます。
		@param size 確保するバイト数
		@return 確保されたバッファ
		@endja
		*/
		virtual VUint8 *BufferPush(std::size_t size)=0;
        
		//!@ja 最後に確保したバッファを解放します。 @endja
		/*!@ja
		実際には解放せずに、次にPushされたときにそのまま使えるように残します。
		@endja
		*/
		virtual void BufferPop()=0;

		//!@ja 自動でBufferPopをコールするクラスです。 @endja
		class AutoPop
		{
			VCE *api;
		public:
			AutoPop(VCE *api);
			~AutoPop();
		};
		//!@ja 確保されたバッファの合計容量を取得します。 @endja
		/*!@ja
		使用されてないバッファを含めて、確保されているバッファの総容量を取得します。
		@endja
		*/
		virtual size_t BufferSize()=0;

		//!@ja Think 関数のコールバック頻度を設定します @endja
		/*!@ja
		Session や Listener のメンバ関数である Think 関数のコールバック頻度を設定します。
		Poll 毎にチェックされ、前回の Poll から interval ミリ秒が過ぎていたら Think をコールします。
		小さすぎると Think が呼ばれすぎて無駄が大きくなります。逆に大きすぎると Codec クラスの
		timeout の反応が鈍ったりします。また Poll 関数の呼び出しに依存するので、例えば 10 を設定しても 1 秒間に必ず 100 回呼ばれるわけではありません。
		@param interval インターバルミリ秒。デフォルトは 10 です。
		@endja
		*/
		virtual void SetThinkIntervalTime(VUint32 interval)=0;
		//!@ja 暗号化利用時の鍵交換のタイムアウトを設定します。 @endja
		/*!@ja
		@param timeout タイムアウトミリ秒。デフォルトは 5000 です。
		@endja
		*/
		virtual void SetKeyExchangeTimeoutTime(VUint32 timeout)=0;
	private:
		VCE(const VCE&);
		VCE& operator=(VCE&);
	};

    //Utility
    //!@ja 16進の長さ2の文字列を数値にします。 @endja
    /*!@ja 
    "10"→16 "aa"→170 "FF"→255 のように変換されます。マイナスや一桁の16進には対応していません。
	@param XX 16進数値の文字列
	@return 10進数の数値。失敗したときは-1。
    @endja
    */
	int XXtoi(const vce::g_string& _XX);

    //selfIP(s)
	//!@ja 自分のアドレスを取得します @endja
	/*!@ja 
	Windowsの場合、アドレスを取得できるのはVCECreateを呼んだ後になります。
	ローカルループバックも含めて複数のデバイスが存在するときは複数のアドレスが取得できます。
	array_qt を超える数のアドレスが見つかった場合は iparray には超えた分は書き込まれず、必要な数が返ります。
	@param iparray アドレスを受け取るための配列
	@param array_qt 配列の最大要素数
	@return 取得したアドレスの数
	@endja
	*/
    size_t GetSelfAddress(unsigned int *iparray,size_t array_qt);


    //!@ja VUint32なアドレスを文字列に変換 @endja
    /*!@ja 
    IPv4アドレスを文字列に変換します。
    @param ip アドレス
    @return 文字列
	@sa StringToAddr
    @endja
    */
	vce::g_string AddrToString(const VUint32 &ip);
    //!@ja IPアドレスを数値に変換します。 @endja
    /*!@ja 
    @param ip "192.168.1.2"のようなアドレス文字列
    @return 正しくないとき0xffffffff
	@sa AddrToString
    @endja
    */
	VUint32 StringToAddr(const vce::g_string& _ip);

    //!@ja エンディアン変換 @endja
    /*!@ja 
    binのエンディアン（バイトオーダー）を変換します。
    @param bin 変換する32ビット値
    @return 変換された32ビット値
    @endja
    */
    VUint32 bswap(VUint32 bin);
    //!@ja エンディアン変換 @endja
    /*!@ja 
    binのエンディアン（バイトオーダー）を変換します。
    @param bin 変換する16ビット値
    @return 変換された16ビット値
    @endja
    */
    VUint16 xchg(VUint16 bin);

    //!@ja メモリ検索 @endja
    /*!@ja 
    sz_pの大きさを持つメモリ領域pの中で、sz_cmpの大きさを持つ部分文字列cmpが最初に現れる位置を見つけます。
    @param p 検索されるメモリ位置への先頭ポインタ
    @param sz_p 検索されるメモリサイズ
    @param cmp 検索するメモリ位置への先頭ポインタ
    @param sz_cmp 検索するメモリサイズ
    @return 見つかった場合は見つかった場所への先頭ポインタ。それ以外の場合はNULL。
    @endja
    */
    void *memmem(const void *p,size_t sz_p,const void *cmp,size_t sz_cmp);
    //!@ja 文字列から数値に変換 @endja
    /*!@ja 
    文字列を数字に変換する標準関数のatoi関数の廉価版。
    intの範囲(-2147483648から2147483647)までの変換ができます。数字の前に現れた文字は無視されます。
    @param str 変換する文字列
    @return 数値、正常に変換できなかった場合は0
    @endja
    */
	int atoi(const vce::g_string& _str);

	//!@ja 文字列に変換 @endja
	template<typename from>
	vce::g_string ntoa(from n)
	{
		vce::g_stringstream ss;
		ss<<n;
		return ss.str();
	}

	//!Calendar time
	struct CalendarTime
	{
		//!@ja 西暦年 @endja
		int year;
		//!@ja 1-12 @endja
		int month;
		//!@ja 1- @endja
		int day;
		//!@ja 1-24 @endja
		int hour;
		//!@ja 0-59 @endja
		int minute;
		int second;
		int milliseconds;
	};

    //!@ja 時間取得 @endja
    /*!@ja 
    一番最初に呼び出されたときからのミリ秒単位の時間を取得します。
    @return 時間
    @endja
    */
    VUint64 GetTime();

    //!@ja カレンダー形式の時間取得 @endja
    /*!@ja 
    カレンダー形式の時間を取得します。戻り値のCalendarTime構造体に年からミリ秒までの情報が入ります。
    @return 時間
    @endja
    */
	CalendarTime GetCalendarTime();

    //!@ja スレッドのSleep @endja
    /*!@ja 
    ミリ秒単位(精度は荒い)でスレッドを一時停止します。
    @param millsec 止める時間
    @endja
    */
    void ThreadSleep(VUint32 millsec);

    //!@ja 文字列のバイト数 @endja
    /*!@ja
    strlenのバッファサイズ指定版。通常のstrlenではNULL終端されてない場合バッファオーバーランしてしまい、
	調べるバッファサイズに制限をかけたいときにstrlen_sを使います。
    @endja
    */
    size_t strlen_s(const char *str,size_t buflen);
    size_t strlen(const char *str);
    //!@ja 文字列をコピー @endja
    /*!@ja 
    strcpyのバッファサイズ制限版。コピー元とコピー先のバッファサイズを指定できます。
    @endja
    */
    void strncpy(char *dest,size_t destlen,const char *src,size_t srclen);
    //!@ja 文字列をコピー @endja
    /*!@ja 
    strcpyのバッファサイズ制限版。コピー先のバッファサイズを指定できます。
    @endja
    */
    void strcpy_s(char *dest,size_t destlen,const char *src);
    //!@ja UTF8文字列の文字数 @endja
    /*!@ja 
    UTF8は1文字に1バイト以上使うので、文字数を数えるときはこの関数を使います。
    @param utf8string UTF8文字列
    @param buflen バッファサイズ
    @return 文字数
    @endja
    */
    size_t utfstrnlen(const utf8 *utf8string,size_t buflen);
    //!@ja UTF16の文字数を調べる @endja
    /*!@ja 
    UTF16の文字数を調べます。サロゲートペアに対応していません。
    @endja
    */
    size_t wcslen(const wchar *str);
    //!@ja UTF16→UTF8 @endja
    /*!@ja 
    UTF16からUTF8に変換します。
    @param dest 出力先のchar配列
    @param destlen 出力先のバッファサイズ
    @param src UTF16の文字列
    @endja
    */
    size_t utf16to8(utf8 *dest,size_t destlen,const wchar *src);
	 //!@ja UTF16→UTF8 @endja
    /*!@ja 
    UTF16→UTF8の変換をします。
    @param wstr UTF16の文字列
	@return UTF8の文字列
    @endja
    */
	std::string utf16to8(const vwstring &wstr);
    //!@ja UTF8→UTF16 @endja
    /*!@ja UTF8→UTF16の変換をします。
    @param dest 出力先のwchar配列
    @param destlen 出力先のバッファのバイト数
    @param src UTF8の文字列
    @endja
    */
    size_t utf8to16(wchar *dest,size_t destlen,const utf8 *src);
    //!@ja UTF8→UTF16 @endja
    /*!@ja UTF8→UTF16の変換をします。
    @param str UTF8の文字列
	@return UTF16の文字列
    @endja
    */
	vwstring utf8to16(const std::string& str);
    //STL使うUtl
	typedef std::map<std::string,std::string> ConfigMap;
    //!@ja 設定ファイルを読み込む @endja
    /*!@ja 
    name=value 形式の文字列をconf["name"]==valueにマップします。"#"記号以降と、"#"記号の前の空白、"="の前後の空白は無視されます。
    @param filename 設定ファイル
    @endja
    */
	ConfigMap LoadSimpleConfig(const std::string& filename);

    /*!internal
	 * 
	 */
	ConfigMap LoadSimpleConfig(std::istream& input);

    //!@ja 時間を文字列で取得 @endja
    /*!@ja 
    "YYYY/MM/DD hh:mm:ss.msec"形式の文字列を取得します。ログ出力に使用します。
    @return 時間の文字列
    @endja
    */
	vce::g_string NowTimeString();

	typedef std::vector<std::string> StringList;
    //!@ja 文字列分割 @endja
    /*!@ja 
	delimで文字列を分割します。ただし、escapeで囲まれた部分はひと固まりの文字列として扱われ、delimを含んでいても分割されません。
	escapeは出力文字列に含まれません。
    初期値がそのままならば、たとえば 'aaa bbb "c c c"' は、'aaa' 'bbb' 'c c c' の3つに分割されます。
    @param string 入力文字列。
    @param delim 区切り文字。初期値は' '。
    @param escape エスケープ文字。初期値は'"'。
    @return 分割された文字列
    @endja
    */
    StringList SplitString(const std::string& string, char delim = ' ', char escape = '"');
	typedef std::map<std::string,std::string> PostMap;
    //!@ja URLのクエリー文字列形式を解析する @endja
    /*!@ja 
    HTTP POSTリクエストなどに使われるクエリー文字列形式を解析して、キーと値のペアに分割します。例えば 'aaa=bb&ccc=a%20bc' は2つに分割されます。'%XX' はURIデコードされます。
    @endja
    */
    PostMap SplitPost(const std::string& _post);

    //!@ja URIエンコード @endja
    /*!@ja 
    a-z,A-Z,0-9を除くすべてをURIエンコードします。
	@return 変換された文字列
    @endja
    */
    std::string encodeURI(const std::string& text);
    //!@ja URIをデコード @endja
    /*!@ja 
	URIエンコードされた文字列をデコードします。
	@return 変換された文字列。失敗すると空
    @endja
    */
    std::string decodeURI(const std::string& text);

	//!@ja Base64エンコード @endja
	/*!@ja 
	バイナリデータをBase64エンコードします。戻り値はNull終端された文字列で、元データの4/3倍程度になります。
	@param data バイナリデータへのアドレス
	@param len バイナリデータの長さ
	@return 変換した文字列
	@endja
	*/
	std::string Base64Encode(const void *data,int len);

	//!@ja Base64Decode @endja
	/*!@ja 
	Base64エンコードされた文字列をデコードします。
	@param src Base64エンコードされた文字列。
	@param dest 格納する確保済み領域へのポインタ。最低、文字列の長さの(len/4+1)*3バイト確保しておきます。
	@return 変換した長さ、失敗した場合-1。
	@endja
	*/
	int Base64Decode(const char *src,unsigned char *dest);
	std::string Base64DecodeString(const std::string &src);
	int Base64Decode(const char *src,std::vector<unsigned char> & dest);

	//!@ja バイナリデータを16進文字列変換 @endja
	/*!@ja 
	変換された文字列は2文字単位で、アルファベットの16進数は大文字で出力されます。
	@param data 変換するデータへの先頭ポインタ
	@param len 変換するバイト数
	@return 変換された文字列
	@endja
	*/
	std::string MemoryToStr(const void *data,size_t len);

	class Random//final
	{
		void *context;
	public:

		friend class BlockEncryptCamellia;
		friend class BlockEncryptBlowfish;
		friend class BlockEncryptRijndael;
		friend class RSAEncrypt;

		/*!@ja
		コンストラクタ。時間係数で擬似乱数の種を設定し初期化します。
		@endja
		*/
		Random();
		/*!@ja
		コンストラクタ。seedを元に擬似乱数の種を設定します。
		@param seed 種の値が入ったバッファへのポインタ。同じ種からは同じパターンで擬似乱数が生成されます。
		@param seedsize 種のバッファのバイト数
		@endja
		*/
		Random(unsigned char *seed,size_t seedsize);
		/*!@ja
		デストラクタ
		@endja
		*/
		~Random();
		/*!@ja
		擬似乱数を生成します。
		@return 生成された値。0~2^32の範囲になります。
		@endja
		*/
		unsigned int rand();
	};

	class RSAEncrypt
	{
		void *context;
		Random* random;
		RSAEncrypt(const RSAEncrypt&);
		RSAEncrypt& operator=(RSAEncrypt&);
	public:
		RSAEncrypt();
		/* FIXME: add description */
		explicit RSAEncrypt(Random* rand);
		~RSAEncrypt();
		/*!@ja
		鍵を生成します。鍵のビット長は128の倍数かつ128以上、1024以下である必要があります。
		@param generatebits 鍵のビット長
		@return 成功した場合はtrue。
		@endja
		*/
		bool GenerateKey(int generatebits);
		/*!@ja
		生成された鍵を設定します。
		@param key 鍵のバッファ
		@param keybits 鍵のビット長
		@return 成功した場合はtrue。
		@endja
		*/
		bool SetPublicKey(const unsigned char *key,int keybits);
		/*!@ja
		生成された鍵を取得します。
		@param key 鍵を受け取るバッファ。バッファは最低でも鍵のビット長/8のバイト数確保されている必要があります。
		@return 成功した場合はtrue。
		@endja
		*/
		bool GetPublicKey(unsigned char *key)const;
		/*!@ja
		暗号化します。最後の1バイトはゼロにしておきます。ただしkeyとデータの組み合わせによっては
		暗号化に失敗することがあります。
		@param buf 暗号化するデータが入ったバッファ。バッファは最低でも鍵のビット長/8バイト数確保されている必要があります。
		@return 成功した場合はtrue。
		@endja
		*/
		bool Encrypt(unsigned char *buf);
		/*!@ja
		暗号化されたデータを復号化します。
		@param buf 復号化するデータが入ったバッファ。
		@return 成功した場合はtrue。
		@endja
		*/
		bool Decrypt(unsigned char *buf);
	};


    /*!@ja
    暗号化を扱うための抽象クラス。このクラスを使って操作します。 
    オブジェクトを生成するときは派生クラスで生成する必要があります。 
    @endja 
    */
 	class BlockEncrypt
	{
	protected:
		BlockEncrypt();
	public:
		virtual ~BlockEncrypt();
        /*!@ja 
        鍵と鍵のビット長を設定して初期化します。Camelliaでは128と256ビット、Blowfishでは96から448ビットまで指定できます。 
        @param key 鍵へのバッファ 
        @param keybitlength 鍵のビット長 
        @return 成功した場合はtrue。 
        @endja 
        */
        virtual bool Initialize(unsigned char *key,int keybitlength)=0;
		/*!@ja
		暗号化の度にcontextの内容が変化するため、同じ鍵であっても暗号結果は変わってしまいます。
		同じ鍵で複数のデータを暗号化したいときはReinitializeで初期化しなおす必要があります。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool Reinitialize()=0;
		/*!@ja
		暗号化します。
		@param src 暗号化するデータが入ったバッファ。
		@param srcsize バッファのバイト数。
		@param dest 暗号化したデータが入るバッファ。vector変数への参照を渡します。自動的にresizeされます。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool Encrypt(const void *src,size_t srcsize,std::vector<unsigned char> &dest)=0;
		/*!@ja
		復号化します。
		@param src 暗号化されたデータが入ったvector。
		@param dest 復号化されたデータが入るバッファ。vector変数への参照を渡します。自動的にresizeされます。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool Decrypt(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest)=0;
		/*!@ja
		暗号化するデータをvectorにし、様々な型を指定できるようにしたものです。
		@sa Encrypt
		@endja
		*/
		template<typename Type>bool Encrypt(const std::vector<Type> &src,std::vector<unsigned char> &dest)
		{
			return Encrypt(&*src.begin(),src.size()*sizeof(Type),dest);
		}
	};


	class BlockEncryptCamellia
		:public BlockEncrypt
	{
		void *context;
		Random* random;
	public:
		/*!@ja
		Camelliaアルゴリズムを使うためのオブジェクトを生成します。鍵長には128ビットと256ビットが指定できます。
		このコンストラクタで構築したオブジェクトはスレッドセーフではありません。
		@endja
		*/
		BlockEncryptCamellia();
		/*!@ja
		 乱数オブジェクトを指定するコンストラクタです。
		 このコンストラクタで構築したオブジェクトはスレッドセーフです。
		@endja
		*/
		explicit BlockEncryptCamellia(Random* rand);
		~BlockEncryptCamellia();

		bool Initialize(unsigned char *key,int keybitlength);
		bool Reinitialize();
		bool Encrypt(const void *src,size_t srcsize,std::vector<unsigned char> &dest);
		bool Decrypt(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest);
	};

	class BlockEncryptBlowfish
		:public BlockEncrypt
	{
		void *context;
		Random* random;
	public:
		/*!@ja
		Blowfishアルゴリズムを使うためのオブジェクトを生成します。鍵長には96から448ビットまで指定できます。
		このコンストラクタで構築した
		オブジェクトはスレッドセーフではありません。
		@endja
		*/
		BlockEncryptBlowfish();
		/*!@ja
		乱数オブジェクトを指定するコンストラクタです。
		このコンストラクタで構築したオブジェクトはスレッドセーフです。
		@endja
		*/
		explicit BlockEncryptBlowfish(Random* rand);
		~BlockEncryptBlowfish();

		bool Initialize(unsigned char *key,int keybitlength);
		bool Reinitialize();
		bool Encrypt(const void *src,size_t srcsize,std::vector<unsigned char> &dest);
		bool Decrypt(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest);
	};

	class BlockEncryptRijndael
		:public BlockEncrypt
	{
		void *context;
		Random* random;
	public:
		/*!@ja
		Rijndaelアルゴリズムを使うためのオブジェクトを生成します。128、192、256ビットの鍵長を指定できます。
		このコンストラクタで構築したオブジェクトはスレッドセーフではありません。
		@endja
		*/
		BlockEncryptRijndael();
		/*!@ja
		乱数オブジェクトを指定するコンストラクタです。
		このコンストラクタで構築したオブジェクトはスレッドセーフです。
		@endja
		*/
		explicit BlockEncryptRijndael(Random* rand);
		~BlockEncryptRijndael();

		bool Initialize(unsigned char *key,int keybitlength);
		bool Reinitialize();
		bool Encrypt(const void *src,size_t srcsize,std::vector<unsigned char> &dest);
		bool Decrypt(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest);
	};

	//!@ja 圧縮を行うインターフェイス @endja
	class CompressBase
	{
	protected:
		//!@ja context @endja
		void *context;
	public:
		//!@ja 直接生成せず、派生クラスを生成するようにしてください。 @endja
		CompressBase();
		//!@ja デストラクタ @endja
		virtual ~CompressBase();
		//!@ja 圧縮 @endja
		/*!@ja
		バイナリデータを圧縮します。
		元データが複雑なデータであったり、小さすぎるデータであったりすると圧縮できずに失敗します。
		圧縮に失敗したときは出力データが正しくないので、destをUncompressに渡してはいけません。
		@param src 圧縮対象のバッファ
		@param srcsize バッファのバイト数
		@param dest 圧縮されたデータを受け取るバッファ、vector配列への参照を渡します。
		@return 成功した場合はtrue。圧縮に失敗した場合はfalse。元データより出力データが大きくなった場合もfalseが返ります。
		@endja
		*/
		virtual bool Compress(const void *src,size_t srcsize,std::vector<unsigned char> &dest)=0;
		//!@ja 圧縮関数のテンプレート版 @endja
		/*!@ja
		@param src 圧縮対象のバッファ
		@sa Compress
		@endja
		*/
		template <typename T>bool Compress(const std::vector<T> &src,std::vector<unsigned char> &dest)
		{
			return src.size()>0?Compress((void*)&src.front(),sizeof(T)*src.size(),dest):false;
		}
		//!@ja 展開 @endja
		/*!@ja
		@param src Compressにより圧縮されたバッファ
		@param dest 展開されるデータを受け取るバッファ。vector配列への参照を渡します。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool Uncompress(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest)=0;
		//!@ja 展開関数のテンプレート版 @endja
		/*!@ja
		さまざまな型のvectorに対応できます。ただしCompressで使う型と合わせる必要があります。
		@endja
		*/
		template <typename T>bool Uncompress(const std::vector<unsigned char> &src,std::vector<T> &dest)
		{
			std::vector<unsigned char> temp;
			if(Uncompress(src,temp))
			{
				dest.resize(temp.size()/sizeof(T));
				std::memcpy(&dest.front(),&temp.front(),temp.size());
				return true;
			}
			return false;
		}

	private:
		CompressBase(const CompressBase&);
		CompressBase& operator=(CompressBase&);
	};

	class CompressLZP
		:public CompressBase
	{
	public:
		CompressLZP();
		~CompressLZP();

		bool Compress(const void *src,size_t srcsize,std::vector<unsigned char> &dest);
		bool Uncompress(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest);
	};

	class CompressZIP
		:public CompressBase
	{
	public:
		CompressZIP();
		~CompressZIP();

		bool Compress(const void *src,size_t srcsize,std::vector<unsigned char> &dest);
		bool Uncompress(const std::vector<unsigned char> &src,std::vector<unsigned char> &dest);
	};



    //!@ja VCE内部向けクラス @endja
    /*!@ja 
    内部で管理させるためのベースクラス。
    ユーザがこのクラスを直接使用することはありません。
    @endja
    */

    class Base
    {
    protected:
		//!@ja コンストラクタ @endja
        Base();
		//!@ja デストラクタ @endja
        virtual ~Base();
	private:
		Base(const Base&);
		Base& operator=(Base&);
    public:
		//!@ja 内部オブジェクトへのポインタのキャッシュ @endja
        void*cache;
        //!@ja 固有識別子 @endja
        VUint32 uID;
        //!@ja このオブジェクトを管理しているVCEオブジェクト @endja
        VCE *api;

		//!@ja VCE 内部管理され始めたら呼ばれるコールバック関数 @endja
		/*!@ja 
        Connect や Listen 等に渡したときに呼ばれます。
		これが呼ばれてから Detached が呼ばれるまで delete してはいけません。
		@sa Detached
		@endja
		*/
        virtual void Attached();
        //!@ja VCE 内部管理から切り離されたときに呼ばれるコールバック関数 @endja
        /*!@ja 
        Connect や Listen 等の引数に渡したオブジェクトが VCE 内部で使用されなくなると呼ばれます。
        Detached が呼ばれた後であれば delete することができます。
		@sa Attached
        @endja
        */
        virtual void Detached();
        //!@ja 思考関数 @endja
        /*!@ja 
        vce::Pollが呼ばれたときにこの関数も呼ばれます。
		ユーザが好きなようにオーバーライドすることができます。
        @endja
        */
        virtual void Think();

		//!@ja 同じオブジェクトか比較します。 @endja
		/*!@ja
		VCEで使用されるオブジェクトは固有のIDを保持しており、その比較をします。
		この比較結果が真であれば同じセッションもしくはリスナーということになります。
		ポインタだけの比較では閉じられたセッションと同じアドレスに再確保されることがあるので
		VCEで一意に管理されているIDで比較する必要があります。
		@endja
		*/
        bool operator ==(Base &target)const;
		//!@ja 違うオブジェクトか比較します。 @endja
        bool operator !=(Base &target)const;
        //!@ja 同じオブジェクトか比較します。 @endja
        bool IsEqual(Base &target)const;

        //!@ja 状態を取得 @endja
        /*!@ja 
		状態を取得します。セッションの状態はPREPROCESS→CONNECTING→ESTABLISHED→CLOSEDのように遷移していきます。
		リスナーではPREPROCESS→LISTENING→CLOSEDのように遷移していきます。
		PREPROCESSの間に名前解決やポートへのバインドがおこなわれます。
		CLOSEDになるとすぐVCEからオブジェクトが削除されてしまうため、実際にこの関数でCLOSEDが返ってくるタイミングは、Closed関数がコールバックされたときだけです。
		@sa VCE_STATE
        @endja
        */
        virtual VCE_STATE GetState()const;

        //!@ja 例外取得用関数 @endja
        /*!@ja 
        クリティカルでない例外が起こったときに呼ばれます。
		@sa VCE_EXCEPTION
        @endja
        */
        virtual void Exception(VCE_EXCEPTION type);

        //!@ja socketハンドルを取得します。 @endja
        int GetSocket();

        //!@ja VCE内部で使われているかチェック @endja
		/*!@ja
		trueの場合はVCE内部で使われているのでdeleteしてはいけません。
		@deprecated この関数は廃止予定です。かわりに VCE::IsUsing(Base*) を使用してください。
		@endja
		*/
        bool IsUsing();

        //!@ja Poll 時の処理優先度を変更します @endja
		/*!@ja
		Poll 時の処理優先度を変更します。 Attached 後に設定を変えることにより Poll するときに優先して処理されます。
		一時的に大量のデータの送受信が発生するときに、一部のセッションだけ多めに処理させることができます。
		@param priority 1にすることで優先して処理します。デフォルトは 0 です。
		@endja
		*/
		void SetPollingPriority(int priority);
    };


    //!@ja VCEリスナークラス @endja
    /*!@ja 
    Listening中のポートへの接続要求に対する動作を定義するためのクラス。
	@sa vce::Listen
    @sa AutoAccepter
    @endja
    */
    class Listener
        :public Base
    {
    public:
		//!@ja コンストラクタ @endja
        Listener();
		//!@ja デストラクタ @endja
        virtual ~Listener();
        //!@ja 接続要求 @endja
        /*!@ja 
        この関数をオーバーライドし、接続を受けるセッションクラスを返します。接続を拒否する場合はNULLを返します。
        @param remote_addr 接続してきたアドレス
        @param remote_port 接続してきたポート
        @return NULLなら拒否
        @endja
        */
        virtual Session *Accepter(VUint32 remote_addr,VUint16 remote_port)=0;

		//!@ja Closeの後呼ばれるコールバック関数 @endja
		/*!@ja
		接続を受け入れられるポートを閉じ、以降受け入れないようにします。
		UDPリスナーの場合は既に受け入れ済みのセッションが通信できなくなり、
		また、セッションのparentオブジェクトが削除されてしまうので注意が必要です。
		@return なし
		@endja
		*/
		virtual void Closed();

		//!@ja 子セッションの Detached が呼ばれる前に呼ばれるコールバック関数 @endja
		virtual void ChildDetached(VUint32 ChildID);

		//!@ja リスニングを中断する @endja
		/*!@ja 
		リスニングをやめてポートを閉じます。
		すでに接続されたセッションは残ります。
		@endja
		*/
        bool Close();
        //!@ja セッション数、リスト取得 @endja
        /*!@ja 
        このリスナーから派生したセッション一覧を取得します。
        @param child 受け取るポインタ配列
        @param array_maxlen 受け取る配列に入れられる最大要素数
        @return セッションの数,若しくは配列にコピーした数
        @endja
        */
        unsigned int GetChildren(Session **child,unsigned int array_maxlen);
		//!@ja セッション数取得 @endja
		/*!@ja 
        @return セッション数
		@endja
		*/
        unsigned int GetChildren();
		//!@ja 状態取得 @endja
		/*!@ja 
        @return 状態
		@endja
		*/
        VCE_STATE GetState()const;
		//!@ja GetChildrenのvector版 @endja
		/*!@ja 
		Session型のvectorを引数に渡すとそこへセッション一覧を詰めます。自動的にresizeされます。
		@param ch 配列
		@endja
		*/
        template <typename T> void GetChildren(std::vector<T*> &ch)
        {
            ch.resize(GetChildren());
			if(ch.size())GetChildren((Session**)&ch[0],(unsigned int)ch.size());
        }

		typedef std::map<std::string,std::string> ProxyConfigParameter;
		//!@ja Proxyの設定を変える @endja
		/*!@ja
		@param function 変えたい設定の名前
		@param parameter パラメータの集合
		@return 送信に失敗したときはfalse
		@endja
		*/
		bool ProxyConfig(const std::string& function,ProxyConfigParameter parameter);

        //!@ja リスナーのIPアドレスを取得 @endja
        /*!@ja 
        @param addr IPアドレスを格納する変数への参照
        @param port ポート番号を格納する変数への参照
        @return 取得できたらtrue、できなかったらfalse。
        @endja
        */
        bool GetListenAddress(VUint32 &addr,VUint16 &port);
	
	};

	//!@ja UDP状態取得用構造体 @endja
	struct UdpStatus
	{
		//!@ja 送信しようとしたヘッダ込みのバイト数 @endja
		VUint32 sendsize;
		//!@ja 送信しようとした回数 @endja
		VUint32 sendcount;
		//!@ja 再送信しようとしたバイト数 @endja
		VUint32 resendsize;
		//!@ja 再送信しようとした回数 @endja
		VUint32 resendcount;
		//!@ja 受信したヘッダ込みのバイト数 @endja
		VUint32 recvsize;
		//!@ja 受信した全パケットの個数 @endja
		VUint32 recvcount;
		//!@ja 受信した不要パケットの個数 @endja
		VUint32 dropcount;
	};

    //!@ja VCEストリームセッションクラス @endja
    /*!@ja 
    セッションを操作するのに使います。このクラスを継承し、各仮想関数をオーバーライドしていきます。
	@sa VCE::Connect
    @sa Listener::Accepter
    @endja
    */

    class Session
        :public Base
    {
    public:
		//!@ja 暗号化鍵交換状態 @endja
		enum KEYEX_STATE
		{
			//!@ja 不明 @endja
			KEYEX_UNKNOWN,
			//!@ja 接続完了している @endja
			KEYEX_CONNECTED,
			//!@ja キー生成した,キー送信中 @endja
			KEYEX_KEYGENERATED,
			//!@ja key送信した @endja
			KEYEX_KEYSENDED,
			//!@ja RSAキー待ち @endja
			KEYEX_WAITING,
			//!@ja RSAキー送信中 @endja
			KEYEX_KEYSENDING,
			//!@ja 鍵交換終了 @endja
			KEYEX_READY
		};
		//!@ja コンストラクタで設定するセッションのフラグ。 @endja
        enum LowLevelFlags
        {
			//!@ja 何もなし @endja
            LLF_NONE=0,
			//!@ja Camellia暗号化 @endja
			LLF_ENCRYPT_CAMELLIA=0x01,
			//!@ja Blowfish暗号化 @endja
			LLF_ENCRYPT_BLOWFISH_128BIT=0x02,
			//!@ja Rijndael暗号化 @endja
			LLF_ENCRYPT_RIJNDAEL=0x03,
			//!@ja VCE2デフォルトの暗号化 @endja
            LLF_ENCRYPT_DEFAULT=LLF_ENCRYPT_CAMELLIA,
			//!@ja LZP圧縮 @endja
			LLF_COMPRESS_LZP=0x10,
			//!@ja ZIP圧縮 @endja
			LLF_COMPRESS_ZIP=0x20
        };
		unsigned int llflags;//!<@ja フラグ @endja

	private:
		//!@ja 接続状態 @endja
		enum ConnectingStatus
		{
			CS_WAIT,
			CS_SUCCESS,
			CS_FAIL
		}connectingstatus;//!<@ja 接続状態 @endja
	public:

        //!@ja リスナーへのポインタ @endja
        /*!@ja 
        パッシブ接続の場合はリスニングしていた親クラスへのポインタが入ります。
        アクティブ接続の場合はNULLが入ります。
        @endja
        */
        Listener *parent;

		//!@ja コンストラクタ @endja
        Session();
		//!@ja デストラクタ @endja
		virtual ~Session();

		void Exception(vce::VCE_EXCEPTION type);

        //!@ja セッションを強制的に閉じる @endja
        /*!@ja 
        セッションをすぐ閉じます。
        @sa Close
        @sa Closed
        @endja
        */
        void ForceClose();
        //!@ja セッションを閉じる @endja
        /*!@ja 
        すぐ切断せず、送信バッファが空になってから送信を閉じ、受信し終わってから受信を閉じます。
        単発のリクエストを出し、それに対するレスポンスだけを受け取る時等に使用します。
		@sa ForceClose
        @sa Closed
        @endja
        */
        void Close();
        //!@ja 接続試行中止 @endja
        /*!@ja 
        Connectした後、timeoutを待たずにあきらめるときに使います。
		この後にExceptionがコールされてVCE_EXCEPT_CONNECTFAILが渡されます。
		@sa VCE::Connect
        @endja
        */
        void CancelConnecting();
		//!@ja ブロッキングする @endja
		/*!@ja
		接続完了もしくは失敗するまでブロッキングします。継承先からベースクラスのExceptionやConnectedを呼び出すのを忘れると正常に動作しないことがあります。
		@return trueなら接続成功
		@sa VCE::Connect
		@endja
		*/
		bool BlockConnecting();

        //!@ja データ送信 @endja
        /*!@ja 
        データを送信バッファに書き込み、送信します。
        @param p 送信データへの先頭ポインタ。
        @param sz 送信データのバイト数。
        @return 成功した場合はtrue、失敗した場合はfalse。
        @endja
        */
        bool Send(const void *p,size_t sz);
        //!@ja 受信バッファのデータを参照 @endja
        /*!@ja 
        受信バッファの内容を直接参照するにはこの関数を使います。
        @param size 受信バッファのバイト数。
        @return 受信バッファへの先頭ポインタ、NULLの場合は失敗。
		@sa EraseReadbuf
        @endja
        */
        const VUint8 *GetReadbuf(size_t &size);
        //!@ja 受信バッファのデータを削除 @endja
        /*!@ja 
        受信バッファの先頭からデータを削除する事ができます。
        @param size 削除するバイト数
		@sa GetReadbuf
        @endja
        */
        void EraseReadbuf(size_t size);

        //!@ja 接続が完了したときに呼ばれるコールバック関数 @endja
        virtual void Connected();
        //!@ja 接続が閉じたときに呼ばれるコールバック関数 @endja
        virtual void Closed(VCE_CLOSEREASON type);
        //!@ja データ受信コールバック関数 @endja
        /*!@ja 
        この仮想関数をオーバーライドし受信処理を書きます。
        @param p 受信データへのポインタ
        @param sz 受信データのバイト数
        @return 処理したバイト数。0なら受信バッファのデータは削除されません。1以上ならバッファの先頭からバイト単位で削除されます。
        @endja
        */
        virtual size_t Recv(const VUint8 *p,size_t sz);

        //!@ja 接続先のIPアドレスを取得 @endja
        /*!@ja 
        @param addr IPアドレスを格納する変数への参照
        @param port ポート番号を格納する変数への参照
        @return 取得できたらtrue、できなかったらfalse。
        @endja
        */
        bool GetRemoteAddress(VUint32 &addr,VUint16 &port);

		//!@ja 接続先のホスト名を取得 @endja
		/*!@ja
		接続時に使用したホスト名が取得できます。
		逆引きには対応していないので逆引きが必要な場合は取得できません。
		@return ホスト名、無い場合は空文字列。
		@endja
		*/
		std::string GetRemoteHostname()const;

		//!@ja Nodelayを有効にします @endja
		/*!@ja
		TCPストリームでは細かいデータを短期間で送信するとパケットのヘッダ分のオーバーヘッドがとても大きくなります。
		そのためNodelayが標準で無効になっており、Nagleアルゴリズムに従って細かいデータを一旦バッファし
		一定時間過ぎたらまとめて効率よく送信するようになっています。しかしバッファされたデータは一定時間過ぎないと送信されないため、
		本当に短時間でデータを送信したい場合はNodelayを有効にするようにします。
		この関数はConnectedコールバック後に使うことができます。ただし環境によっては設定を変えられない可能性もあります。
		@param enable Nodelayを有効にするときはtrue
		@return 成功した場合はtrue。
		@endja
		*/
		bool SetNodelay(bool enable=true);

        //!@ja 接続元のIPアドレスを取得 @endja
        /*!@ja 
        @param addr IPアドレスを格納する変数への参照
        @param port ポート番号を格納する変数への参照
        @return 失敗はfalse
        @endja
        */
        bool GetLocalAddress(VUint32 &addr,VUint16 &port)const;

        //!@ja 送信バッファの空き容量 @endja
        /*!@ja 
        送信バッファの使用量が増えるとメモリ再確保が発生するため、シビアな状況ではこの関数でチェックしながら送信するようにします。
        @return 送信バッファ空き容量
        @endja
        */
        size_t GetAvailableSendBuffer()const;

        //!@ja 送信キューの最大値の設定 @endja
        /*!@ja 
        一度に送信する量を設定します。送信キューにまだデータが残っているときはそれを超えないようにします。
		1 回の Poll でより多く送信したいときや、特定のセッションの送信量を絞ったりするときに使います。
		Connected 後に設定することで有効になります。現在、 Linux のみ対応しています。
		他のプラットフォームでは送信キューの利用量にかかわらず送信キューに追加されます。
		@param limit 送信キューのサイズ。デフォルトは 16384 です。
        @endja
        */
        bool SetSendQueueLimit(int limit);

		//!@ja 送信バッファサイズの設定 @endja
		/*!@ja
		送信バッファサイズを設定します。同時に可変長かどうか、可変長ならば最大の容量は
		何バイトかの設定もします。Connectedコールバック後に設定します。
		コンストラクタで呼ぶと設定は反映されず、送受信が始まってからでは
		指定した容量に設定できない場合があります。
		@param size バッファサイズ
		@param variable バッファサイズを可変長にするかの指定。trueにすると大きなデータが書き込まれたときにバッファが足りない時に大きくなり、
		時間とともにバッファが小さくなります。capacityを超えるサイズが確保されることはありません。
		@param capacity バッファを大きくする場合は何バイトまで許容するかを設定します。
		バッファサイズより大きい必要があります。
		@return 成功した場合はtrue。
		@endja
		*/
		bool SetSendBufferSize(size_t size,bool variable=true,size_t capacity=1024*1024*16);
		//!@ja 受信バッファサイズの設定 @endja
		/*!@ja
		受信バッファサイズを設定します。同時に可変長かどうか、可変長ならば最大の容量は
		何バイトかの設定もします。Connectedコールバック後に設定します。
		コンストラクタで呼ぶと設定は反映されず、送受信が始まってからでは
		指定した容量に設定できない場合があります。
		@param size バッファサイズ
		@param variable バッファサイズを可変長にするかの指定。trueにすると大きなデータが書き込まれたときにバッファが足りない時に大きくなり、
		時間とともにバッファが小さくなります。capacityを超えるサイズが確保されることはありません。
		@param capacity バッファを大きくする場合は何バイトまで許容するかの設定。
		バッファサイズより大きい必要がああります。
		@return 成功した場合はtrue。
		@endja
		*/
		bool SetRecvBufferSize(size_t size,bool variable=true,size_t capacity=1024*1024*16);

		//!@ja 送信バッファサイズの取得 @endja
		/*!@ja
		送信バッファサイズを取得します。
		@return 送信バッファサイズ
		@endja
		*/
		virtual size_t GetSendBufferSize()const;
		//!@ja 送信バッファサイズが可変長かどうかの取得 @endja
		/*!@ja
		送信バッファサイズが可変長かどうかを取得します。
		@return 0ならば送信バッファサイズは固定長。
		@endja
		*/
		virtual int GetSendBufferVariable()const;
		//!@ja 送信バッファの最大容量の取得 @endja
		/*!@ja
		送信バッファの最大容量を取得します。
		@return 送信バッファの最大容量
		@endja
		*/
		virtual size_t GetSendBufferCapacity()const;
		//!@ja 受信バッファサイズの取得 @endja
		/*!@ja
		受信バッファサイズを取得します。
		@return 受信バッファサイズ
		@endja
		*/
		virtual size_t GetRecvBufferSize()const;
		//!@ja 受信バッファサイズが可変長かどうかの取得 @endja
		/*!@ja
		受信バッファサイズが可変長かどうかを取得します。
		@return 0ならば受信バッファサイズは固定長。
		@endja
		*/
		virtual int GetRecvBufferVariable()const;
		//!@ja 受信バッファの最大容量の取得 @endja
		/*!@ja
		受信バッファの最大容量を取得します。
		@return 受信バッファの最大容量
		@endja
		*/
		virtual size_t GetRecvBufferCapacity()const;

		//!@ja UDPのみ。タイムアウト設定 @endja
		/*!@ja
		タイムアウト時間を設定します。この時間内にデータが受信されなければ
		相手側セッションの不具合と判断して切断します。
		Connectedコールバック後に設定することができます。TCPの場合はアプリケーションが終了されたときはOSが後処理をするので
		大抵の場合は正しくCloseされますが、UDPはアプリケーションが終了されると正しくCloseできないのでTimeoutを待つことになります。
		@param time ミリ秒単位の時間。初期値は20000ミリ秒。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool UdpSetTimeoutTime(int time);
		//!@ja UDPのみ。パケット再送時間設定 @endja
		/*!@ja
		相手からパケット受信の確認が無かったときにパケットを再送するまでの時間を設定します。
		Connectedコールバック後に設定することができます。
		@param time ミリ秒単位の時間。初期値は1000ミリ秒。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool UdpSetResendTime(int time);
		//!@ja UDPのみ。同時送受信量設定 @endja
		/*!@ja
		相手から合図なしに送信する量を設定します。大きく設定すると一度に送受信できる量が多くなりロスしやすく、レスポンス速度が落ちます。
		小さく設定すると大きいデータのやり取りには向きませんが、ロス率が減りレスポンスが高くなります。TCPの受信窓バッファと同じような働きをします。
		Connectedコールバック後に設定することができます。
		@param size バイト単位のサイズ。実際にはパケットのサイズなどに合わせたりするので若干これより大きくなることがあります。初期値は5720です。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool UdpSetTargetBufferSize(int size);
		//!@ja UDPのみ。UDP固有の情報取得関数 @endja
		/*!@ja
		UDP固有のセッション情報を取得します。
		@param status UdpStatus構造体への参照。
		@return 成否
		@sa UdpStatus
		@endja
		*/
		virtual bool UdpGetStatus(UdpStatus &status)const;

		//!@ja UDP のみ。信頼性のないデータ送信に切り替えます。 @endja
		/*!@ja
		信頼性のないデータ送信に切り替えます。暗号や圧縮セッションの場合はこの関数を利用しても信頼性のあるデータとして送信されます。
		以後、 UdpFinishRaw が呼ばれるまで一旦バッファリングします。 
		最大で 1400 バイトまでバッファリングされ、 1400 バイトを超えたときは超えた分が破棄されます。
		送信関数を呼ぶ前にこの関数を使い、送信関数を呼んだ後に UdpFinishRaw を使ってください。
		@return 成功した場合は true
		@sa UdpFinishRaw
		@endja
		*/
		bool UdpPrepareRaw();

		//!@ja UDP のみ。信頼性のないデータ送信をします。 @endja
		/*!@ja
		バッファリングされたデータを即座に送信します。必ず UdpPrepareRaw と対にして使ってください。
		@return 成功した場合は true
		@sa UdpPrepareRaw
		@endja
		*/
		bool UdpFinishRaw();

		//!@ja UDP のみ。信頼性のないデータ受信か調べます。 @endja
		/*!@ja
		gen を利用するとフォーマットに合わないデータが受信された場合は切断してしまいますが、
		信頼性のないデータの場合は RecvException 関数の中でこの関数を使い、切断するかどうかの判断の精度を高められます。
		@return 信頼性のないデータ受信の場合は true を返します。
		@endja
		*/
		bool UdpRecvIsRaw();

        VCE_STATE GetState()const;

		//!@ja 鍵交換の状態を取得します @endja
		KEYEX_STATE GetKeyexState()const;
    };

    //!@ja 自動セッション開始クラス @endja
    /*!@ja 
    接続が来たときに自動でサーバ用セッションを生成し、返すためのテンプレートクラス。
    このクラスからリスナーを生成し、Listen関数に引数として渡すだけで動作します。
	@sa net::Listen
    @endja
    */
    template <class T>class AutoAccepter:public Listener
    {
        bool enable;//!@ja 接続を許可しているか @endja
        const unsigned int maxconnections;//!@ja 最大同時接続数 @endja
    protected:
		//!@ja Accepter @endja
        virtual Session *Accepter(VUint32,VUint16){
            if(maxconnections&&GetChildren()>=maxconnections)
                return NULL;
            return enable?new T:NULL;};
    public:
		//!@ja コンストラクタ @endja
		/*!@ja 
		@param maxconn 最大同時接続数
		@endja
		*/
        AutoAccepter(int maxconn=0):maxconnections(maxconn){
            enable=true;}
        //!@ja 今後の接続を許可します。 @endja
        void Enable(){
            enable=true;}
        //!@ja 今後の接続を拒否します。 @endja
        void Disable(){
            enable=false;}
	private:
		AutoAccepter(const AutoAccepter&);
		AutoAccepter& operator=(AutoAccepter&);
		
    };

    //!@ja VCEパケットクラス @endja
    /*!@ja 
    Sessionの派生クラスで、送受信ストリームデータをパース/マージし、パケットデータにする機能を持っています。
    パケット単位でデータを送信(マージ)、および受信(パース)して処理する事ができます。
    例えば5バイトを2回送信すると、ストリームデータのままの場合、受信側では区切り無しに10バイトが一気に受信されます。パケットデータならば、送信側と同じ大きさのデータを一まとまりで受信することができます。
	接続先のセッションもCodecクラスを継承している必要があります。
    @endja
    */
    class Codec
        :public Session
    {
		//!@ja ping-pong時間。ミリ秒単位です。 @endja
		VUint64 ping;
		VUint64 pinginterval;
		VUint64 lastping;
		VUint64 timeout;
		VUint64 lastrecv;
	protected:
        //!@ja 内部で定義されている受信関数 @endja
        size_t Recv(const VUint8 *p,size_t sz);
    public:
        //!@ja パースされたパケットを受信するための関数 @endja
        /*!@ja 
        パケット単位のデータを受信すると呼ばれます。
        @param p 受信データへのポインタ
        @param sz
        @endja
        */
        virtual void Parsed(const VUint8 *p,size_t sz)=0;

		//!@ja 思考関数。継承先で必ずコールする必要があります。 @endja
		void Think();

		//! initialize
		Codec();
		//! uninitialize
		~Codec();
		//!@ja ping-pong時間を取得します @endja
		/*!@ja pingを送りpongが返ってくるまでの時間差をミリ秒単位で返します。定期的に更新されます。
		@return ping
		@endja
		*/
		VUint64 GetPingTime()const;
		//!@ja タイムアウトを設定します @endja
		/*!@ja
		タイムアウト時間内に何もデータを受信しなかったときにCloseします。pingのintervalを設定しておくと、接続先の反応が無くなったときに自動で切断することもできます。
		@param timeout タイムアウトミリ秒時間。0を指定するとタイムアウトしなくなります。初期値は0です。
		@endja
		*/
		void SetTimeoutTime(VUint64 timeout);
		//!@ja pingを送信する間隔を設定します。 @endja
		/*!@ja
		@param interval 間隔をミリ秒で設定します。0にすると更新しなくなります。
		@endja
		*/
		void SetPingIntervalTime(VUint64 interval);
        //!@ja データ送信 @endja
        /*!@ja 
        データを送信するときに使います。szサイズ分のデータが受信側に到達すると、受信側のParsedがコールされます。
        @param p 送信するデータへのポインタ
        @param sz 送信するデータのバイト数
        @return 成功した場合はtrue、失敗した場合はfalse
        @endja
        */
        virtual bool Merge(const void *p,size_t sz);

		enum TERMINATE_REASON
		{
			//!@ja 正常 @endja
			TERMINATE_REASON_NORMAL=0,
			//!@ja 不正 @endja
			TERMINATE_REASON_INVALID=1,
			//!@ja ユーザエリア @endja
			TERMINATE_REASON_USER,

			//!@ja 使われません。 @endja
			TERMINATE_REASON_32BIT=0x7fffffff
		};

		//!@ja 切断 @endja
		/*!@ja
		Session クラスの Close 関数とは異なり、こちらは意図して切断していることが伝わります。
		接続先が VCE_CLOSE_REMOTE のとき、正常終了なのか不正終了なのか判断することができます。
		ただし、 ForceClose 関数を使うと接続先に伝わることなく切断されます。
		@param reason 値を 1 つだけ設定できます。「TERMINATE_REASON_USER+n」のように設定することも可能です。
		@sa Close
		@sa Terminated
		@endja
		*/
		void Terminate(unsigned int reason=TERMINATE_REASON_NORMAL);

		//!@ja 切断されるときに呼ばれるコールバック関数 @endja
		/*!@ja
		切断されるときに呼ばれます。この後 Closed 関数がコールされます。
		@param reason 設定された値です。
		@endja
		*/
		virtual void Terminated(unsigned int reason);

		//!@ja DirectContact を有効にします。 @endja
		/*!@ja
		@param contactid すべてのアプリケーションのセッションで一意なゼロ以外の値
		@param timeouttime 送信パケットがタイムアウトするまでの時間
		@param reliable trueにするとタイムアウトしたときに自動再送します。
		@param debug trueにするとデバッグモードとして動作します。
		@endja
		*/
		bool DirectContactEnable(VUint32 contactid,VUint64 timeouttime=2000,bool reliable=true,bool debug=false);

		//!@ja DirectContact を無効にします。 @endja
		void DirectContactDisable();

		//!@ja 次のパケットのあて先を設定 @endja
		bool DirectContactSetNext(VUint32 contactid);

		//!@ja 受信したパケットの送信元を取得 @endja
		VUint32 DirectContactGetLast();

		enum DIRECTCONTACT_PROGRESS
		{
			//!@ja 送信した @endja
			DIRECTCONTACT_PROGRESS_SEND,
			//!@ja 再送信した @endja
			DIRECTCONTACT_PROGRESS_RESEND,
			//!@ja 送信完了した @endja
			DIRECTCONTACT_PROGRESS_COMPLETE,
			//!@ja タイムアウトした @endja
			DIRECTCONTACT_PROGRESS_TIMEOUT
		};

		struct DirectContactPacketStatus
		{
			//!@ja パケットの一意な ID @endja
			VUint32 transactionID;
			//!@ja あて先 @endja
			VUint32 to;
			//!@ja 送信したときの時間 @endja
			VUint64 timestamp;
		};

		//!@ja DirectContact での経過を受け取るコールバック関数 @endja
		virtual void DirectContactProgress(DIRECTCONTACT_PROGRESS progress,const DirectContactPacketStatus &status);

		/*!@ja
		DirectContact機能が有効になっているかどうか調べます。
		@return 0以外であれば有効です。有効の場合は contactID を返します。
		@endja
		*/
		VUint32 DirectContactIsEnable();

		struct DirectContactNodeStatus
		{
			VUint32 contactID;
			VUint32 hops;
			VUint64 lastResponseTime;
			VUint64 ping;
		};

		/*!@ja
		接続先の状態をすべて取得します。
		デバッグモード以外のときに利用しても何も起こりません。
		@endja
		*/
		vce::g_vector<DirectContactNodeStatus> DirectContactGetNodeStatus();

		/*!@ja
		送信可能な範囲ですべてに Ping などを送り 状態を取得します。
		状態の更新は非同期で行われ、また完了するような区切りもないためしばらくしてから
		DirectContactGetNodeStatus を利用してそれぞれの状態を取得します。
		デバッグモード以外のときに利用しても何も起こりません。
		@endja
		*/
		void DirectContactUpdateNodeStatus();


		void*DirectContactInstance;
    };

	/*!@ja
	複数のセッションをまとめるときは、 Codec の代わりにこのクラスを継承し、 MPISession に渡します。
	@endja
	*/
    class MPINode
        :public Codec
    {
        friend class MPISession;
	private:
		//以下の関数は使えません。MPISessionで使ってください。
		void SetTimeoutTime(VUint64 timeout);
		void SetPingIntervalTime(VUint64 interval);
		void DirectContactEnable(VUint32 contactid,VUint64 timeouttime,bool reliable);
    public:
		//!@ja コンストラクタ @endja
		/*!@ja 
		@param number ノード固有の番号を振ります。
		@endja
		*/
        MPINode(unsigned char number);
		MPINode &operator =(MPINode &);
		//!@ja 親 @endja
        class MPISession *pnode;
		//!@ja 識別子 @endja
        const unsigned char MPINumber;
    public:
		//!@ja 送信関数 @endja
		/*!@ja 
		CodecのMergeと同じように使います。
		@sa Codec::Merge
		@endja
		*/
        bool Merge(const void *p,size_t sz);
    };

	typedef std::map<unsigned char,MPINode*> MPIMap;
	typedef std::map<unsigned char,MPINode*>::iterator MPIMapItr;
    //!@ja 複数セッションをまとめるクラス @endja
    class MPISession
        :public Codec
    {
		//!@ja ノード一覧 @endja
        MPIMap node;
    protected:
		//!@ja CodecのParsedを内部でオーバーライドしています @endja
        void Parsed(const VUint8 *p,size_t sz);
    public:
//      int Merge(const void *p,size_t sz,unsigned char MPInum);
		//!@ja ノード登録 @endja
		/*!@ja 
		@param node 登録するノード
		@return 成功した場合はtrue。
		@endja
		*/
        bool Register(MPINode *node);
		//!@ja 先頭のノード取得 @endja
        /*!@ja
		ノードの先頭のイテレータを取得します。
		@endja
		*/
		MPIMapItr GetNodeBegin() { return node.begin(); }
		//!@ja 末尾のノード取得 @endja
        /*!@ja
		ノードの最後のイテレータを取得します。
		@endja
		*/
		MPIMapItr GetNodeEnd() { return node.end(); }
		//!@ja 特定のクラスのノードを取得 @endja
        /*!@ja 
		取得したいノードのクラスを仮引数に入れて使います。
		@endja
		*/
        template <typename N>N*GetNode(){for(MPIMapItr i=node.begin();i!=node.end();i++)if(dynamic_cast<N*>(i->second))return dynamic_cast<N*>(i->second);return NULL;}
    protected://wrapper
        void Attached();//全nodeのAttachedを呼ぶ
        void Connected();//以下同じ
        void Closed(VCE_CLOSEREASON type);
        void Detached();
        void Exception(VCE_EXCEPTION type);
        void Think();
    };

	//!@ja HTTPヘッダー用定義 @endja
	typedef std::map<std::string,std::string> HTTPHeader;
	//!@ja URIパラメータ用定義 @endja
	typedef std::map<std::string,std::string> URIParam;

	//!@ja 単純なHTTPサーバ用リスナー @endja
	class HTTPServerListener
		:public Listener
	{
	public:
		typedef std::vector<std::string> PasswordList;
		struct authinfo
		{
			PasswordList passwords;
			std::string realm;
		};
		typedef std::map<std::string,authinfo> AuthList;
		typedef std::map<std::string,authinfo>::iterator AuthListItr;
	private:
		//!@ja Basic認証のユーザ管理リスト。ディレクトリ名をキーとし、パスワードのリストを値としたmapです。 @endja
		AuthList authinfos;
	public:
		HTTPServerListener();
		HTTPServerListener&operator=(HTTPServerListener&){return*this;}

		/*!@ja
		許可するユーザを追加します。一つのディレクトリに複数のユーザを設定することも可能です。
		@param dir ディレクトリ
		@param user ユーザ名
		@param password パスワード
		@endja
		*/
		void AddUser(const std::string& dir, const std::string& user, const std::string& password);
		/*!@ja
		認証ダイアログに出す説明を設定します。
		AddUserの実行後に設定します。
		@param dir ディレクトリ
		@param realm 説明
		@endja
		*/
		void SetRealm(const std::string& dir, const std::string& realm);
		/*!@ja
        リクエストが認証済みかチェックします。
        @param header リクエストのヘッダを渡します。
		@return 認証済みならtrue、それ以外ならfalse。
		@endja
		*/
		bool CheckAuth(const std::string& URI, const HTTPHeader &header);
		//!@ja 指定されたディレクトリの認証の説明を取得します。 @endja
		std::string GetRealm(const std::string& URI);
		//!@ja URIからディレクトリ部分を抜き出す @endja
		/*!@ja
		www.hoge/aho/foo/index.htmlのようなURIを与えられたらhoge/aho/foo、hoge/aho、hogeと三つの文字列が返ります。
		最後に拡張子がない場合、そこもディレクトリとして認識されます。
		@endja
		*/
		std::vector<std::string> GetDir(const std::string& URI);
	};

	//!@ja 自動HTTPセッション開始クラス @endja
    /*!@ja 
    接続が来たときに自動でサーバ用HTTPセッションを生成し、返すためのテンプレートクラスです。
    このクラスからリスナーを生成し、Listen関数に引数として渡すだけで動作します。
	@sa net::HTTPServerListening
    @endja
    */
    template <class T>class AutoHTTPAccepter:public HTTPServerListener
    {
		//!@ja 接続を許可しているか @endja
        bool enable;
		//!@ja 最大同時接続数 @endja
        const unsigned int maxconnections;
    protected:
		//!@ja Accepter @endja
        virtual Session *Accepter(VUint32,VUint16){
            if(maxconnections&&GetChildren()>=maxconnections)
                return NULL;
            return enable?new T:NULL;}
    public:
		//!@ja コンストラクタ @endja
		/*!@ja 
		@param maxconn 最大同時接続数
		@endja
		*/
        AutoHTTPAccepter(int maxconn=0):maxconnections(maxconn){
            enable=true;}
        //!@ja 今後の接続を許可します。 @endja
        void Enable(){
            enable=true;}
        //!@ja 今後の接続を拒否します。 @endja
        void Disable(){
            enable=false;}
		AutoHTTPAccepter&operator=(AutoHTTPAccepter&){return*this;}
    };

	//!@ja HTTPサーバのレスポンスの文字コードの種類 @endja
	enum HTTP_CHARSET
	{
		HTTP_CHARSET_UTF_8 = 0,
		HTTP_CHARSET_SJIS = 1,
		HTTP_CHARSET_NONE = 2,
		HTTP_CHARSET_NUM = HTTP_CHARSET_NONE
	};
	//!@ja HTTPサーバのレスポンスのContent-Typeの種類 @endja
	enum HTTP_CONTENTTYPE
	{
		HTTP_CONTENTTYPE_HTML = 0,
		HTTP_CONTENTTYPE_TEXT = 1,
		HTTP_CONTENTTYPE_PLAIN = 2,
		HTTP_CONTENTTYPE_XML = 3,
		HTTP_CONTENTTYPE_CSS = 4,
		HTTP_CONTENTTYPE_NUM = HTTP_CONTENTTYPE_CSS
	};

	//!@ja 単純なHTTPサーバ @endja
    /*!@ja 
	簡単なHTTPサーバを作るときに使います。リクエストに対して高速なレスポンスができます。HTTPSには対応していません。
	Basic認証のチェックはRecv関数内で自動的におこなわれているので、この関数をオーバライドしない限り気にする必要はありません。
    @endja
    */
    class HTTPServerSession
        :public Session
    {
		//!@ja Recvのオーバーライド @endja
		/*!@ja
		受信したリクエストのHTTPヘッダを取り出したりしています。この中でBasic認証のチェックも行っています。
		@sa Session::Recv
		@endja
		*/
        size_t Recv(const vce::VUint8 *p,size_t sz);
    protected:
		static const char *charset[];
		/*!@ja
        クライアントからリクエストを受信すると呼ばれます。
		1つのリクエストに対して必ず1つのレスポンスを返す必要があります。
		デフォルトの動作では、root以下からURIで与えられたファイルを探して返信します。
		@param method HTTPメソッド。"GET"や"POST"など。
		@param URI URI。対象リソースへのアドレスを指定します。
		@param header HTTPヘッダの名前をキーとし、対応する内容を値としたmap。
		@param body データ本体。GETやHEADメソッドの場合は空です。
		@param body_len データ本体のバイト数。
		@return 正常に返信したときはtrue、切断するときはfalse。
		@endja
		*/
        virtual bool Request(const std::string& method,const std::string& URI,const HTTPHeader& header,const char *body,size_t body_len);
		/*!@ja
        クライアントへレスポンスを返すときに呼びます。
		@param status HTTPレスポンスコード。
		@param header ヘッダに必要な要素を入れます。"Content-Length"は自動的に設定されます。
		@param body データ本体。HEADメソッドの場合は空、GETやPOSTのときは内容を返すようにします。
		@param body_len データ本体のバイト数
		@return なし
		@endja
		*/
        void Response(int status,const HTTPHeader& header,const char *body,size_t body_len);
		//!@ja 拡張子から判断してContent-Typeの文字列を返す @endja
		/*!@ja 
		拡張子から判断して"text/html; charset=UTF-8"のようなHTTPヘッダ用の文字列を返します。デフォルトはtext/htmlです。
		@endja
		*/
		std::string GetContentTypeString(const std::string& URI, HTTP_CHARSET cs);
		std::string GetContentTypeString(HTTP_CONTENTTYPE type, HTTP_CHARSET cs);
		/*!@ja 
		ヘッダからユーザ名を取り出します。見つからない場合は空のstringが返ります。
		@param header リクエストのヘッダ
		@return ユーザ名
		@endja
		*/
		std::string GetAuthUser(const HTTPHeader &header);
		/*!@ja 
		認証が失敗したときのレスポンスを返します。内部でResponseが呼ばれます。
		@param realm 認証についての説明
		@endja
		*/
		void ResponseAuth(const std::string& realm);
		/*!@ja 
        リクエストが認証済みかチェックします。
		認証が失敗なら、自動的にResponseAuthを呼ぶことでレスポンスを返します。
        @param header リクエストのヘッダをそのまま渡します。
		@return 認証済みならtrue、それ以外ならfalse。
		@endja
		*/
		bool CheckAuth(const std::string& URI, const HTTPHeader &header);
	public:
		//!@ja ファイルを探す時のルートディレクトリ @endja
		std::string root;
		//!@ja ページリクエスト時の文字コード @endja
		HTTP_CHARSET code;
		typedef std::map<std::string, std::string> ContentTypeMap;
		//!@ja 拡張子と対応するContent-Typeのリスト @endja
		/*!@ja
		最初から"cgi、htm、html、txt"は"text/html"、"js"は"text/plain"、"css"は"text/css"、"xml、xsl"は"text/xml"、
		"jpg"は"image/jpeg"、"png"は"image/png"、"gif"は"image/gif"で登録されています。適宜、追加変更してください。
		@endja
		*/
		ContentTypeMap contenttype;

		HTTPServerSession();
		~HTTPServerSession(){}
		HTTPServerSession&operator=(HTTPServerSession&){return*this;}
    };

	//!@ja 単純なHTTPクライアント用セッション @endja
	class HTTPClientSession
		:public Session
	{
		std::string lastrequest;
		size_t Recv(const vce::VUint8 *p,size_t sz);
	public:
		//!@ja HTTPサーバにリクエストを出します @endja
		/*!@ja
		@param method HTTPメソッド。"GET"や"POST"など。
		@param URI ルートなら"/"を指定します。URIに使えない文字はencodeURIしてください。
		@param header ヘッダに必要な要素を入れます。"Host"と"Content-Length"は自動的に設定されます。
		@param body データ本体。POSTの場合に必要なら入れます。不要であればNULLを設定します。
		@param body_len データ本体のバイト数
		@endja
		*/
		bool Request(const std::string& method,const std::string& URI,const HTTPHeader& header,const char *body,size_t body_len);
	protected:
		//!@ja レスポンスコールバック関数 @endja
		/*!@ja
		レスポンスを処理するときにオーバーライドします。
		@param status HTTPレスポンスコード
		@param header HTTPヘッダのmap
		@param body データ本体
		@param body_len データ本体のバイト数
		@return trueなら接続維持、falseなら切断。
		@endja
		*/
		virtual bool Response(int status,const HTTPHeader& header,const char *body,size_t body_len)=0;
		void Closed(VCE_CLOSEREASON type);
	};

	class MonitorConsole;
	typedef std::vector<vce::MonitorConsole*>::iterator MonitorConsoleListItr;
	class MonitorCustomPage;
	typedef std::vector<vce::MonitorCustomPage*>::iterator MonitorCustomPageListItr;

	//!@ja モニター用リスナー @endja
	/*!@ja
	このクラスの生成はMonitorListenerCreate関数からのみおこなってください。
	インターフェイスのみですが、それぞれの関数をユーザー定義する必要はありません。
	@endja
	*/
	class MonitorListener
		:public HTTPServerListener
	{
	protected:
		MonitorListener();

	public:
		virtual ~MonitorListener();
		//!@ja  MonitorConsoleを登録 @endja
		/*!@ja 
		登録は必須ですが、MonitorConsoleのコンストラクタで呼ばれるので、自分で呼び出す必要はありません。
		@endja
		*/
		virtual void RegisterConsole(MonitorConsole* con)=0;
		//!@ja  MonitorConsoleを登録から削除 @endja
		/*!@ja
		MonitorConsoleのデストラクタで呼ばれるので、自分で呼び出す必要はありません。
		@endja
		*/
		virtual void DeregisterConsole(MonitorConsole* con)=0;
		//!@ja MonitorCustomを登録 @endja
		/*!@ja
		登録は必須ですが、MonitorCustomのコンストラクタで呼ばれるので、自分で呼び出す必要はありません。
		@endja
		*/
		virtual void RegisterCustom(MonitorCustomPage* cus)=0;
		//!@ja MonitorCustomを登録から削除 @endja
		/*!@ja
		MonitorCustomのデストラクタで呼ばれるので、自分で呼び出す必要はありません。
		@endja
		*/
		virtual void DeregisterCustom(MonitorCustomPage* cus)=0;

		//!@ja idからMonitorConsoleを取り出す @endja
		virtual MonitorConsole *GetConsoleFromID(vce::VUint32 consoleID)=0;
		//!@ja idからMonitorCustomPageを取り出す @endja
		virtual MonitorCustomPage *GetCustomPageFromID(vce::VUint32 customID)=0;

		//!@ja nameからMonitorConsoleを取り出す @endja
		virtual vce::MonitorConsole *GetConsoleFromName(const std::string& name)=0;
		//!@ja nameからMonitorCustomPageを取り出す @endja
		virtual vce::MonitorCustomPage *GetCustomPageFromName(const std::string& name)=0;

		//!@ja iterator.begin @endja
		virtual MonitorConsoleListItr GetConsoleBegin()=0;
		//!@ja iterator.end @endja
		virtual MonitorConsoleListItr GetConsoleEnd()=0;
		//!@ja iterator.begin @endja
		virtual MonitorCustomPageListItr GetCustomBegin()=0;
		//!@ja iterator.end @endja
		virtual MonitorCustomPageListItr GetCustomEnd()=0;

		//!@ja テンプレート読み込み @endja
		/*!@ja
		それぞれのページのテンプレートをカレントディレクトリの以下のファイルから読み込みます:
		index.html、status.html、console.html、custom.html、dummy.html。
		@endja
		*/
		virtual void ReadTemplate()=0;
	};

    //!@ja Monitorコンソール @endja
    /*!@ja 
    モニターと関連していろいろ使うためのコンソールです。
	MonitorListenerに登録されているコンソールがwebブラウザから使用できます。
	コンストラクタで登録、デストラクタで登録が解除されます。
	任意のタイミングで登録を解除したい場合はDetachを使用してください。
    @endja
    */
	class MonitorConsole
    {
		//!@ja コンソールID @endja
		vce::VUint32 consoleID;
		vce::MonitorListener* ml;
		//!@ja コンソール名 @endja
		std::string cname;
		//!@ja ページが送信される時の文字コード @endja
		/*!@ja
		HTTPServerSession::codeより優先されます。
		@endja
		*/
		HTTP_CHARSET code;
    public:
		//!@ja コンストラクタ @endja
		MonitorConsole(vce::MonitorListener *parent, const std::string& name = "");
		MonitorConsole&operator=(MonitorConsole&);
		MonitorConsole(MonitorConsole&);
		//!@ja デストラクタ @endja
        virtual ~MonitorConsole();
		//!@ja ID取得 @endja
		inline vce::VUint32 GetID()const{return consoleID;}
		//!@ja コマンドを受け取ったときに呼ばれます。 @endja
		/*!@ja
		cmdはwebブラウザで入力された文字列になります。
		@endja
		*/
        virtual void CommandRequest(const std::string& cmd);
		//!@ja MonitorListenerから切り離します。 @endja
		void Detach();
		//!@ja 名前を取得 @endja
		std::string GetName(){return cname;}
    public:
        //!@ja ログ出力用のストリーム @endja
        std::stringstream clog;
    };

	//!@ja Monitorダミープロトコル @endja
	/*!@ja 
	多重継承とgenの設定によりダミープロトコルを送信できます。
	このクラスはgenによって生成されたセッションクラスに継承させて使います。
	詳しくはチュートリアルを参照してください。
	@endja
	*/
   class MonitorDummyProtocol
    {
		//!@ja ダミープロトコルID @endja
		vce::VUint32 dummyID;
		//!@ja タブなどに表示される名前 @endja
		std::string dpname;
    protected:
		//!@ja コンストラクタ @endja
		MonitorDummyProtocol(const std::string& name = "");
		MonitorDummyProtocol&operator=(MonitorDummyProtocol&);
		MonitorDummyProtocol(MonitorDummyProtocol&);
		//!@ja デストラクタ @endja
        virtual ~MonitorDummyProtocol();
    public:
		//!@ja ID取得 @endja
		inline vce::VUint32 GetID() const { return dummyID; }
		//!@ja ダミープロトコルの名前を設定 @endja
		void SetName(const std::string& name) { dpname = name; }
		//!@ja ダミープロトコルの名前を取得 @endja
		inline std::string GetName() const { return dpname; }
		//!@ja HTTPのPOSTが来たら呼ばれる @endja
		/*!@ja
		定義は自分で書く必要がありますが、genで生成されたセッションのPostRequest関数を呼ぶようにするだけで済みます。
		詳しくはチュートリアルを参照してください。
		@endja
		*/
        virtual void Post(const char *post)=0;
        //!@ja ダミープロトコルページのHTML @endja
		/*!@ja
		この中身はgenが生成してくれます。
		@endja
		*/
        std::stringstream html;
		//!@ja ダミープロトコルページのJavaScript @endja
		/*!@ja
		この中身はgenが生成してくれます。
		@endja
		*/
		std::stringstream js;

		//!@ja 受信したプロトコルひとつ分のデータの保存用構造体 @endja
		struct ProtocolData
		{
			vce::VUint32 time;
			vce::g_string name;
			vce::g_vector<vce::VUint8> data;
		};
		typedef std::vector<ProtocolData> ProtocolDataList;
		typedef std::vector<ProtocolData>::iterator ProtocolDataListItr;
		//!@ja 受信したプロトコルのリスト @endja
		/*!@ja
		プロトコルを受信するとこのリストに追加されます。
		@endja
		*/
		ProtocolDataList recordproto;
		//!@ja 受信したプロトコルのリストをXML形式にする @endja
		/*!@ja
          プロトコル名と受信時間のリストを出力します。
		@param out 生成されるXMLデータ
		@endja
		*/
		void GetRecordProtocolXML(std::stringstream &out);
		//!@ja 受信したプロトコルを再現する @endja
		/*!@ja
		定義は自分で書く必要がありますが、genで生成されたセッションのReplay関数を呼ぶようにするだけで済みます。
		詳しくはチュートリアルを参照してください。
		@endja
		*/
		virtual void ReplayRecords(VUint32 index);
	};

	//!@ja Monitorカスタムページ @endja
	/*!@ja
	アプリケーション独自のぺージを作成できます。
	MonitorListenerに登録されているコンソールがwebブラウザから使用できます。
	コンストラクタで登録、デストラクタで登録が解除されます。
	任意のタイミングで登録を解除したい場合はDetachを使用してください。
	@endja
	*/
    class MonitorCustomPage
    {
		vce::VUint32 customID;
		vce::MonitorListener* ml;
		//!@ja タブなどに表示する名前 @endja
		std::string cpname;
	public:
		//!@ja カスタムページのHTML @endja
        std::stringstream html;
		//!@ja カスタムページ内の自動更新間隔(ミリ秒単位) @endja
		vce::VUint32 interval;
		//!@ja カスタムページ内の自動更新するタグの要素名 @endja
        std::string element;
		//!@ja ブラウザのページキャッシュを無効にするかどうか。 @endja
		bool nocache;
		//!@ja ページが送信される時の文字コード @endja
		/*!@ja
		HTTPServerSession::codeより優先されます。
		@endja
		*/
		HTTP_CHARSET code;
		//!@ja ページが送信される時のContent-Type @endja
		/*!@ja
		HTTPServerSession::typeより優先されます。
		@endja
		*/
		HTTP_CONTENTTYPE type;

		//!@ja コンストラクタ @endja
		MonitorCustomPage(vce::MonitorListener *owner, const std::string& name = "");
		MonitorCustomPage&operator=(MonitorCustomPage&);
		MonitorCustomPage(MonitorCustomPage&);
		//!@ja デストラクタ @endja
        virtual ~MonitorCustomPage();
		//!@ja IDを取得 @endja
		inline vce::VUint32 GetID() const { return customID; }
		//!@ja カスタムページの名前を取得 @endja
		inline std::string GetName() const { return cpname; }
		//!@ja MonitorListenerから切り離す @endja
		void Detach();
		//!@ja ページ独自のPOST処理 @endja
		/*!@ja
		webブラウザであらかじめ用意されているJavaScript(SendCustomProtocolRequest)を呼んだときに呼び出される関数。
		@param post webブラウザから送られてきたリクエストのデータ本体。
		@endja
		*/
        virtual void Post(const char *post)=0;
		//!@ja ページが送信される直前に呼ばれるます。 @endja
		/*!@ja
		正確には、タブの中身を返す直前に呼ばれるコールバック関数。
		リクエストによってページの内容を変更したい時は、この関数の中でhtmlを変更します。
		引数を独自に使いたいときはJavaScriptを書き換えます。デフォルトだとHTTPメソッドは"GET"、paramにはIDが入っています。
		@param method webブラウザから送られてきたリクエストのHTTPメソッド。
		@param param URIのクエリ。
		@param body webブラウザから送られてきたリクエストのデータ本体。
		@endja
		*/
		virtual void PreContentSend(std::string method, vce::URIParam param, std::string body)=0;
		//!@ja カスタムページ内でAjax的なことをする時に呼ばれます。 @endja
		/*!@ja
		intervalメンバに指定された間隔でサーバからPreContentPartSend関数の戻り値を取得し、elementメンバをidに持つHTML要素内に表示します。intervalが0の場合は何もしません。
		@param body webブラウザから送られてきたリクエストのデータ本体。
		@return この戻り値の文字列が送信されます。
		@endja
		*/
		virtual std::string PreContentPartSend(std::string body)=0;
	};

    //!@ja VCEライブラリの初期化 @endja
    /*!@ja 
	 VCE全体で共有する資源を初期化します。
	 VCECreateでVCEオブジェクトを作る前に呼び出します。
	 VCEの利用が終了したらVCEFinalizeを呼び出します。
	 @sa vce::VCEFinalize
	 @endja
	 */
	bool VCEInitialize();

    //!@ja VCEライブラリの終了 @endja
    /*!@ja 
	 vce::VCEInitializeで初期化した共有資源を解放します。
	 通常はプログラムの終了時に呼び出します。
	 @sa vce::VCEInitialize
	 @endja
	 */
	void VCEFinalize();

    //!@ja VCEオブジェクトの生成 @endja
    /*!@ja 
    この関数でまずVCEオブジェクトを作ってAPIを使えるようにする必要があります。複数作ることもできます。
	同時にログをセットすることもできます。
	@sa vce::SetLogger
    @endja
    */
    VCE *VCECreate(std::ostream *target=NULL,int loglevel=VCE_LOGLEVEL_LOW,MemoryAllocator *ma=&defaultallocator);
    //!@ja VCEオブジェクトの削除 @endja
    /*!@ja 
    VCECreateで作られたオブジェクトは必ずこの関数で削除しなければなりません。メモリ解放や各種後処理をします。
    @endja
    */
    void VCEDelete(VCE *n);

	//!@ja MonitorListenerオブジェクトの生成 @endja
	/*!@ja
	MonitorListenerの作成は必ずこの関数を用いてください。
	@endja
	*/
	MonitorListener *MonitorListenerCreate();
	//!@ja MonitorListenerオブジェクトの削除 @endja
	/*!@ja
	MonitorListenerの削除は必ずこの関数を用いてください。
	@endja
	*/
	void MonitorListenerDelete(MonitorListener *ml);


	//!@ja ローカルネットワーク内にマルチキャストするクラス @endja
	class Finder
		:public Base
	{
		//!@ja グループID(マルチキャストIPアドレス) @endja
		unsigned int group;
	public:
		//!@ja コンストラクタ @endja
		/*!@ja
		ローカルネットワーク内の他のアプリケーションを検索します。
		@param group 検索対象のグループID。0xE0000001から0xEFFFFFFFの範囲内に設定しなければなりません。マルチキャストIPアドレスのグループになります。
		@endja
		*/
		Finder(unsigned int group);
		virtual ~Finder();
		//!@ja グループIDを取得します。 @endja
		unsigned int GetGroupID();
		typedef std::map<std::string,std::string> Keywords;
		/*!@ja
		キーワードを設定します。複数の文字列をマップできますが、keyとvalue合わせて最大で約1000バイト分までです。
		@param key 設定するキーワードへの参照。この変数にキーワードを設定します。
		@endja
		*/
		virtual void Keyword(Keywords &key)=0;
        //!@ja 見つかったときに呼ばれるコールバック関数 @endja
		/*!@ja
		自分自身で設定したキーワードもヒットします。
		一定間隔で検索するため何度も同じキーワードがヒットします。
		@param key 見つかったキーワードへの参照。
		@endja
		*/
		virtual void Find(Keywords &key)=0;
		//!@ja 閉じたときに呼ばれるコールバック関数 @endja
		virtual void Closed();

		//!@ja 閉じる @endja
		/*!@ja
		Closeすることで検索をしなくなり、他のFinderにも発見されなくなります。
		@endja
		*/
		bool Close();
	};

	/*!@ja
	TCPリスナー検索用のテンプレートです。
	Session仮引数に接続したいセッションを設定し、
	接続を誘導したいportをコンストラクタで設定してください。
	ローカルネットワーク内のTCPListenerFinderを発見するとConnectします。
	@endja
	*/
	template<typename Session>
	class TCPListenerFinder
		:public Finder
	{
		//!@ja 誘導するポート番号 @endja
		VUint16 port;
	protected:
		//!@ja リスナー誘導用のキーワードを設定します。 @endja
		/*!@ja
		@param key キーワード
		@endja
		*/
		void Keyword(Keywords &key)
		{
			//自分のアドレスとリスニングポートをキーワードに設定する。
			unsigned int arry[4];
			size_t find=vce::GetSelfAddress(arry,4);
			// 0.0.0.0 と 127.0.0.1 以外の最初に現れた値を使う
			for(int i =0; i < find; ++i)
			{
				if ((arry[i] > 0) && (arry[i] != 2130706433)) // not 127.0.0.1
				{
					key["address"] = vce::AddrToString(arry[i]);
					break;
				}
			}	
			key["port"]=ntoa(port);
		}
		//!@ja Findのオーバーライド @endja
		/*!@ja
        リスナー検索用キーを見てConnectします。
        @param key キーワード
		@endja
		*/
		void Find(Keywords &key)
		{
			//キーワードが正しく有るかチェック
			if(!key["address"].empty()&&vce::atoi(key["port"]))
			{
				vce::VUint32 remoteaddr,addr=vce::StringToAddr(key["address"]);
				vce::VUint16 remoteport,port=vce::atoi(key["port"]);
				std::vector<vce::Session*> v;
				api->GetAllSession(v);
				//すでに同じリスナーへのセッションが無いかチェック
				for(size_t i=0;i<v.size();i++)
					if(v[i]->GetRemoteAddress(remoteaddr,remoteport)&&remoteaddr==addr&&remoteport==port)
						return;
				api->Connect(new Session,key["address"],port,2000);
			}
		}
	public:
		//!@ja コンストラクタ @endja
		/*!@ja
		ポートとグループを設定します。
		@param port 接続を誘導するTCPポート番号
		@sa Finder
		@endja
		*/
		TCPListenerFinder(VUint16 port,unsigned int group=0xeff0f1f2)
			:Finder(group)
		{
			this->port=port;
		}
	};

	//!@ja UPnPポート設定をするためのインターフェイスクラス @endja
	class UPnPStatus
	{
	public:
		//!@ja ポートマッピングの状態 @endja
		enum Status
		{
			//!@ja 不明 @endja
			UPnP_Unknown=0,
			//!@ja ルータ検索中 @endja
			UPnP_SearchRouter,
			//!@ja ルータ情報取得中 @endja
			UPnP_GetRouterSpec,
			//!@ja アクション処理中 @endja
			UPnP_ActionWait,
			//!@ja 処理完了。ただし操作が成功したとは限りません。 @endja
			UPnP_Complete,
			//!@ja 失敗 @endja
			UPnP_Fail
		};
		//!@ja UPnP操作の状態を取得します。 @endja
		virtual Status GetUPnPStatus()=0;
		
		//!@ja UPnPの操作が完了するまで待ちます。 @endja
		/*!@ja
		処理が完了するまで内部でVCE::Pollを呼び出します。
		@param timeout 処理がタイムアウトするまでのミリ秒時間。
		@return UPnP_Completeの場合はtrue、それ以外の場合はfalse。
		@endja
		*/
		virtual bool WaitComplete(VUint64 timeout=5000)=0;

		virtual ~UPnPStatus();
	};

	/*!@ja
	  UPnPの操作はスレッドセーフではありません。
	  複数のスレッドで同時にUPnPのAPIを用いることはできません。
	@param port IPマスカレードを設定するポート番号
	@param api VCEオブジェクトへのポインタ
	@return UPnP状態取得用のインターフェイス
	@endja
	*/
	UPnPStatus* UPnPSetPortmapping(VUint16 port,VCE *api);
	/*!@ja
	  UPnPの操作はスレッドセーフではありません。
	  複数のスレッドで同時にUPnPのAPIを用いることはできません。
	@param port IPマスカレードを削除するポート番号
	@param api VCEオブジェクトへのポインタ
	@return UPnP状態取得用のインターフェイス
	@endja
	*/
	UPnPStatus* UPnPDeletePortmapping(VUint16 port,VCE *api);
	//!@ja UPnP操作オブジェクトの削除 @endja
	/*!@ja
	ルータ操作をキャンセルするときや、操作終了後に削除する必要があります。
	UPnPの操作はスレッドセーフではありません。
	複数のスレッドで同時にUPnPのAPIを用いることはできません。
	@endja
	*/
	void DeleteUPnP(UPnPStatus *uppp);

	//!@ja QoSRequest用 @endja
	class UPnPQoSStatus
		:virtual public UPnPStatus
	{
	public:
		//!@ja QoS操作用ハンドルを取得する @endja
		/*!@ja
		Update操作やRelease操作に必要なハンドル番号を取得します。
		状態がUPnP_Complete以外のときに取得しようとすると失敗します。
		@param handle ハンドルを取得するための変数への参照。
		@return 成功した場合はtrue。
		@endja
		*/
		virtual bool GetTrafficHandle(int &handle)=0;
	};

	/*!@ja
	ルータの上り帯域を設定します。

	UPnPの操作はスレッドセーフではありません。
	複数のスレッドで同時にUPnPのAPIを用いることはできません。

	@param bandwidth 上り帯域をkbps単位で指定します。1Mbpsであれば1024です。
	@param api VCEオブジェクトへのポインタ。
	@return UPnP状態取得用のインターフェイス。
	@endja
	*/
	UPnPStatus* UPnPQoSSetUpstreamBandwidth(VUint32 bandwidth,VCE *api);

	/*!@ja
	アプリケーションが占有したい上り帯域を設定します。
	ルータによって設定できる数が決まっているので、アプリケーション終了後にUPnPQoSReleaseTrafficで解放するか、leaseTimeを設定しておくことが望ましいです。
	@param guaranteeBandwidth 帯域を確保するパーセンテージの設定。合計で100%を超えると失敗します。また、必ず設定した帯域が保証されるわけではありません。

	UPnP の操作はスレッドセーフではありません。
	複数のスレッドで同時に UPnP の API を用いることはできません。

	@param address 対象アドレス。0を指定するとローカルアドレスが設定されます。
	@param portStart 対象ポート範囲の開始。
	@param portEnd 対象ポート範囲の終わり。portStartと同じにすると一つのポートのみ設定されます。portStart未満に設定すると操作に失敗します。
	@param prefixLength アドレスマスクビット数。通常は32を指定します。
	@param isSource trueの場合、addressやportをsourceとして、falseの場合destinationとして設定します。
	@param leaseTime 有効にする秒単位の時間。0にすると無期限になります。
	@param api VCEオブジェクトへのポインタ。
	@return UPnP状態取得用のインターフェイス。
	@sa UPnPQoSReleaseTraffic
	@endja
	*/
	UPnPQoSStatus* UPnPQoSRequestTraffic(int guaranteeBandwidth,VUint32 address,VUint32 prefixLength,VUint16 portStart,VUint16 portEnd,bool isSource,VUint32 leaseTime,VCE *api);

	/*!@ja
	UPnPQoSRequestTrafficで設定した値を変更するときに使います。

	UPnP の操作はスレッドセーフではありません。
	複数のスレッドで同時にUPnPのAPIを用いることはできません。

	@param handle UPnPQoSRequestTrafficの返り値オブジェクトから取得したGetTrafficHandleの値を設定します。
	@sa UPnPQoSRequestTraffic
	@endja
	*/
	UPnPStatus* UPnPQoSUpdateTraffic(int handle,int guaranteeBandwidth,VUint32 address,VUint32 prefixLength,VUint16 portStart,VUint16 portEnd,bool isSource,VUint32 leaseTime,VCE *api);

	/*!@ja
	占有している上り帯域を解放します。

	UPnPの操作はスレッドセーフではありません。
	複数のスレッドで同時にUPnPのAPIを用いることはできません。

	@param handle UPnPQoSRequestTrafficの返り値オブジェクトから取得したGetTrafficHandleの値を設定します。
	@sa UPnPQoSRequestTraffic
	@endja
	*/
	UPnPStatus* UPnPQoSReleaseTraffic(int handle,VCE *api);
}

#endif//VCE_VCE2_H

/**@en
@namespace vce
@enden
*/

/**@en
@class vce::DefaultAllocator
@enden
*/

/**@en
@fn void * vce::DefaultAllocator::alloc(size_t sz)
@enden
*/

/**@en
@fn void vce::DefaultAllocator::free(void *p)
@enden
*/

/**@en
@class vce::VCE
@brief VCE main class
@details Do not generate VCE objects directly with this class. Always reference them using VCECreate() return values. When the object is no longer in use, delete it by passing this return value to VCEDelete(). Multiple objects can be created.
@enden
*/

/**@en
@var vce::MemoryAllocator* vce::VCE::ma
@brief Allocator
@enden
*/

/**@en
@fn  vce::VCE::VCE()
@enden
*/

/**@en
@fn virtual vce::VCE::~VCE()
@enden
*/

/**@en
@fn void vce::VCE::Poll()=0
@brief Poll VCE messages.
@details Calling at regular intervals provides access to VCE internal messages. The thread that called this method calls the user-defined virtual functions for each class.
@enden
*/

/**@en
@fn void vce::VCE::SetPollingWait(bool iswait)=0
@brief Enable waiting for VCE message polling.
@details Setting this to true suspends VCE processing when polling has no data to process. Set this to false when it is acceptable for the application using VCE to monopolize the CPU.
@param iswait Wait flag. This defaults to false.
@enden
*/

/**@en
@fn void vce::VCE::SetSelectLimit(unsigned int limit)=0
@brief Limit number of Listeners and Sessions in use.
@details @brief Limit number of Listeners and Sessions in use. The more Sessions there are in use, the longer that it takes for a Poll() call to return, so this provides a means to distribute processing over separate calls. Using a lower number causes Poll() calls to return faster, but can degrade communications response. Using a higher number lengthens Poll() return times, but improves communications response. The default is 64, the maximum possible under Windows.
@param limit Limit
@enden
*/

/**@en
@fn VUint32 vce::VCE::Connect(Session *s, const std::string &hostname, VUint16 port, unsigned int timeout=0)=0
@brief Connect.
@details Connect session s to hostname:port.
@param s Pointer to class derived from Session. VCE2 uses the pointer passed to this function for internal management and detaches it when the object is no longer in use. The object cannot be one already in use.
@param hostname String specifying the remote host name or IP address.
@param port Remote TCP port number.
@param timeout Maximum time, in milliseconds, to attempt connection. Exceeding this time limit triggers a call to Exception(). Specifying 0 loops until connection succeeds or is canceled. 
@return Unique ID or 0 for failure.
@see Session
Session::CancelConnecting
Listen
@enden
*/

/**@en
@fn VUint32 vce::VCE::Connect(Session *s, const vwstring &hostname, VUint16 port, unsigned int timeout=0)=0
@enden
*/

/**@en
@fn VUint32 vce::VCE::UdpConnect(Session *s, const std::string &hostname, VUint16 port, unsigned int timeout=0)=0
@brief Connect with UDP.
@details Connects with UDP instead of TCP. The other end must be a UdpListen server. Specify the UDP port number. Other operations are the same as with Connect().
@see Connect
UdpListen
@enden
*/

/**@en
@fn VUint32 vce::VCE::UdpConnect(Session *s, const vwstring &hostname, VUint16 port, unsigned int timeout=0)=0
@enden
*/

/**@en
@fn VUint32 vce::VCE::LoopbackConnect(Session *s, VUint16 port)=0
@brief Connect to LoopbackListener.
@details Connects to the LoopbackListen port number specified for the same VCE object.
@param port Port number.
@return Session ID. 0 for failure.
@enden
*/

/**@en
@fn VUint32 vce::VCE::Listen(Listener *l, VUint16 port, const std::string &bindaddress="")=0
@brief Wait for connection.
@details Port listens--in other words, waits for connections.
@param l Pointer to Listener object. VCE2 uses the pointer passed to this function for internal management and detaches it when the object is no longer in use. The object cannot be one already in use.
@param port TCP port number for listening.
@param bindaddress IP address for listening. Setting this to NULL specifies all addresses for the object.
@return Unique ID.
@enden
*/

/**@en
@fn VUint32 vce::VCE::UdpListen(Listener *l, VUint16 port, const std::string &bindaddress="")=0
@brief Wait for a UDP connection.
@details Waits for UDP connections instead of TCP ones. Use and operation is the same as with Listen() functions.
@see Listen
UdpConnect
@enden
*/

/**@en
@fn VUint32 vce::VCE::ProxyListen(Listener *l, VUint16 proxyport, const std::string &proxyaddress, const std::string &password="")=0
@brief Listen() for Proxy connections.
@details ProxyListen() connects to a proxy tool and sends it a Listen request. ProxyListen() is not able to connect immediately. It can only connect after the proxy tool is ready. This Listener automatically closes when the proxy tool terminates or the connection with the proxy tool is broken.
@param l Listener object.
@param proxyport Proxy tool's backendport.
@param proxyaddress IP address of the PC running the Proxy tool.
@param password Password for connecting to proxy tool. This is optional if one has not been set.
@enden
*/

/**@en
@fn VUint32 vce::VCE::LoopbackListen(Listener *l, VUint16 port)=0
@brief Loopback Listen
@details This function is only available within the same VCE object.
@param port Port number for listening. This is separate from the TCP and UDP ports.
@return Listener ID for success or 0 for failure.
@enden
*/

/**@en
@fn vce::VUint32 vce::VCE::CreateFinder(vce::Finder *f, vce::VUint16 port=59820, const std::string &bindaddress="", vce::VUint16 range=10)=0
@brief Set up Finder.
@param f Pointer to Finder interface to configure.
@param port UDP port number for Finder.
@param address Address to bind.
@param range Range for retries if the specified port is not available.
@enden
*/

/**@en
@fn Session * vce::VCE::GetSession(VUint32 uID)=0
@brief Get interface object.
@param uID Session or Listener uID.
@return Session, Listener, or NULL for invalid input parameter.
@enden
*/

/**@en
@fn Listener * vce::VCE::GetListener(VUint32 uID)=0
@enden
*/

/**@en
@fn void vce::VCE::GetAllSession(std::vector< Session * > &v)=0
Gets all sessions.
@details Gets all sessions.
@param v Session* vector for allocating session. There is automatic resizing to match the number of sessions.
@enden
*/

/**@en
@fn void vce::VCE::ReUseAddress(bool use)=0
@brief Reuse address and port used for listening.
@details Under Windows, enabling reuse allows Listen() to succeed, even when the server port is not closed by one problem or another. Connections are not accepted, however, until the operating system closes the port. Neither does connecting to a port already in use trigger failure. Under a Unix-based operating system, Listen() succeeds only when the Listener has been closed. It is perfectly acceptable for Sessions derived from the Listener to remain. Attempting to bind to an address and port combination already listening triggers failure.
@param use Setting this to true enables reuse. This defaults to false.
@see Listen
@enden
*/

/**@en
@fn void vce::VCE::SetTCPListenBacklog(unsigned int backlog)=0
@brief Specify the number of TCP Listen sessions to accept.
@details Specifies the number of TCP Listen sessions to accept.  If there are many connections in a short time or the Poll() call spacing after Connect() is too long, a backlog of TCP Listen sessions accumulates in the Accept queue. If attempting simultaneous connections triggers connection failure, try increasing the queue length with this function. The default is 100. This function must be called before starting listening. Some Unix-based operating systems impose upper bounds on this setting. /proc/sys/net/core/somaxconn is the CentOS limit, for example. Solaris provides the following commands for changing this limit: /usr/sbin/ndd -set /dev/tcp tcp_conn_req_max_q and /usr/sbin/ndd -set /dev/tcp tcp_conn_req_max_q0. This value has no effect under Windows.
@param backlog Maximum number of Sessions to accept.
@see Listen
@enden
*/

/**@en
@fn unsigned int vce::VCE::GetUsed()=0
@brief Get number of Listeners and Sessions in use.
@enden
*/

/**@en
@fn bool vce::VCE::IsUsing(Base *iface)=0
@brief Check whether VCE interface is in use.
@details This tells whether a connected Session or listening Listener is still in use. A Session that is not in use can be reused or deleted.
@param iface Interface to investigate.
@return true means in use.
@enden
*/

/**@en
@fn void vce::VCE::SetLogger(std::ostream *target, int level=VCE_LOGLEVEL_LOW)=0
@brief Specify the destination stream for log output.
@details Specify the output destination and level. Viewing all logs requires specifying output destinations for each level. Each level has its own destination. There is only one output destination for each level. Other VCE objects can share these.
@param target Pointer to output destination stream.
@param level Combination of level flags.
@see VCE_LOGLEVEL
@enden
*/

/**@en
@fn std::ostream * vce::VCE::Log(int level=0)=0
@brief Get output destination stream for log.
@details Gets output destination stream for the specified log level.
@param level Log level for which to obtain output destination stream.
@return Log output destination stream. 0 means that the setting is blank.
@enden
*/

/**@en
@fn void vce::VCE::GetMonitorListener(MonitorListenerList &ml)=0
@enden
*/

/**@en
@fn void vce::VCE::SafeDelete(unsigned int timeout)=0
@brief Wait until all object processing is complete.
@details This function continues polling until there are no Connect and Listen objects open. It is then safe to call VCEDelete().
@param timeout Maximum time, in milliseconds, to wait for processing to complete.
@enden
*/

/**@en
@fn VUint8 * vce::VCE::BufferPush(std::size_t size)=0
@brief Allocate a temporary buffer.
@details size This specifies the initial buffer size in bytes. This buffer expands as necessary. VCEDelete() releases all such buffers. The primary application is allocating space to replace a stack when building gen data.
@param size Number of bytes to allocate.
@return Buffer allocated.
@enden
*/

/**@en
@fn void vce::VCE::BufferPop()=0
@brief Deallocate the last buffer allocated.
@details The buffer is not actually deallocated. It remains available for reuse with the next Push().
@enden
*/

/**@en
@fn size_t vce::VCE::BufferSize()=0
@brief Get the total size of all buffers allocated.
@details The value returned includes space allocated for buffers not currently in use.
@enden
*/

/**@en
@fn  vce::VCE::VCE(const VCE &)
@enden
*/

/**@en
@fn VCE & vce::VCE::operator=(VCE &)
@enden
*/

/**@en
@class vce::VCE::AutoPop
@brief Class automatically calling BufferPop().
@enden
*/

/**@en
@var VCE* vce::VCE::AutoPop::api
@enden
*/

/**@en
@fn  vce::VCE::AutoPop::AutoPop(VCE *api)
@enden
*/

/**@en
@fn  vce::VCE::AutoPop::~AutoPop()
@enden
*/

/**@en
@struct vce::CalendarTime
@brief Calendar time
@enden
*/

/**@en
@var int vce::CalendarTime::year
@brief Calendar year.
@enden
*/

/**@en
@var int vce::CalendarTime::month
@brief 1-12
@enden
*/

/**@en
@var int vce::CalendarTime::day
@brief 1-
@enden
*/

/**@en
@var int vce::CalendarTime::hour
@brief 1-24
@enden
*/

/**@en
@var int vce::CalendarTime::minute
@brief 0-59
@enden
*/

/**@en
@var int vce::CalendarTime::second
@enden
*/

/**@en
@var int vce::CalendarTime::milliseconds
@enden
*/

/**@en
@class vce::Random
@enden
*/

/**@en
@var void* vce::Random::context
@enden
*/

/**@en
@fn  vce::Random::Random()
@details Constructor. This initializes the random number generator using the current time as the seed.
@enden
*/

/**@en
@fn  vce::Random::Random(unsigned char *seed, size_t seedsize)
@details Constructor. This initializes the random number generator using the specified seed.
@param seed Pointer to the buffer containing the seed. Using the same seed produces the same random number sequence.
@param seedsize Number of bytes in seed buffer.
@enden
*/

/**@en
@fn  vce::Random::~Random()
@details Destructor
@enden
*/

/**@en
@fn unsigned int vce::Random::rand()
@details Generate the next pseudo-random number.
@return Number generated. The value is between 0 and 2^32.
@enden
*/

/**@en
@class vce::RSAEncrypt
@enden
*/

/**@en
@var void* vce::RSAEncrypt::context
@enden
*/

/**@en
@var Random* vce::RSAEncrypt::random
@enden
*/

/**@en
@fn  vce::RSAEncrypt::RSAEncrypt(const RSAEncrypt &)
@enden
*/

/**@en
@fn RSAEncrypt & vce::RSAEncrypt::operator=(RSAEncrypt &)
@enden
*/

/**@en
@fn  vce::RSAEncrypt::RSAEncrypt()
@enden
*/

/**@en
@fn  vce::RSAEncrypt::RSAEncrypt(Random *rand)
@enden
*/

/**@en
@fn  vce::RSAEncrypt::~RSAEncrypt()
@enden
*/

/**@en
@fn bool vce::RSAEncrypt::GenerateKey(int generatebits)
@details Generate a key. The key length in bits must be a multiple of 128 between 128 and 1024.
@param generatebits Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::RSAEncrypt::SetPublicKey(const unsigned char *key, int keybits)
@details Specify key to use.
@param key Buffer containing key.
@param keybits Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::RSAEncrypt::GetPublicKey(unsigned char *key) const
@details Get key generated.
@param key Buffer to receive key. The number of bytes allocated to this buffer must be at least the key length in bits divided by 8.
@return true for success.
@enden
*/

/**@en
@fn bool vce::RSAEncrypt::Encrypt(unsigned char *buf)
@details Encrypt data. The last byte must be zero. Encryption fails for some combinations of key and data.
@param buf Buffer containing data to encrypt. The number of bytes allocated to this buffer must be at least the key length in bits divided by 8.
@return true for success.
@enden
*/

/**@en
@fn bool vce::RSAEncrypt::Decrypt(unsigned char *buf)
@details Decode encrypted data.
@param buf Buffer containing data to decode.
@return true for success.
@enden
*/

/**@en
@class vce::BlockEncrypt
@details Abstract class for handling encryption. Use this class to manipulate instances. Objects must be based on derived classes.
@enden
*/

/**@en
@fn  vce::BlockEncrypt::BlockEncrypt()
@enden
*/

/**@en
@fn virtual vce::BlockEncrypt::~BlockEncrypt()
@enden
*/

/**@en
@fn bool vce::BlockEncrypt::Initialize(unsigned char *key, int keybitlength)=0
@details Specify key length in bits and initialize key. Camellia supports key lengths of 128 and 256 bits; Blowfish, 96 to 448 bits.
@param key Buffer containing key.
@param keybitlength Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncrypt::Reinitialize()=0
@details Even with the same key, the encryption results change each time because the encryption context changes. Encrypting multiple data streams with the same key requires reinitializing with Reinitialize().
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncrypt::Encrypt(const void *src, size_t srcsize, std::vector< unsigned char > &dest)=0
@details Encrypt data.
@param src Buffer containing data to encrypt.
@param srcsize Number of bytes in buffer.
@param dest Buffer for storing encrypted data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncrypt::Decrypt(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)=0
@details Decode data.
@param src Buffer containing data to decode.
@param dest Buffer for storing decoded data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncrypt::Encrypt(const std::vector< Type > &src, std::vector< unsigned char > &dest)
@details Using a vector allows the specification of various types of encrypted data.
@see Encrypt
@enden
*/

/**@en
@class vce::BlockEncryptCamellia
@enden
*/

/**@en
@var void* vce::BlockEncryptCamellia::context
@enden
*/

/**@en
@var Random* vce::BlockEncryptCamellia::random
@enden
*/

/**@en
@fn  vce::BlockEncryptCamellia::BlockEncryptCamellia()
@details Creates object using Camellia algorithm. The choices for key length are 128 and 256 bits. Objects created with this constructor are not thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptCamellia::BlockEncryptCamellia(Random *rand)
@details This constructor specifies a random number object. Objects created with this constructor are thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptCamellia::~BlockEncryptCamellia()
@enden
*/

/**@en
@fn bool vce::BlockEncryptCamellia::Initialize(unsigned char *key, int keybitlength)
@details Specify key length in bits and initialize key. Camellia supports key lengths of 128 and 256 bits; Blowfish, 96 to 448 bits.
@param key Buffer containing key.
@param keybitlength Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptCamellia::Reinitialize()
@details Even with the same key, the encryption results change each time because the encryption context changes. Encrypting multiple data streams with the same key requires reinitializing with Reinitialize().
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptCamellia::Encrypt(const void *src, size_t srcsize, std::vector< unsigned char > &dest)
@details Encrypt data.
@param src Buffer containing data to encrypt.
@param srcsize Number of bytes in buffer.
@param dest Buffer for storing encrypted data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptCamellia::Decrypt(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)
@details Decode data.
@param src Buffer containing data to decode.
@param dest Buffer for storing decoded data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@class vce::BlockEncryptBlowfish
@enden
*/

/**@en
@var void* vce::BlockEncryptBlowfish::context
@enden
*/

/**@en
@var Random* vce::BlockEncryptBlowfish::random
@enden
*/

/**@en
@fn  vce::BlockEncryptBlowfish::BlockEncryptBlowfish()
@details Creates object using Blowfish algorithm. The choices for key length are 96 to 448 bits. Objects created with this constructor are not thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptBlowfish::BlockEncryptBlowfish(Random *rand)
@details This constructor specifies a random number object. Objects created with this constructor are thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptBlowfish::~BlockEncryptBlowfish()
@enden
*/

/**@en
@fn bool vce::BlockEncryptBlowfish::Initialize(unsigned char *key, int keybitlength)
@details Specify key length in bits and initialize key. Camellia supports key lengths of 128 and 256 bits; Blowfish, 96 to 448 bits.
@param key Buffer containing key.
@param keybitlength Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptBlowfish::Reinitialize()
@details Even with the same key, the encryption results change each time because the encryption context changes. Encrypting multiple data streams with the same key requires reinitializing with Reinitialize().
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptBlowfish::Encrypt(const void *src, size_t srcsize, std::vector< unsigned char > &dest)
@details Encrypt data.
@param src Buffer containing data to encrypt.
@param srcsize Number of bytes in buffer.
@param dest Buffer for storing encrypted data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptBlowfish::Decrypt(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)
@details Decode data.
@param src Buffer containing data to decode.
@param dest Buffer for storing decoded data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@class vce::BlockEncryptRijndael
@enden
*/

/**@en
@var void* vce::BlockEncryptRijndael::context
@enden
*/

/**@en
@var Random* vce::BlockEncryptRijndael::random
@enden
*/

/**@en
@fn  vce::BlockEncryptRijndael::BlockEncryptRijndael()
@details Creates object using Rijndael algorithm. The choices for key length are 128, 192, and 256 bits. Objects created with this constructor are not thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptRijndael::BlockEncryptRijndael(Random *rand)
@details This constructor specifies a random number object. Objects created with this constructor are thread safe.
@enden
*/

/**@en
@fn  vce::BlockEncryptRijndael::~BlockEncryptRijndael()
@enden
*/

/**@en
@fn bool vce::BlockEncryptRijndael::Initialize(unsigned char *key, int keybitlength)
@details Specify key length in bits and initialize key. Camellia supports key lengths of 128 and 256 bits; Blowfish, 96 to 448 bits.
@param key Buffer containing key.
@param keybitlength Key length in bits.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptRijndael::Reinitialize()
@details Even with the same key, the encryption results change each time because the encryption context changes. Encrypting multiple data streams with the same key requires reinitializing with Reinitialize().
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptRijndael::Encrypt(const void *src, size_t srcsize, std::vector< unsigned char > &dest)
@details Encrypt data.
@param src Buffer containing data to encrypt.
@param srcsize Number of bytes in buffer.
@param dest Buffer for storing encrypted data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@fn bool vce::BlockEncryptRijndael::Decrypt(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)
@details Decode data.
@param src Buffer containing data to decode.
@param dest Buffer for storing decoded data. Passed as a reference to a vector array. There is automatic resizing.
@return true for success.
@enden
*/

/**@en
@class vce::CompressBase
@brief Compression interface.
@enden
*/

/**@en
@var void* vce::CompressBase::context
@brief context
@enden
*/

/**@en
@fn  vce::CompressBase::CompressBase()
@brief Do not use this class directly. Generate a derived one instead.
@enden
*/

/**@en
@fn virtual vce::CompressBase::~CompressBase()
@brief Destructor.
@enden
*/

/**@en
@fn bool vce::CompressBase::Compress(const void *src, size_t srcsize, std::vector< unsigned char > &dest)=0
@brief Compress data.
@details Compresses binary data. Compression fails if the data is too complex or the data size is too small. The output data is invalid, so do not pass dest to Uncompress().
@param src Buffer containing data to compress.
@param srcsize Number of bytes in buffer.
@param dest Buffer to receive compressed data. Passed as a reference to a vector array.
@return true for success. false for failure. One reason for false is output data larger than the original data.
@enden
*/

/**@en
@fn bool vce::CompressBase::Compress(const std::vector< T > &src, std::vector< unsigned char > &dest)
@brief Template version of Compress().
@param src Buffer containing data to compress.
@see Compress
@enden
*/

/**@en
@fn bool vce::CompressBase::Uncompress(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)=0
@brief Uncompress.
@param src Buffer containing data compressed with Compress().
@param dest Buffer to receive uncompressed data. Passed as a reference to a vector array.
@return true for success.
@enden
*/

/**@en
@fn bool vce::CompressBase::Uncompress(const std::vector< unsigned char > &src, std::vector< T > &dest)
@brief Template version of Uncompress().
@details This supports vectors of various types, but the type must match that used with Compress().
@enden
*/

/**@en
@fn  vce::CompressBase::CompressBase(const CompressBase &)
@enden
*/

/**@en
@fn CompressBase & vce::CompressBase::operator=(CompressBase &)
@enden
*/

/**@en
@class vce::CompressLZP
@enden
*/

/**@en
@fn  vce::CompressLZP::CompressLZP()
@enden
*/

/**@en
@fn  vce::CompressLZP::~CompressLZP()
@enden
*/

/**@en
@fn bool vce::CompressLZP::Compress(const void *src, size_t srcsize, std::vector< unsigned char > &dest)
@brief Compress data.
@details Compresses binary data. Compression fails if the data is too complex or the data size is too small. The output data is invalid, so do not pass dest to Uncompress().
@param src Buffer containing data to compress.
@param srcsize Number of bytes in buffer.
@param dest Buffer to receive compressed data. Passed as a reference to a vector array.
@return true for success. false for failure. One reason for false is output data larger than the original data.
@enden
*/

/**@en
@fn bool vce::CompressLZP::Uncompress(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)
@brief Uncompress.
@param src Buffer containing data compressed with Compress().
@param dest Buffer to receive uncompressed data. Passed as a reference to a vector array.
@return true for success.
@enden
*/

/**@en
@class vce::CompressZIP
@enden
*/

/**@en
@fn  vce::CompressZIP::CompressZIP()
@enden
*/

/**@en
@fn  vce::CompressZIP::~CompressZIP()
@enden
*/

/**@en
@fn bool vce::CompressZIP::Compress(const void *src, size_t srcsize, std::vector< unsigned char > &dest)
@brief Compress data.
@details Compresses binary data. Compression fails if the data is too complex or the data size is too small. The output data is invalid, so do not pass dest to Uncompress().
@param src Buffer containing data to compress.
@param srcsize Number of bytes in buffer.
@param dest Buffer to receive compressed data. Passed as a reference to a vector array.
@return true for success. false for failure. One reason for false is output data larger than the original data.
@enden
*/

/**@en
@fn bool vce::CompressZIP::Uncompress(const std::vector< unsigned char > &src, std::vector< unsigned char > &dest)
@brief Uncompress.
@param src Buffer containing data compressed with Compress().
@param dest Buffer to receive uncompressed data. Passed as a reference to a vector array.
@return true for success.
@enden
*/

/**@en
@class vce::Base
@brief Class for VCE internal use.
@details This case is for internal management of objects. Users must not use this class directly.
@enden
*/

/**@en
@var void* vce::Base::cache
@brief Working copy of pointer to internal object
@enden
*/

/**@en
@var VUint32 vce::Base::uID
@brief Unique identifier
@enden
*/

/**@en
@var VCE* vce::Base::api
@brief VCE object managing this object
@enden
*/

/**@en
@fn  vce::Base::Base()
@brief Constructor.
@enden
*/

/**@en
@fn virtual vce::Base::~Base()
@brief Destructor.
@enden
*/

/**@en
@fn  vce::Base::Base(const Base &)
@enden
*/

/**@en
@fn Base & vce::Base::operator=(Base &)
@enden
*/

/**@en
@fn void vce::Base::Attached()
@brief Callback for when VCE first receives an object for processing.
@details A call to Connect(), Listen(), and certain other functions also produces a call. Do not delete the object until after the Detached callback.
@see Detached
@enden
*/

/**@en
@fn void vce::Base::Detached()
@brief Callback for when VCE detaches (no longer uses) the object.
@details Such calls arise when VCE no longer uses an object passed as a parameter to Connect(), Listen(), or other function. The object can be safely deleted after this callback.
@see Attached
@enden
*/

/**@en
@fn void vce::Base::Think()
@brief Think function.
@details Calling vce::Poll() also calls this function. Users are free to override this function.
@enden
*/

/**@en
@fn bool vce::Base::operator==(Base &target) const
@brief This operator compares two objects for identity.
@details This compares the individual IDs that VCE maintains for objects in use. true indicates that the two objects are the same Session or Listener. This comparison must use the internal IDs instead of pointers because recycling can assign a new object to the same address as a session since closed.
@enden
*/

/**@en
@fn bool vce::Base::operator!=(Base &target) const
@brief This operator compares two objects for non-identity.
@enden
*/

/**@en
@fn bool vce::Base::IsEqual(Base &target) const
@brief This operator compares two objects for identity.
@enden
*/

/**@en
@fn VCE_STATE vce::Base::GetState() const
@brief Get state.
@details Gets state of specified object. A Session progresses through the following states: PREPROCESS, CONNECTING, ESTABLISHED, and CLOSED. A Listener progresses through the following states: PREPROCESS, LISTENING, and CLOSED. The PREPROCESS state covers name resolution and port binding. The CLOSED state normally causes VCE to immediately delete the object, so this function actually only returns CLOSED while the callback to Closed() is not complete.
@see VCE_STATE
@enden
*/

/**@en
@fn void vce::Base::Exception(VCE_EXCEPTION type)
@brief Get exception.
@details This is called when there is a non-critical exception.
@see VCE_EXCEPTION
@enden
*/

/**@en
@fn int vce::Base::GetSocket() const
@brief Get socket handle.
@enden
*/

/**@en
@fn bool vce::Base::IsUsing()
@brief Check whether VCE is using object internally.
@details true indicates that the object is still in use, so must not be deleted.
@enden
*/

/**@en
@class vce::Listener
@brief VCE Listener class.
@details This class is for defining handling of connection requests to a listening port.
@see vce::Listen
AutoAccepter
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::Listener::ProxyConfigParameter
@enden
*/

/**@en
@fn  vce::Listener::Listener()
@brief Constructor.
@enden
*/

/**@en
@fn virtual vce::Listener::~Listener()
@brief Destructor.
@enden
*/

/**@en
@fn Session * vce::Listener::Accepter(VUint32 remote_addr, VUint16 remote_port)=0
@brief Process connection request.
@details Override this function to return a Session to accept the connection request. Alternatively, return NULL to reject the request.
@param remote_addr Address requesting connection.
@param remote_port Port requesting connection.
@return NULL indicates rejection.
@enden
*/

/**@en
@fn void vce::Listener::Closed()
@brief Callback for after VCE closes the object.
@details Closes the connection port and rejects subsequent connection requests. A UDP Listener cannot exchange data with a Session that it has already accepted. Note that attempting to do so deletes the Session's parent object as well.
@return None.
@enden
*/

/**@en
@fn void vce::Listener::ChildDetached(VUint32 ChildID)
@brief Callback preceding Detached callback for a child Session.
@enden
*/

/**@en
@fn bool vce::Listener::Close()
@brief Close Listener.
@details Terminates Listener and closes the port. Sessions already connected remain.
@enden
*/

/**@en
@fn unsigned int vce::Listener::GetChildren(Session **child, unsigned int array_maxlen)
@brief Get number and list of Sessions.
@details Gets a list of Sessions derived from this Listener.
@param child Array of pointers to child Sessions accepted.
@param array_maxlen Maximum number of elements to put in the array.
@return Number of Sessions or number copied to array.
@enden
*/

/**@en
@fn unsigned int vce::Listener::GetChildren()
@brief Get number of Sessions.
@return Number of Sessions.
@enden
*/

/**@en
@fn VCE_STATE vce::Listener::GetState() const
@brief Get state.
@return State.
@enden
*/

/**@en
@fn void vce::Listener::GetChildren(std::vector< T * > &ch)
@brief Vector version of GetChildren().
@details Passing a vector parameter allows packing it with a list of Sessions. There is automatic resizing.
@param ch Arrays
@enden
*/

/**@en
@fn bool vce::Listener::ProxyConfig(const std::string &function, ProxyConfigParameter parameter)
@brief Modify Proxy configuration.
@param function Name of setting to change.
@param parameter Parameter set.
@return false for failure to send.
@enden
*/

/**@en
@struct vce::UdpStatus
@brief Structure to receive UDP status
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::sendsize
@brief Bytes in send attempt, including headers.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::sendcount
@brief Number of send attempts.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::resendsize
@brief Number of bytes in resend attempt.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::resendcount
@brief Number of resend attempts.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::recvsize
@brief Bytes received, including headers.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::recvcount
@brief Number of packets received.
@enden
*/

/**@en
@var VUint32 vce::UdpStatus::dropcount
@brief Number of unnecessary packets received.
@enden
*/

/**@en
@class vce::Session
@brief VCE stream session class.
@details Use this class to manipulate a Session by overriding virtual functions in classes derived from this one.
@see VCE::Connect
Listener::Accepter
@enden
*/

/**@en
@var unsigned int vce::Session::llflags
@brief Flags
@enden
*/

/**@en
@var Listener* vce::Session::parent
@brief Pointer to Listener
@details Pointer to the parent's Listener—for passive connections only. For an active connection, set this to NULL.
@enden
*/

/**@en
@var enum vce::Session::ConnectingStatus vce::Session::connectingstatus
@brief Status of connection attempt
@details Progress indicator for connection attempt
@enden
*/

/**@en
@enum vce::Session::LowLevelFlags
@brief Low level Session flags configured with constructor.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_NONE
@brief Nothing at all.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_ENCRYPT_CAMELLIA
@brief Camellia encryption.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_ENCRYPT_BLOWFISH_128BIT
@brief Blowfish encryption.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_ENCRYPT_RIJNDAEL
@brief Rijndael encryption.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_ENCRYPT_DEFAULT
@brief VCE2 default encryption.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_COMPRESS_LZP
@brief LZP compression.
@enden
*/

/**@en
@var vce::Session::LowLevelFlags vce::Session::LLF_COMPRESS_ZIP
@brief ZIP compression.
@enden
*/

/**@en
@enum vce::Session::ConnectingStatus
@brief Status of connection attempt
@enden
*/

/**@en
@var vce::Session::ConnectingStatus vce::Session::CS_WAIT
@enden
*/

/**@en
@var vce::Session::ConnectingStatus vce::Session::CS_SUCCESS
@enden
*/

/**@en
@var vce::Session::ConnectingStatus vce::Session::CS_FAIL
@enden
*/

/**@en
@fn  vce::Session::Session()
@brief Constructor.
@enden
*/

/**@en
@fn virtual vce::Session::~Session()
@brief Destructor.
@enden
*/

/**@en
@fn void vce::Session::Exception(vce::VCE_EXCEPTION type)
@brief Get exception.
@details This is called when there is a non-critical exception.
@see VCE_EXCEPTION
@enden
*/

/**@en
@fn void vce::Session::ForceClose()
@brief Force Session to close.
@details Immediately closes Session.
@see Close
Closed
@enden
*/

/**@en
@fn void vce::Session::Close()
@brief Close Session.
@details Disconnection is not immediate. The sender closes when the send buffer is empty; the receiver, when receive operations are complete. This is for use, for example, when only a single request is issued and responding is all that is necessary.
@see ForceClose
Closed
@enden
*/

/**@en
@fn void vce::Session::CancelConnecting()
@brief Cancel connection attempt.
@details Use this to cancel a Connect() attempt without waiting for a timeout. Failure passes VCE_EXCEPT_CONNECTFAIL to the Exception() function.
@see VCE::Connect
@enden
*/

/**@en
@fn bool vce::Session::BlockConnecting()
@brief Enable blocking.
@details Enables blocking until the connection completes or the attempt fails. Forgetting to call base class functions Exception() and Connected() from derived classes sometimes leads to abnormal operation.
@return true for a successful connection.
@see VCE::Connect
@enden
*/

/**@en
@fn bool vce::Session::Send(const void *p, size_t sz)
@brief Send data.
@details Copies data into the send buffer and sends it.
@param p Pointer to head of send data.
@param sz Number of bytes to send.
@return true for success; false for failure.
@enden
*/

/**@en
@fn const VUint8 * vce::Session::GetReadbuf(size_t &size)
@brief Reference receive buffer data.
@details This function is for directly referencing receive buffer contents.
@param size Number of bytes in receive buffer.
@return Pointer to head of receive buffer; NULL for failure.
@see EraseReadbuf
@enden
*/

/**@en
@fn void vce::Session::EraseReadbuf(size_t size)
@brief Delete receive buffer data.
@details Deletes data from the beginning of the receive buffer.
@param size Number of bytes to delete.
@see GetReadbuf
@enden
*/

/**@en
@fn void vce::Session::Connected()
@brief Callback for when connection is complete.
@enden
*/

/**@en
@fn void vce::Session::Closed(VCE_CLOSEREASON type)
@brief Callback for when connection is closed.
@enden
*/

/**@en
@fn size_t vce::Session::Recv(const VUint8 *p, size_t sz)
@brief Callback for when data is received.
@details Override this virtual function to process data received.
@param p Pointer to receive data.
@param sz Number of bytes in receive buffer.
@return Number of bytes processed. 0 indicates no change in receive buffer contents. A positive number gives the number of bytes deleted from the beginning of the buffer.
@enden
*/

/**@en
@fn bool vce::Session::GetRemoteAddress(VUint32 &addr, VUint16 &port)
@brief Get remote IP address.
@param addr Reference to variable for storing IP address.
@param port Reference to variable for storing port number.
@return true for success; false for failure.
@enden
*/

/**@en
@fn std::string vce::Session::GetRemoteHostname() const
@brief Get remote host name.
@details Gets the remote host name used when the connection was established. It is not available if getting it requires reverse lookup, functionality which is not supported.
@return Empty string if the host name is not available.
@enden
*/

/**@en
@fn bool vce::Session::SetNodelay(bool enable=true)
@brief Enable Nodelay.
@details Trying to send detailed data in a short interval via a TCP stream entails much greater packet header overhead. As a result, the standard configuration is to disable Nodelay and use the Nagle algorithm to buffer detailed data, accumulate it over a fixed interval, and thus send it more efficiently in bigger amounts. Because this buffering introduces delays, however, enable Nodelay for time-sensitive data. This function is available after the callback to Connected(). Note, however, that some environment settings do not allow changing this setting.
@param enable Setting this to true enables Nodelay.
@return true for success.
@enden
*/

/**@en
@fn bool vce::Session::GetLocalAddress(VUint32 &addr, VUint16 &port) const
@brief Get local IP address.
@param addr Reference to variable for storing IP address.
@param port Reference to variable for storing port number.
@return false for failure.
@enden
*/

/**@en
@fn size_t vce::Session::GetAvailableSendBuffer() const
@brief Get send buffer space available.
@details Monitoring availability allows a critical code section to limit the send volume, prevent automatic send buffer expansion, and thus avoid the performance hit associated with memory reallocation.
@return Send buffer space available.
@enden
*/

/**@en
@fn bool vce::Session::SetSendBufferSize(size_t size, bool variable=true, size_t capacity=1024 *1024 *16)
@brief Specify send buffer size.
@details Specifies the send buffer size. It also specifies whether the buffer has a variable length, and, if so, an upper bound for that length. This function is available after the Connected() callback. The size setting does not take effect immediately with the constructor. The full amount specified is sometimes not available after data exchanges commence.
@param size Buffer size.
@param variable Specifies whether the buffer has a variable length. Setting this to true allows the buffer to grow when the data volume is too high and to shrink as the load decreases over time. Expansions never exceed the specified maximum size, however.
@param capacity Upper bound, in bytes, for buffer expansion. This value must be larger than that for the buffer size.
@return true for success.
@enden
*/

/**@en
@fn bool vce::Session::SetRecvBufferSize(size_t size, bool variable=true, size_t capacity=1024 *1024 *16)
@brief Specify receive buffer size.
@details Specifies the receive buffer size. It also specifies whether the buffer has a variable length, and, if so, an upper bound for that length. This function is available after the Connected() callback. The size setting does not take effect immediately with the constructor. The full amount specified is sometimes not available after data exchanges commence.
@param size Buffer size.
@param variable Specifies whether the buffer has a variable length. Setting this to true allows the buffer to grow when the data volume is too high and to shrink as the load decreases over time. Expansions never exceed the specified maximum size, however.
@param capacity Upper bound, in bytes, for buffer expansion. This value must be larger than that for the buffer size.
@return true for success.
@enden
*/

/**@en
@fn size_t vce::Session::GetSendBufferSize() const
@brief Get send buffer size.
@details Gets the send buffer size.
@return Send buffer size
@enden
*/

/**@en
@fn int vce::Session::GetSendBufferVariable() const
@brief Check whether send buffer is variable length.
@details Checks whether the send buffer is variable length.
@return 0 indicates a fixed-length send buffer.
@enden
*/

/**@en
@fn size_t vce::Session::GetSendBufferCapacity() const
@brief Get send buffer maximum size.
@details Gets the maximum size for the send buffer.
@return Send buffer maximum size.
@enden
*/

/**@en
@fn size_t vce::Session::GetRecvBufferSize() const
@brief Get receive buffer size.
@details Gets the receive buffer size.
@return Receive buffer size
@enden
*/

/**@en
@fn int vce::Session::GetRecvBufferVariable() const
@brief Check whether receive buffer is variable length.
@details Checks whether the receive buffer is variable length.
@return 0 indicates a fixed-length receive buffer.
@enden
*/

/**@en
@fn size_t vce::Session::GetRecvBufferCapacity() const
@brief Get receive buffer maximum size.
@details Gets the maximum size for the receive buffer.
@return Receive buffer maximum size.
@enden
*/

/**@en
@fn bool vce::Session::UdpSetTimeoutTime(int time)
@brief For UDP only. Specify timeout limit.
@details Specifies a limit for processing. Failure to receive data within this time limit suggests a problem at the other end, so disconnect the session. This function is available after the Connected() callback. A TCP application can generally rely on operating system postprocessing to force a proper close, but a UDP one has to ensure the close with a timeout.
@param time Time limit in milliseconds. The default is 20000 ms.
@return true for success.
@enden
*/

/**@en
@fn bool vce::Session::UdpSetResendTime(int time)
@brief For UDP only. Specify packet resend spacing.
@details Specifies the time to wait before resending a packet which the other end does not acknowledge. This function is available after the Connected() callback.
@param time Time limit in milliseconds. The default is 1000 ms.
@return true for success.
@enden
*/

/**@en
@fn bool vce::Session::UdpSetTargetBufferSize(int size)
@brief For UDP only. Specify volume of simultaneous data exchanges.
@details Specifies the volume that the other end sends without signals. A large setting increases the limits for simultaneously exchanges, but at the risk of higher losses and slower responses. A smaller setting is not suitable for high-volume exchanges, but it can reduce the loss ratio and boost responsiveness. This functionality is similar to a TCP receive window buffer. This function is available after the Connected() callback.
@param size Buffer size in bytes. The actual size used can be somewhat bigger to accommodate packet sizes and other factors. The default is 5720.
@return true for success.
@enden
*/

/**@en
@fn bool vce::Session::UdpGetStatus(UdpStatus &status) const
@brief For UDP only. Get UDP status.
@details Gets information peculiar to UDP sessions.
@param status Reference to UdpStatus structure.
@return true for success; false for failure.
@see UdpStatus
@enden
*/

/**@en
@fn VCE_STATE vce::Session::GetState() const
@brief Get state.
@details Gets state of specified object. A Session progresses through the following states: PREPROCESS, CONNECTING, ESTABLISHED, and CLOSED. A Listener progresses through the following states: PREPROCESS, LISTENING, and CLOSED. The PREPROCESS state covers name resolution and port binding. The CLOSED state normally causes VCE to immediately delete the object, so this function actually only returns CLOSED while the callback to Closed() is not complete.
@see VCE_STATE
@enden
*/

/**@en
@class vce::AutoAccepter
@brief Automatic session starting class.
@details This template class responds to a connection request by automatically generating and returning a server Session. This class is for generating a Listener and passing it as a parameter to Listen().
@see net::Listen
@enden
*/

/**@en
@var bool vce::AutoAccepter< T >::enable
@enden
*/

/**@en
@var const unsigned int vce::AutoAccepter< T >::maxconnections
@brief Enable/disable connections.
@enden
*/

/**@en
@fn Session * vce::AutoAccepter::Accepter(VUint32, VUint16)
@brief Maximum number of connections.
@details Accepter
@enden
*/

/**@en
@fn  vce::AutoAccepter::AutoAccepter(int maxconn=0)
@brief Constructor.
@param maxconn Maximum number of simultaneous connections.
@enden
*/

/**@en
@fn void vce::AutoAccepter::Enable()
@brief Enable connections.
@enden
*/

/**@en
@fn void vce::AutoAccepter::Disable()
@brief Disable further connections.
@enden
*/

/**@en
@fn  vce::AutoAccepter::AutoAccepter(const AutoAccepter &)
@enden
*/

/**@en
@fn AutoAccepter & vce::AutoAccepter::operator=(AutoAccepter &)
@enden
*/

/**@en
@class vce::Codec
@brief VCE packet class.
@details Derived from Session, this class converts between stream data and data packets, parsing the incoming receive stream and merging outgoing send packets. It merges outgoing data into packets and parses received data. Sending two packets of five bytes each, for example, produces an unbroken data stream of ten bytes at the receiver. In other words, the receiver sees not the constituent packets, but the same data units as the sender. The remote Session class must also be derived from the Codec class.
@enden
*/

/**@en
@var VUint32 vce::Codec::ping
@brief ping interval in milliseconds.
@enden
*/

/**@en
@var VUint32 vce::Codec::pinginterval
@enden
*/

/**@en
@var VUint32 vce::Codec::lastping
@enden
*/

/**@en
@var VUint32 vce::Codec::timeout
@enden
*/

/**@en
@var VUint32 vce::Codec::lastrecv
@enden
*/

/**@en
@fn size_t vce::Codec::Recv(const VUint8 *p, size_t sz)
@brief Internally defined Receive function.
@enden
*/

/**@en
@fn void vce::Codec::Parsed(const VUint8 *p, size_t sz)=0
@brief Called for incoming parsed packet.
@details Called for each packet received.
@param p Pointer to receive data.
@param sz
@enden
*/

/**@en
@fn void vce::Codec::Think()
@brief Think function. Inherited functions must call this function.
@enden
*/

/**@en
@fn  vce::Codec::Codec()
@brief Initialize.
@enden
*/

/**@en
@fn  vce::Codec::~Codec()
@brief Destructor.
@enden
*/

/**@en
@fn VUint32 vce::Codec::GetPingTime() const
@brief Get ping-pong time.
@details Gets the time difference, in milliseconds, between an outgoing ping and the return pong. This measurement is updated at regular intervals.
@return ping
@enden
*/

/**@en
@fn void vce::Codec::SetTimeoutTime(VUint32 timeout)
@brief Specify timeout limit.
@details Failure to receive data within this time limit closes the Codec. Specifying a ping interval enables automatic disconnection if there is no response from the other end.
@param timeout Maximum time, in milliseconds. Setting this to 0 disables timer. The default is 0.
@enden
*/

/**@en
@fn void vce::Codec::SetPingIntervalTime(VUint32 interval)
@brief Specify the interval for pinging.
@param interval ping interval in milliseconds. Setting this to 0 disables updates.
@enden
*/

/**@en
@fn bool vce::Codec::Merge(const void *p, size_t sz)
@brief Send data.
@details Use this to send data. The receiver calls its Parsed() after it receives sz bytes of data.
@param p Pointer to send data.
@param sz Number of bytes to send.
@return true for success; false for failure.
@enden
*/

/**@en
@class vce::MPINode
@details To bundle multiple Sessions, derive the instance from this class instead of Codec and pass it to MPISession.
@enden
*/

/**@en
@var class MPISession* vce::MPINode::pnode
@brief parent
@enden
*/

/**@en
@var const unsigned char vce::MPINode::MPINumber
@brief Identifier
@enden
*/

/**@en
@fn void vce::MPINode::SetTimeoutTime(VUint32 timeout)
@brief Specify timeout limit.
@details Failure to receive data within this time limit closes the Codec. Specifying a ping interval enables automatic disconnection if there is no response from the other end.
@param timeout Maximum time, in milliseconds. Setting this to 0 disables timer. The default is 0.
@enden
*/

/**@en
@fn void vce::MPINode::SetPingIntervalTime(VUint32 interval)
@brief Specify the interval for pinging.
@param interval ping interval in milliseconds. Setting this to 0 disables updates.
@enden
*/

/**@en
@fn  vce::MPINode::MPINode(unsigned char number)
@brief Constructor.
@param number Unique ID for node.
@enden
*/

/**@en
@fn MPINode & vce::MPINode::operator=(MPINode &)
@enden
*/

/**@en
@fn bool vce::MPINode::Merge(const void *p, size_t sz)
@brief send function.
@details This is equivalent to Merge() for the Codec class.
@see Codec::Merge
@enden
*/

/**@en
@class vce::MPISession
@brief Class for bundling multiple sessions.
@enden
*/

/**@en
@var MPIMap vce::MPISession::node
@brief List of nodes.
@enden
*/

/**@en
@fn void vce::MPISession::Parsed(const VUint8 *p, size_t sz)
@brief This internally overrides Codec::Parsed.
@enden
*/

/**@en
@fn void vce::MPISession::Attached()
@brief Callback for when VCE first receives an object for processing.
@details A call to Connect(), Listen(), and certain other functions also produces a call. Do not delete the object until after the Detached callback.
@see Detached
@enden
*/

/**@en
@fn void vce::MPISession::Connected()
@brief Callback for when connection is complete.
@enden
*/

/**@en
@fn void vce::MPISession::Closed(VCE_CLOSEREASON type)
@brief Callback for when connection is closed.
@enden
*/

/**@en
@fn void vce::MPISession::Detached()
@brief Callback for when VCE detaches (no longer uses) the object.
@details Such calls arise when VCE no longer uses an object passed as a parameter to Connect(), Listen(), or other function. The object can be safely deleted after this callback.
@see Attached
@enden
*/

/**@en
@fn void vce::MPISession::Exception(VCE_EXCEPTION type)
@brief Get exception.
@details This is called when there is a non-critical exception.
@see VCE_EXCEPTION
@enden
*/

/**@en
@fn void vce::MPISession::Think()
@brief Think function. Inherited functions must call this function.
@enden
*/

/**@en
@fn bool vce::MPISession::Register(MPINode *node)
@brief Register node.
@param node Node to register.
@return true for success.
@enden
*/

/**@en
@fn MPIMapItr vce::MPISession::GetNodeBegin()
@brief Get first node.
@details Iterates to first node.
@enden
*/

/**@en
@fn MPIMapItr vce::MPISession::GetNodeEnd()
@brief Get last node.
@details Iterates to last node.
@enden
*/

/**@en
@fn N * vce::MPISession::GetNode()
@brief Get node for specified class.
@details The parameter specifies the class of the desired node.
@enden
*/

/**@en
@class vce::HTTPServerListener
@brief Listener for simple HTTP server.
@enden
*/

/**@en
@var AuthList vce::HTTPServerListener::authinfos
@brief User management list for basic authentication. This key-value map is for retrieving passwords for individual directories.
@enden
*/

/**@en
@typedef std::vector<std::string> vce::HTTPServerListener::PasswordList
@enden
*/

/**@en
@typedef std::map<std::string, authinfo> vce::HTTPServerListener::AuthList
@enden
*/

/**@en
@typedef std::map<std::string, authinfo>::iterator vce::HTTPServerListener::AuthListItr
@enden
*/

/**@en
@fn  vce::HTTPServerListener::HTTPServerListener()
@enden
*/

/**@en
@fn HTTPServerListener & vce::HTTPServerListener::operator=(HTTPServerListener &)
@enden
*/

/**@en
@fn void vce::HTTPServerListener::AddUser(const std::string &dir, const std::string &user, const std::string &password)
@details Adds authorized user. A single directory can have multiple users.
@param dir Directory.
@param user User name.
@param password Password.
@enden
*/

/**@en
@fn void vce::HTTPServerListener::SetRealm(const std::string &dir, const std::string &realm)
@details Specifies the descriptive text displayed in the authentication dialog box. Specify this after AddUser().
@param dir Directory.
@param realm Descriptive text.
@enden
*/

/**@en
@fn bool vce::HTTPServerListener::CheckAuth(const std::string &URI, const HTTPHeader &header)
@details Checks whether request is authorized.
@param header Request header.
@return true for authorized; false otherwise.
@enden
*/

/**@en
@fn std::string vce::HTTPServerListener::GetRealm(const std::string &URI)
@brief Get authorization descriptive text for specified directory.
@enden
*/

/**@en
@fn std::vector< std::string > vce::HTTPServerListener::GetDir(const std::string &URI)
@brief Extract directories from URI.
@details For the URI www.hoge/aho/foo/index.html, for example, this returns three strings: hoge/aho/foo, hoge/aho, and hoge. If there is no file extension at the end, that portion is also considered to be a directory.
@enden
*/

/**@en
@struct vce::HTTPServerListener::authinfo
@enden
*/

/**@en
@var PasswordList vce::HTTPServerListener::authinfo::passwords
@enden
*/

/**@en
@var std::string vce::HTTPServerListener::authinfo::realm
@enden
*/

/**@en
@class vce::AutoHTTPAccepter
@brief Automatic session starting class.
@details This template class responds to a connection request by automatically generating and returning an HTTP server Session. This class is for generating a Listener and passing it as a parameter to Listen().
@see net::HTTPServerListening
@enden
*/

/**@en
@var bool vce::AutoHTTPAccepter< T >::enable
@brief Enable/disable connections.
@enden
*/

/**@en
@var const unsigned int vce::AutoHTTPAccepter< T >::maxconnections
@brief Maximum number of connections.
@enden
*/

/**@en
@fn Session * vce::AutoHTTPAccepter::Accepter(VUint32, VUint16)
@brief Accepter
@enden
*/

/**@en
@fn  vce::AutoHTTPAccepter::AutoHTTPAccepter(int maxconn=0)
@brief Constructor.
@param maxconn Maximum number of simultaneous connections.
@enden
*/

/**@en
@fn void vce::AutoHTTPAccepter::Enable()
@brief Enable connections.
@enden
*/

/**@en
@fn void vce::AutoHTTPAccepter::Disable()
@brief Disable further connections.
@enden
*/

/**@en
@fn AutoHTTPAccepter & vce::AutoHTTPAccepter::operator=(AutoHTTPAccepter &)
@enden
*/

/**@en
@class vce::HTTPServerSession
@brief Simple HTTP server.
@details Use this to create a simple HTTP server. This server responds rapidly to requests. HTTPS is not supported. Recv() automatically provides a basic authentication check, so the application only needs to provide one if it overrides this function.
@enden
*/

/**@en
@var std::string vce::HTTPServerSession::root
@brief Root (starting) directory for file searches.
@enden
*/

/**@en
@var HTTP_CHARSET vce::HTTPServerSession::code
@brief Encoding for page request.
@enden
*/

/**@en
@var ContentTypeMap vce::HTTPServerSession::contenttype
@brief List mapping file extension to Content-Type.
@details The initial list maps .cgi, .htm, .html, and .txt to "text/html"; .js to "text/plain"; .css to "text/css"; .xml and .xsl to "text/xml"; .jpg to "image/jpeg"; .png to "image/png"; and .gif to "image/gif". Expand or modify this list as appropriate.
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::HTTPServerSession::ContentTypeMap
@enden
*/

/**@en
@fn size_t vce::HTTPServerSession::Recv(const vce::VUint8 *p, size_t sz)
@brief Override for Recv().
@details Extracts HTTP headers from request received, etc. It also checks basic authentication.
@see Session::Recv
@enden
*/

/**@en
@fn bool vce::HTTPServerSession::Request(const std::string &method, const std::string &URI, const HTTPHeader &header, const char *body, size_t body_len)
@details Called when client receives a request. Each request always requires some response or other The default operation is to retrieve, starting at root, the file specified by URI.
@param method HTTP method: GET or POST, for example.
@param URI URI. Address for the resource.
@param header This key-value map lists the HTTP headers as name-content pairs.
@param body Data body. This is empty for GET, HEAD, and other methods.
@param body_len Number of bytes in data body.
@return true for a successful reply; false for disconnection.
@enden
*/

/**@en
@fn void vce::HTTPServerSession::Response(int status, const HTTPHeader &header, const char *body, size_t body_len)
@details Called when returning a response to a client.
@param status HTTP response code.
@param header Structure containing elements for header. The element Content-Length is automatically supplied.
@param body Data body. This holds the content returned by GET, POST, and other methods. It is empty for the HEAD method.
@param body_len Number of bytes in data body.
@return None.
@enden
*/

/**@en
@fn std::string vce::HTTPServerSession::GetContentTypeString(const std::string &URI, HTTP_CHARSET cs)
@brief Return Content-Type string corresponding to file extension.
@details Returns, based on the file extension, an HTTP header similar to the "text/html; charset=UTF-8" one for HTML files. "text/html" is the default.
@enden
*/

/**@en
@fn std::string vce::HTTPServerSession::GetContentTypeString(HTTP_CONTENTTYPE type, HTTP_CHARSET cs)
@enden
*/

/**@en
@fn std::string vce::HTTPServerSession::GetAuthUser(const HTTPHeader &header)
@details Extracts the user name from the header. An empty string means NOT FOUND.
@param header Request header.
@return User name.
@enden
*/

/**@en
@fn void vce::HTTPServerSession::ResponseAuth(const std::string &realm)
@details Returns response for authorization failure. Internally, this calls Response().
@param realm Descriptive text displayed in the authentication dialog box.
@enden
*/

/**@en
@fn bool vce::HTTPServerSession::CheckAuth(const std::string &URI, const HTTPHeader &header)
@details Checks whether request is authorized. Authorization failure automatically calls ResponseAuth() to return a response.
@param header Request header passed as is.
@return true for authorized; false otherwise.
@enden
*/

/**@en
@fn  vce::HTTPServerSession::HTTPServerSession()
@enden
*/

/**@en
@fn  vce::HTTPServerSession::~HTTPServerSession()
@enden
*/

/**@en
@fn HTTPServerSession & vce::HTTPServerSession::operator=(HTTPServerSession &)
@enden
*/

/**@en
@class vce::HTTPClientSession
@brief Session for simple HTTP client.
@enden
*/

/**@en
@var std::string vce::HTTPClientSession::lastrequest
@enden
*/

/**@en
@fn size_t vce::HTTPClientSession::Recv(const vce::VUint8 *p, size_t sz)
@brief Callback for when data is received.
@details Override this virtual function to process data received.
@param p Pointer to receive data.
@param sz Number of bytes in receive buffer.
@return Number of bytes processed. 0 indicates no change in receive buffer contents. A positive number gives the number of bytes deleted from the beginning of the buffer.
@enden
*/

/**@en
@fn bool vce::HTTPClientSession::Request(const std::string &method, const std::string &URI, const HTTPHeader &header, const char *body, size_t body_len)
@brief Send a request to an HTTP server.
@param method HTTP method: GET or POST, for example.
@param URI URI "/" for root, for example. Use EncodeURI() if the string contains characters not suitable for URIs.
@param header Structure containing elements for header. The elements Host and Content-Length are automatically supplied.
@param body Data body. This is for POST, if necessary. Otherwise, set it to NULL.
@param body_len Number of bytes in data body.
@enden
*/

/**@en
@fn bool vce::HTTPClientSession::Response(int status, const HTTPHeader &header, const char *body, size_t body_len)=0
@brief Callback for response.
@details This is for overriding response processing.
@param status HTTP response code.
@param header HTTP header map.
@param body Data body.
@param body_len Number of bytes in data body.
@return true for valid connection; false for a disconnection.
@enden
*/

/**@en
@fn void vce::HTTPClientSession::Closed(VCE_CLOSEREASON type)
@brief Callback for when connection is closed.
@enden
*/

/**@en
@class vce::MonitorListener
@brief Listener for Monitor.
@details Always use MonitorListenerCreate() to create instances of this class. This provides only the interface, but requires no user-defined functions.
@enden
*/

/**@en
@fn  vce::MonitorListener::MonitorListener()
@enden
*/

/**@en
@fn virtual vce::MonitorListener::~MonitorListener()
@enden
*/

/**@en
@fn void vce::MonitorListener::RegisterConsole(MonitorConsole *con)=0
@brief Register MonitorConsole.
@details Although registration is obligatory, users need not call this function because the MonitorConsole constructor automatically does.
@enden
*/

/**@en
@fn void vce::MonitorListener::DeregisterConsole(MonitorConsole *con)=0
@brief Deregister MonitorConsole.
@details Users need not call this function because the MonitorCustom destructor automatically does.
@enden
*/

/**@en
@fn void vce::MonitorListener::RegisterCustom(MonitorCustomPage *cus)=0
@brief Register MonitorCustom.
@details Although registration is obligatory, users need not call this function because the MonitorCustom constructor automatically does.
@enden
*/

/**@en
@fn void vce::MonitorListener::DeregisterCustom(MonitorCustomPage *cus)=0
@brief Deregister MonitorCustom.
@details Users need not call this function because the MonitorCustom destructor automatically does.
@enden
*/

/**@en
@fn MonitorConsole * vce::MonitorListener::GetConsoleFromID(vce::VUint32 consoleID)=0
@brief Retrieve MonitorConsole by its ID.
@enden
*/

/**@en
@fn MonitorCustomPage * vce::MonitorListener::GetCustomPageFromID(vce::VUint32 customID)=0
@brief Retrieve MonitorCustomPage by its ID.
@enden
*/

/**@en
@fn vce::MonitorConsole * vce::MonitorListener::GetConsoleFromName(const std::string &name)=0
@brief Retrieve MonitorConsole by its name.
@enden
*/

/**@en
@fn vce::MonitorCustomPage * vce::MonitorListener::GetCustomPageFromName(const std::string &name)=0
@brief Retrieve MonitorCustomPage by its name.
@enden
*/

/**@en
@fn MonitorConsoleListItr vce::MonitorListener::GetConsoleBegin()=0
@brief iterator.begin
@enden
*/

/**@en
@fn MonitorConsoleListItr vce::MonitorListener::GetConsoleEnd()=0
@brief iterator.end
@enden
*/

/**@en
@fn MonitorCustomPageListItr vce::MonitorListener::GetCustomBegin()=0
@brief iterator.begin
@enden
*/

/**@en
@fn MonitorCustomPageListItr vce::MonitorListener::GetCustomEnd()=0
@brief iterator.end
@enden
*/

/**@en
@fn void vce::MonitorListener::ReadTemplate()=0
@brief Read in templates.
@details Read in the page templates from the following files in the current directory: index.html, status.html, console.html, custom.html, and dummy.html.
@enden
*/

/**@en
@class vce::MonitorConsole
@brief Monitor console.
@details This creates a console page for monitoring purposes. This console allows viewing from any web browser. The constructor registers this console; the destructor deregisters it. To deregister it at any time before that, call MonitorConsole::Detach().
@enden
*/

/**@en
@var vce::VUint32 vce::MonitorConsole::consoleID
@brief Console ID
@enden
*/

/**@en
@var vce::MonitorListener* vce::MonitorConsole::ml
@enden
*/

/**@en
@var std::string vce::MonitorConsole::cname
@brief Console name.
@enden
*/

/**@en
@var HTTP_CHARSET vce::MonitorConsole::code
@brief Encoding for sending page.
@details This takes precedence over HTTPServerSession::code. 
@enden
*/

/**@en
@var std::stringstream vce::MonitorConsole::clog
@brief Stream for log output.
@enden
*/

/**@en
@fn  vce::MonitorConsole::MonitorConsole(vce::MonitorListener *parent, const std::string &name="")
@brief Constructor.
@enden
*/

/**@en
@fn MonitorConsole & vce::MonitorConsole::operator=(MonitorConsole &)
@enden
*/

/**@en
@fn  vce::MonitorConsole::MonitorConsole(MonitorConsole &)
@enden
*/

/**@en
@fn virtual vce::MonitorConsole::~MonitorConsole()
@brief Destructor.
@enden
*/

/**@en
@fn vce::VUint32 vce::MonitorConsole::GetID() const
@brief Get ID.
@enden
*/

/**@en
@fn void vce::MonitorConsole::CommandRequest(const std::string &cmd)
@brief This is called when VCE accepts a command.
@details cmd is the string entered with the web browser.
@enden
*/

/**@en
@fn void vce::MonitorConsole::Detach()
@brief Detach from MonitorListener.
@enden
*/

/**@en
@fn std::string vce::MonitorConsole::GetName()
@brief Get name.
@enden
*/

/**@en
@class vce::MonitorDummyProtocol
@brief Monitor dummy protocol.
@details Multiple inheritance and gen settings allow sending of dummy protocol. Using this class involves inheriting a Session class generated with gen. For further details, please refer to the tutorial.
@enden
*/

/**@en
@var vce::VUint32 vce::MonitorDummyProtocol::dummyID
@brief Dummy protocol ID.
@enden
*/

/**@en
@var std::string vce::MonitorDummyProtocol::dpname
@brief Name displayed in tab, etc.
@enden
*/

/**@en
@var std::stringstream vce::MonitorDummyProtocol::html
@brief HTML for dummy protocol page.
@details gen generates this.
@enden
*/

/**@en
@var std::stringstream vce::MonitorDummyProtocol::js
@brief JavaScript for dummy protocol page.
@details gen generates this.
@enden
*/

/**@en
@var ProtocolDataList vce::MonitorDummyProtocol::recordproto
@brief Protocol list received.
@details Incoming protocols are added to this list.
@enden
*/

/**@en
@typedef std::vector<ProtocolData> vce::MonitorDummyProtocol::ProtocolDataList
@enden
*/

/**@en
@typedef std::vector<ProtocolData>::iterator vce::MonitorDummyProtocol::ProtocolDataListItr
@enden
*/

/**@en
@fn  vce::MonitorDummyProtocol::MonitorDummyProtocol(const std::string &name="")
@brief Constructor.
@enden
*/

/**@en
@fn MonitorDummyProtocol & vce::MonitorDummyProtocol::operator=(MonitorDummyProtocol &)
@enden
*/

/**@en
@fn  vce::MonitorDummyProtocol::MonitorDummyProtocol(MonitorDummyProtocol &)
@enden
*/

/**@en
@fn virtual vce::MonitorDummyProtocol::~MonitorDummyProtocol()
@brief Destructor.
@enden
*/

/**@en
@fn vce::VUint32 vce::MonitorDummyProtocol::GetID() const
@brief Get ID.
@enden
*/

/**@en
@fn void vce::MonitorDummyProtocol::SetName(const std::string &name)
@brief Specify name for dummy protocol.
@enden
*/

/**@en
@fn std::string vce::MonitorDummyProtocol::GetName() const
@brief Get name for dummy protocol.
@enden
*/

/**@en
@fn void vce::MonitorDummyProtocol::Post(const char *post)=0
@brief Arrival of an HTTP POST request triggers a call to this function.
@details The user-defined function required needs only call PostRequest() for the Session generated with gen. For further details, please refer to the tutorial.
@enden
*/

/**@en
@fn void vce::MonitorDummyProtocol::GetRecordProtocolXML(std::stringstream &out)
@brief Get list of protocols received in XML format.
@details This list gives the protocol name and time received.
@param out XML data generated.
@enden
*/

/**@en
@fn void vce::MonitorDummyProtocol::ReplayRecords(VUint32 index)
@brief Simulate protocol received.
@details The user-defined function required needs only call Replay() for the Session generated with gen. For further details, please refer to the tutorial.
@enden
*/

/**@en
@struct vce::MonitorDummyProtocol::ProtocolData
@brief Structure to hold data for protocol received.
@enden
*/

/**@en
@var vce::VUint32 vce::MonitorDummyProtocol::ProtocolData::time
@enden
*/

/**@en
@var std::string vce::MonitorDummyProtocol::ProtocolData::name
@enden
*/

/**@en
@var std::vector<vce::VUint8> vce::MonitorDummyProtocol::ProtocolData::data
@enden
*/

/**@en
@class vce::MonitorCustomPage
@brief Monitor custom page.
@details Applications can add their own user-defined pages. This console allows viewing from any web browser. The constructor registers this console; the destructor deregisters it. To deregister it at any time before that, call MonitorConsole::Detach().
@enden
*/

/**@en
@var vce::VUint32 vce::MonitorCustomPage::customID
@enden
*/

/**@en
@var vce::MonitorListener* vce::MonitorCustomPage::ml
@enden
*/

/**@en
@var std::string vce::MonitorCustomPage::cpname
@brief Name displayed in tab, etc.
@enden
*/

/**@en
@var std::stringstream vce::MonitorCustomPage::html
@brief HTML for custom page.
@enden
*/

/**@en
@var vce::VUint32 vce::MonitorCustomPage::interval
@brief Automatic update interval, in milliseconds, for custom page.
@enden
*/

/**@en
@var std::string vce::MonitorCustomPage::element
@brief Element name for custom page tag to update.
@enden
*/

/**@en
@var bool vce::MonitorCustomPage::nocache
@brief true disables browser page caching.
@enden
*/

/**@en
@var HTTP_CHARSET vce::MonitorCustomPage::code
@brief Encoding for sending page.
@details This takes precedence over HTTPServerSession::code. 
@enden
*/

/**@en
@var HTTP_CONTENTTYPE vce::MonitorCustomPage::type
@brief Content-Type for sending page.
@details This takes precedence over HTTPServerSession::type.
@enden
*/

/**@en
@fn  vce::MonitorCustomPage::MonitorCustomPage(vce::MonitorListener *owner, const std::string &name="")
@brief Constructor.
@enden
*/

/**@en
@fn MonitorCustomPage & vce::MonitorCustomPage::operator=(MonitorCustomPage &)
@enden
*/

/**@en
@fn  vce::MonitorCustomPage::MonitorCustomPage(MonitorCustomPage &)
@enden
*/

/**@en
@fn virtual vce::MonitorCustomPage::~MonitorCustomPage()
@brief Destructor.
@enden
*/

/**@en
@fn vce::VUint32 vce::MonitorCustomPage::GetID() const
@brief Get ID.
@enden
*/

/**@en
@fn std::string vce::MonitorCustomPage::GetName() const
@brief Get name for custom page.
@enden
*/

/**@en
@fn void vce::MonitorCustomPage::Detach()
@brief Detach from MonitorListener.
@enden
*/

/**@en
@fn void vce::MonitorCustomPage::Post(const char *post)=0
@brief Custom POST processing for page.
@details This function is called when a web browser calls the provided JavaScript (SendCustomProtocolRequest).
@param post Data body from request arriving from web browser.
@enden
*/

/**@en
@fn void vce::MonitorCustomPage::PreContentSend(std::string method, vce::URIParam param, std::string body)=0
@brief Called immediately before sending the page.
@details More precisely, VCE calls this callback immediately before returning the tab contents. To tailor the contents of this page in accordance with the request, modify the HTML inside this function. To use parameters for other purposes, rewrite the JavaScript. The defaults are GET for the HTTP method and the ID for param.
@param method HTTP method from request arriving from web browser.
@param param URI query.
@param body Data body from request arriving from web browser.
@enden
*/

/**@en
@fn std::string vce::MonitorCustomPage::PreContentPartSend(std::string body)=0
@brief Called if the custom page contains Ajax functionality.
@details At the interval specified in the member interval, a custom page obtains the server's return value for this function and displays an HTML element inside the member having the specified id. Setting interval to 0, however, disables these updates.
@param body Data body from request arriving from web browser.
@return String value returned.
@enden
*/

/**@en
@class vce::Finder
@brief Class for multicast broadcasts to local network.
@enden
*/

/**@en
@var unsigned int vce::Finder::group
@brief Group ID (multicast IP address)
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::Finder::Keywords
@enden
*/

/**@en
@fn  vce::Finder::Finder(unsigned int group)
@brief Constructor.
@details Searches out other applications on the local network.
@param group Group ID for search. This must be between 0xE0000001 and 0xEFFFFFFF. This is the multicast IP address group.
@enden
*/

/**@en
@fn virtual vce::Finder::~Finder()
@enden
*/

/**@en
@fn unsigned int vce::Finder::GetGroupID()
@brief Get group ID.
@enden
*/

/**@en
@fn void vce::Finder::Keyword(Keywords &key)=0
@details This is for specifying a keyword map. This map can contain multiple key-value pairs, but the upper bound on string storage is approximately 1000 bytes.
@param key Reference to keyword map. Specify the keywords with this variable.
@enden
*/

/**@en
@fn void vce::Finder::Find(Keywords &key)=0
@brief Callback for when Finder search succeeds.
@details The hits include keywords that the Finder has set itself. The same keywords repeat as the Finder search repeats at a fixed interval.
@param key Reference to keywords found.
@enden
*/

/**@en
@fn void vce::Finder::Closed()
@brief Callback for after VCE closes the object.
@enden
*/

/**@en
@fn bool vce::Finder::Close()
@brief Close Finder.
@details Closing the Finder disables further searches and hide this Finder from others.
@enden
*/

/**@en
@class vce::TCPListenerFinder
@details Class providing a template for searching for TCP listeners. Specify the target Session with the session parameter and the connection port with the constructor. Searches out TCPListenerFinder instances on the local network and connects to one.
@enden
*/

/**@en
@var VUint16 vce::TCPListenerFinder< Session >::port
@brief Port number to use.
@enden
*/

/**@en
@fn void vce::TCPListenerFinder::Keyword(Keywords &key)
@brief Specify Listener keywords.
@param key Keyword map
@enden
*/

/**@en
@fn void vce::TCPListenerFinder::Find(Keywords &key)
@brief Override for Find().
@details This version connects after referring to Listener keywords.
@param key Keyword map
@enden
*/

/**@en
@fn  vce::TCPListenerFinder::TCPListenerFinder(VUint16 port, unsigned int group=0xeff0f1f2)
@brief Constructor.
@details Specify the port and group ID.
@param port Port number to use for connection.
@see Finder
@enden
*/

/**@en
@class vce::UPnPStatus
@brief Interface for configuring UPnP port.
@enden
*/

/**@en
@enum vce::UPnPStatus::Status
@brief Port mapping status.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_Unknown
@brief Unknown.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_SearchRouter
@brief Searching for router.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_GetRouterSpec
@brief Getting router specifications.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_ActionWait
@brief Processing action.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_Complete
@brief Processing complete. Note that this does not necessarily mean success.
@enden
*/

/**@en
@var vce::UPnPStatus::Status vce::UPnPStatus::UPnP_Fail
@brief Failure.
@enden
*/

/**@en
@fn Status vce::UPnPStatus::GetUPnPStatus()=0
@brief Get UPnP manipulation status.
@enden
*/

/**@en
@fn bool vce::UPnPStatus::WaitComplete(VUint32 timeout=5000)=0
@brief Wait for UPnP manipulation to complete.
@details Internally, this repeatedly calls VCEPoll() until processing is complete.
@param timeout Maximum time, in milliseconds, to wait for processing to complete.
@return true if UPnP manipulation is complete; false otherwise.
@enden
*/

/**@en
@fn virtual vce::UPnPStatus::~UPnPStatus()
@enden
*/

/**@en
@class vce::UPnPQoSStatus
@brief Class for QoSRequest.
@enden
*/

/**@en
@fn bool vce::UPnPQoSStatus::GetTrafficHandle(int &handle)=0
@brief Get handle for QoS manipulation.
@details Get the handle number necessary for calling Update(), Release(), and other functions. Attempting this for any state other than UPnP_Complete triggers failure.
@param handle Reference to variable for storing handle.
@return true for success.
@enden
*/

/**@en
@struct vce::AutoVariableArrayBuf
@enden
*/

/**@en
@var T* vce::AutoVariableArrayBuf< T >::var
@enden
*/

/**@en
@var const int vce::AutoVariableArrayBuf< T >::n
@enden
*/

/**@en
@struct vce::AutoArrayBuf
@enden
*/

/**@en
@var T* vce::AutoArrayBuf< T, N >::var
@enden
*/

/**@en
@struct vce::AutoBuf
@enden
*/

/**@en
@var T& vce::AutoBuf< T >::var
@enden
*/

/**@en
@struct vce::MemoryAllocator
@enden
*/

/**@en
@struct vce::Altr
@enden
*/

/**@en
@var MemoryAllocator* vce::Altr< T >::mm
@enden
*/

/**@en
@struct vce::vector
@enden
*/

/**@en
@struct vce::Allocatable
@enden
*/

/**@en
@var T* vce::Allocatable< T >::t
@enden
*/

/**@en
@var MemoryAllocator*& vce::Allocatable< T >::cma
@enden
*/

/**@en
@struct vce::ArrayPointer
@enden
*/

/**@en
@var std::size_t vce::ArrayPointer< T >::sz
@enden
*/

/**@en
@struct vce::Pointer
@enden
*/

/**@en
@typedef std::vector<MonitorListener*> vce::MonitorListenerList
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::ConfigMap
@enden
*/

/**@en
@typedef std::vector<std::string> vce::StringList
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::PostMap
@enden
*/

/**@en
@typedef std::map<unsigned char, MPINode*> vce::MPIMap
@enden
*/

/**@en
@typedef std::map<unsigned char, MPINode*>::iterator vce::MPIMapItr
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::HTTPHeader
@brief Definition of HTTP header.
@enden
*/

/**@en
@typedef std::map<std::string, std::string> vce::URIParam
@brief Definition of URI parameters.
@enden
*/

/**@en
@typedef std::vector<vce::MonitorConsole*>::iterator vce::MonitorConsoleListItr
@enden
*/

/**@en
@typedef std::vector<vce::MonitorCustomPage*>::iterator vce::MonitorCustomPageListItr
@enden
*/

/**@en
@typedef unsigned int vce::VUint32
@enden
*/

/**@en
@typedef int vce::VSint32
@enden
*/

/**@en
@typedef unsigned short vce::VUint16
@enden
*/

/**@en
@typedef short vce::VSint16
@enden
*/

/**@en
@typedef unsigned char vce::VUint8
@enden
*/

/**@en
@typedef char vce::VSint8
@enden
*/

/**@en
@typedef unsigned short vce::wchar
@enden
*/

/**@en
@typedef std::basic_string<vce::wchar> vce::vwstring
@enden
*/

/**@en
@typedef char vce::utf8
@enden
*/

/**@en
@enum vce::VCE_EXCEPTION
@brief Non-critical exceptions.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_UNKNOWN
@brief Unknown.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_CREATESOCKETFAIL
@brief Socket creation failure.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_BINDFAIL
@brief Port binding failure.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_LISTENINGFAIL
@brief Listen failure.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_NONBLOCKING
@brief Failure switching to non-blocking mode.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_CONNECTFAIL
@brief Connection failure.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_CONNECTTIMEOUT
@brief Connection timed out.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_NAMERESOLUTIONFAIL
@brief Name resolution failure.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_KEYEXCHANGEFAIL
@brief Failure exchanging encryption keys.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_RECVBUFFER_FULL
@brief Receive buffer overflow.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_SENDBUFFER_FULL
@brief Send buffer overflow.
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_SIGPIPE
@brief SIGPIPE
@enden
*/

/**@en
@var vce::VCE_EXCEPTION vce::VCE_EXCEPT_INVALIDDATA
@brief Invalid data.
@enden
*/

/**@en
@enum vce::VCE_STATE
@brief Session or Listener status.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_UNKNOWN
@brief Unknown.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_CONNECTING
@brief Connecting.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_PREPROCESS
@brief Between session creation and connection or between connection established and session enabled.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_ESTABLISHED
@brief Session is valid.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_LISTENING
@brief While listener is listening.
@enden
*/

/**@en
@var vce::VCE_STATE vce::VCE_STATE_CLOSED
@brief Object has been closed. The CLOSED state causes VCE to immediately delete the object, so only lasts a short time, until the callback to Closed() is complete, forcing a shift to UNKNOWN.
@enden
*/

/**@en
@enum vce::VCE_CLOSEREASON
@brief Reason for closing.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_UNKNOWN
@brief Unknown.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_LOCAL
@brief Disconnected at this end.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_REMOTE
@brief Disconnected at other end.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_RECVBUFFERFULL
@brief Receive buffer full.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_RECVERROR
@brief Receive error.
@enden
*/

/**@en
@var vce::VCE_CLOSEREASON vce::VCE_CLOSE_INVALIDDATA
@brief Invalid data received.
@enden
*/

/**@en
@enum vce::VCE_LOGLEVEL
@brief Log level flags.
@enden
*/

/**@en
@var vce::VCE_LOGLEVEL vce::VCE_LOGLEVEL_LOW
@brief Lower for important logs.
@enden
*/

/**@en
@var vce::VCE_LOGLEVEL vce::VCE_LOGLEVEL_MEDIUM
@brief Slightly higher for VCE internal operation logs.
@enden
*/

/**@en
@var vce::VCE_LOGLEVEL vce::VCE_LOGLEVEL_HIGH
@brief Higher for operating system API calls.
@enden
*/

/**@en
@enum vce::HTTP_CHARSET
@brief HTTP server response letter codes.
@enden
*/

/**@en
@var vce::HTTP_CHARSET vce::HTTP_CHARSET_UTF_8
@enden
*/

/**@en
@var vce::HTTP_CHARSET vce::HTTP_CHARSET_SJIS
@enden
*/

/**@en
@var vce::HTTP_CHARSET vce::HTTP_CHARSET_NONE
@enden
*/

/**@en
@var vce::HTTP_CHARSET vce::HTTP_CHARSET_NUM
@enden
*/

/**@en
@enum vce::HTTP_CONTENTTYPE
@brief HTTP server response Content-Type values.
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_HTML
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_TEXT
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_PLAIN
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_XML
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_CSS
@enden
*/

/**@en
@var vce::HTTP_CONTENTTYPE vce::HTTP_CONTENTTYPE_NUM
@enden
*/

/**@en
@fn int vce::XXtoi(const std::string &_XX)
@brief Convert two-digit hexadecimal string into a numeric value.
@details Converts 10 to 16, aa to 170, FF to 255, etc. Negative values and single-digit hexadecimal values are not supported.
@param XX Hexadecimal string
@return Decimal values. A return value of -1 indicates failure.
@enden
*/

/**@en
@fn size_t vce::GetSelfAddress(unsigned int *iparray, size_t array_qt)
@brief Get own address(es). (could be plural)
@details Under Windows, this address is only available after VCECreate() has been called.
@param iparray Array to hold addresses.
@param array_qt Maximum number of elements in array.
@return Number of addresses returned.
@enden
*/

/**@en
@fn std::string vce::AddrToString(VUint32 ip)
@brief Convert VUint32 address into string.
@details Convert IPv4 address into string.
@param ip Address
@return String.
@see StringToAddr
@enden
*/

/**@en
@fn VUint32 vce::StringToAddr(const std::string &_ip)
@brief Convert IP address into string.
@param ip IP address in dotted decimal notation—the popular 192.168.1.2, for example.
@return 0xffffffff for invalid input.
@see AddrToString
@enden
*/

/**@en
@fn VUint32 vce::bswap(VUint32 bin)
@brief Swap byte order.
@details Convert bin byte order between little- and big-endian formats.
@param bin 32-bit value to convert.
@return Resulting 32-bit value.
@enden
*/

/**@en
@fn VUint16 vce::xchg(VUint16 bin)
@brief Swap byte order.
@details Convert bin byte order between little- and big-endian formats.
@param bin 16-bit value to convert.
@return Resulting 16-bit value.
@enden
*/

/**@en
@fn void * vce::memmem(const void *p, size_t sz_p, const void *cmp, size_t sz_cmp)
@brief Search memory.
@details Find the first occurrence of the string cmp, with length sz_cmp size, in the memory range p, with length sz_p size.
@param p Pointer to head of memory range to search.
@param sz_p Size of memory range to search.
@param cmp Pointer to head of string to find.
@param sz_cmp Size of string to find.
@return Pointer to first match found; otherwise, NULL.
@enden
*/

/**@en
@fn int vce::atoi(const std::string &_str)
@brief Convert string to integer.
@details Low-cost alternative to standard atoi() for converting a string to an integer. This covers the entire int range: -2147483648 to 2147483647. It skips any initial non-digit characters.
@param str String to convert.
@return Integer result or 0 for failure.
@enden
*/

/**@en
@fn std::string vce::ntoa(from n)
@brief Convert to string.
@enden
*/

/**@en
@fn VUint32 vce::GetTime()
@brief current time.
@details Gets clock timer value in milliseconds. This 32-bit counter has a cycle of 49 days.
@return Time.
@enden
*/

/**@en
@fn CalendarTime vce::GetCalendarTime()
@brief Get time in calendar format.
@details Gets time in calendar format. The CalendarTime structure returned has members from year down to milliseconds.
@return Time.
@enden
*/

/**@en
@fn void vce::ThreadSleep(VUint32 millsec)
@brief Put thread to sleep.
@details Pauses thread execution for roughly the specified number of milliseconds.
@param millsec Time to sleep.
@enden
*/

/**@en
@fn size_t vce::strlen_s(const char *str, size_t buflen)
@brief Length of string in bytes.
@details This version of strlen() adds a buffer length specification. Normal strlen() is subject to buffer overruns when the string does not end with a null byte, so this version specifies a limit.
@enden
*/

/**@en
@fn size_t vce::strlen(const char *str)
@enden
*/

/**@en
@fn void vce::strncpy(char *dest, size_t destlen, const char *src, size_t srclen)
@brief Copy string.
@details This version of strcpy() adds a buffer length specification for both source and destination.
@enden
*/

/**@en
@fn void vce::strcpy_s(char *dest, size_t destlen, const char *src)
@brief Copy string.
@details This version of strcpy() adds a buffer length specification for the destination.
@enden
*/

/**@en
@fn size_t vce::utfstrnlen(const utf8 *utf8string, size_t buflen)
@brief String length in UTF-8 characters.
@details This function gives the string length in characters, not bytes.
@param utf8string UTF-8 string.
@param buflen Buffer size.
@return Number of characters.
@enden
*/

/**@en
@fn size_t vce::wcslen(const wchar *str)
@brief String length in UTF-16 characters.
@details String length in UTF-8 characters. It does not support surrogate pairs.
@enden
*/

/**@en
@fn size_t vce::utf16to8(utf8 *dest, size_t destlen, const wchar *src)
@brief Convert from UTF-16 to UTF-8.
@details Converts UTF-16 string to UTF-8.
@param dest char array for storing output.
@param destlen Size of output buffer in bytes.
@param src UTF-16 string.
@enden
*/

/**@en
@fn std::string vce::utf16to8(const vwstring &wstr)
@brief Convert from UTF-16 to UTF-8.
@details Converts UTF-16 string to UTF-8.
@param wstr UTF-16 string.
@return UTF-8 string.
@enden
*/

/**@en
@fn size_t vce::utf8to16(wchar *dest, size_t destlen, const utf8 *src)
@brief Convert from UTF-8 to UTF-16.
@details Converts UTF-8 string to UTF-16.
@param dest wchar array for storing output.
@param destlen Size of output buffer in bytes.
@param src UTF-8 string.
@enden
*/

/**@en
@fn vwstring vce::utf8to16(const std::string &str)
@brief Convert from UTF-8 to UTF-16.
@details Converts UTF-8 string to UTF-16.
@param str UTF-8 string.
@return UTF-16 string.
@enden
*/

/**@en
@fn ConfigMap vce::LoadSimpleConfig(const std::string &filename)
@brief Read in configuration file.
@details Map strings in name=value format to conf["name"]==value format. This conversion ignores any # and the following comment, whitespace preceding the #, and whitespace on either side of the equals sign.
@param filename Configuration file.
@enden
*/

/**@en
@fn ConfigMap vce::LoadSimpleConfig(std::istream &input)
@details internal
@enden
*/

/**@en
@fn std::string vce::NowTimeString()
@brief Get time in string format.
@details The string has the following format: YYYY/MM/DD hh:mm:ss.ms. This is used to log output.
@return Time string.
@enden
*/

/**@en
@fn StringList vce::SplitString(const std::string &string, char delim= ' ', char escape= '"')
@brief Split string.
@details Partitions string on the specified delimiter. Delimiters between pairs of the escape character, however, are considered normal characters. The escape characters do not appear in the output. With its default settings, the function splits the string 'aaa bbb "c c c"' into three parts: 'aaa', 'bbb', and 'c c c'.
@param string Input string.
@param delim Character to use as delimiter. The default is space (' ').
@param escape Escape character. The default is double quote ('"').
@return List of substrings.
@enden
*/

/**@en
@fn PostMap vce::SplitPost(const std::string &_post)
@brief Partition URL query string.
@details Parses a query from HTTP POST or other request into key=value pairs. For example, 'aaa=bb&ccc=a20bc' contains two such pairs. URI decoding applies to XX sequences.
@enden
*/

/**@en
@fn std::string vce::encodeURI(const std::string &text)
@brief Encode URI string.
@details URI encoding applies to everything except a-z, A-Z, and 0-9.
@return Resulting string.
@enden
*/

/**@en
@fn std::string vce::decodeURI(const std::string &text)
@brief Decode URI.
@details Decodes encoded URI.
@return Resulting string. Empty string for failure.
@enden
*/

/**@en
@fn std::string vce::Base64Encode(const void *data, int len)
@brief Encode using Base64.
@details Encodes binary data using Base64. The return value is a null-terminated string approximately one-third larger than the source data.
@param data Pointer to binary data.
@param len Length of binary data in bytes.
@return Resulting string.
@enden
*/

/**@en
@fn int vce::Base64Decode(const char *src, unsigned char *dest)
@brief Decode Base64.
@details Decodes string encoded with Base64.
@param src String encoded with Base64.
@param dest Pointer to area for storing results. The minimum storage required is (len/4+1)*3 bytes, where len is the length of the source string in bytes.
@return Length of result or -1 for failure.
@enden
*/

/**@en
@fn std::string vce::Base64DecodeString(const std::string &src)
@enden
*/

/**@en
@fn int vce::Base64Decode(const char *src, std::vector< unsigned char > &dest)
@enden
*/

/**@en
@fn std::string vce::MemoryToStr(const void *data, size_t len)
@brief Convert binary data to hexadecimal string.
@details The output consists of two hexadecimal digits per source byte--upper case for the letter portions.
@param data Pointer to head of data to convert.
@param len Number of bytes to convert.
@return Resulting string.
@enden
*/

/**@en
@fn bool vce::VCEInitialize()
@brief Initialize VCE library.
@details Initializes shared VCE internal resources. Call this before creating any VCE objects with VCECreate(). Call VCEFinalize() when VCE is no longer used.
@see vce::VCEFinalize
@enden
*/

/**@en
@fn void vce::VCEFinalize()
@brief Shut down VCE library.
@details Releases shared VCE internal resources initialized by vce::VCEInitialize(). This is normally called near the end of the program. 
@see vce::VCEInitialize
@enden
*/

/**@en
@fn VCE * vce::VCECreate(std::ostream *target=NULL, int loglevel=VCE_LOGLEVEL_LOW, MemoryAllocator *ma=&defaultallocator)
@brief Create VCE object.
@details The first step is creating a VCE object with this function so that the API can be used. Multiple objects can be created. This function also specifies the log level.
@see vce::SetLogger
@enden
*/

/**@en
@fn void vce::VCEDelete(VCE *n)
@brief Delete VCE object.
@details An object created with VCECreate() must be deleted with this function. This releases memory and performs other cleanup operations.
@enden
*/

/**@en
@fn MonitorListener * vce::MonitorListenerCreate()
@brief Create MonitorListener object.
@details Always use this function to create instances of this class.
@enden
*/

/**@en
@fn void vce::MonitorListenerDelete(MonitorListener *ml)
@brief Delete MonitorListener object.
@details Always use this function to delete instances of the MonitorListener class.
@enden
*/

/**@en
@fn UPnPStatus * vce::UPnPSetPortmapping(VUint16 port, VCE *api)
@details UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param port Port number for which to enable IP masquerading.
@param api Pointer to VCE object.
@return Interface for accessing UPnP status.
@enden
*/

/**@en
@fn UPnPStatus * vce::UPnPDeletePortmapping(VUint16 port, VCE *api)
@details UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param port Port number for which to disable IP masquerading.
@param api Pointer to VCE object.
@return Interface for accessing UPnP status.
@enden
*/

/**@en
@fn void vce::DeleteUPnP(UPnPStatus *uppp)
@brief Delete UPnP manipulation object.
@details The object must be deleted when canceling or terminating router manipulations. UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@enden
*/

/**@en
@fn UPnPStatus * vce::UPnPQoSSetUpstreamBandwidth(VUint32 bandwidth, VCE *api)
@details Configure the router's upstream bandwidth.
UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param bandwidth Upstream bandwidth in kbps: 1024 for 1 Mbps.
@param api Pointer to VCE object.
@return Interface for accessing UPnP status.
@enden
*/

/**@en
@fn UPnPQoSStatus * vce::UPnPQoSRequestTraffic(int guaranteeBandwidth, VUint32 address, VUint32 prefixLength, VUint16 portStart, VUint16 portEnd, bool isSource, VUint32 leaseTime, VCE *api)
@details Specifies the bandwidth for the exclusive use of the application. Since router resources are limited, the application should release this bandwidth with UPnPQoSReleaseTraffic() when it terminates or specify leaseTime.
@param guaranteeBandwidth Percentage of bandwidth to allocate. Specifying a total exceeding 100% triggers failure. Note also that the requested bandwidth comes with no guarantee.
UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param address Target address. Setting this to 0 specifies a local address.
@param portStart First port in range.
@param portEnd Last port in range. Making this the same as portStart specifies a single port. Specifying a port lower than portStart triggers failure.
@param prefixLength Number of bits in address mask. The normal setting is 32.
@param isSource true uses the specified address, port, and other settings for the source; false, for the destination.
@param leaseTime Time limit, in milliseconds, for this configuration. Setting this to 0 requests an open-ended configuration.
@param api Pointer to VCE object.
@return Interface for accessing UPnP status.
@see UPnPQoSReleaseTraffic
@enden
*/

/**@en
@fn UPnPStatus * vce::UPnPQoSUpdateTraffic(int handle, int guaranteeBandwidth, VUint32 address, VUint32 prefixLength, VUint16 portStart, VUint16 portEnd, bool isSource, VUint32 leaseTime, VCE *api)
@details This is for modifying UPnPQoSRequestTraffic() settings.
UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param handle Use GetTrafficHandle() to retrieve this from the object returned by UPnPQoSRequestTraffic().
@see UPnPQoSRequestTraffic
@enden
*/

/**@en
@fn UPnPStatus * vce::UPnPQoSReleaseTraffic(int handle, VCE *api)
@details Release upstream bandwidth.
UPnP manipulations are not thread safe, so multiple threads must not simultaneously use the UPnP API.
@param handle Use GetTrafficHandle() to retrieve this from the object returned by UPnPQoSRequestTraffic().
@see UPnPQoSRequestTraffic
@enden
*/

/**@en
@fn bool vce::operator==(const Altr< T1 > &, const Altr< T2 > &)
@enden
*/

/**@en
@fn bool vce::operator!=(const Altr< T1 > &, const Altr< T2 > &)
@enden
*/

/**@en
@fn Altr< T > vce::Altring(MemoryAllocator *&ma)
@enden
*/

/**@en
@var const char* vce::version_number
@brief Version number.
@enden
*/

/**@en
@var const char* vce::build_number
@brief Build number.
@enden
*/

/**@en
@var DefaultAllocator vce::defaultallocator
@enden
*/

/**@en
@var MemoryAllocator* vce::glovalallocator
@enden
*/

/**@en
@var const size_t vce::VCE_BUFFER_ERROR
@enden
*/

/**@en
@namespace vce_gen_serialize
@enden
*/

/**@en
@struct vce_gen_serialize::pack
@enden
*/

/**@en
@var vce::VUint8* vce_gen_serialize::pack::first
@enden
*/

/**@en
@var vce::VUint8* vce_gen_serialize::pack::last
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VSint64 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VUint64 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VSint32 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VUint32 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VSint16 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VUint16 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VSint8 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const vce::VUint8 &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const float &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const double &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VSint64 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VUint64 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VSint32 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VUint32 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VSint16 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VUint16 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VSint8 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(vce::VUint8 &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(float &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(double &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const char *src, pack &buf, vce::VUint32 src_buf_len)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(char *dest, pack &buf, vce::VUint32 dest_buf_len)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const std::basic_string< Any > &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(std::basic_string< Any > &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const std::pair< Any, Bny > &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(std::pair< Any, Bny > &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const std::map< Any, Bny > &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(std::map< Any, Bny > &dest, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Push(const std::vector< Any > &src, pack &buf)
@enden
*/

/**@en
@fn bool vce_gen_serialize::Pull(std::vector< Any > &dest, pack &buf)
@enden
*/


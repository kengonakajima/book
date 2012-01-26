// 通信ミドルウェアのヘッダ
#include "vce2.h"

#include "util.h"


#include <string>
#include "boost/functional/hash.hpp"


// VCE のセッションのアルゴリズムの種類を選択するコールバック関数
void * MySelectAlgorithmCallback(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata)
{
	// TCP セッションを使用する
	return vce::SelectSession::SelectTcpStreamSession(api, s, l);
}


// ハッシュ文字列を作りだす。
// FIXME: 今はboostのハッシュ関数を手軽なので使っているが、SHA-1などより強いハッシュ関数を使いたい。あえて、それとわかる文字列を生成するようにした。
std::string makeHashString( std::string s)
{
    std::size_t h = boost::hash<std::string>()(s);
    std::ostringstream ss;
    ss << "boosthash_fixme_" << h;
    return ss.str();
}



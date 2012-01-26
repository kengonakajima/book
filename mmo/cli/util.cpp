// server util

// 通信ミドルウェアのヘッダ
#include "vce2.h"
#include <math.h>
#include "util.h"


#include <string>
#include "boost/functional/hash.hpp"

// VCE のセッションのアルゴリズムの種類を選択するコールバック関数
void * MySelectAlgorithmCallback(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata)
{
	// TCP セッションを使用する
	return vce::SelectSession::SelectTcpStreamSession(api, s, l);
}



//　文字列をエスケープして安全にする。 json libからぱくってきた
std::string escapeString( std::string s ){
    const char *p = s.c_str();
    std::string result;
    result.reserve( s.size()); // 大量のmallocを避ける
    for( const char* c=p; *c != 0; ++c ){
      switch(*c){
      case '\"':
          result += "\\\"";
          break;
      case '\\':
          result += "\\\\";
          break;
      case '\b':
          result += "\\b";
          break;
      case '\f':
          result += "\\f";
          break;
      case '\n':
          result += "\\n";
          break;
      case '\r':
          result += "\\r";
          break;
      case '\t':
          result += "\\t";
          break;
      case '/':
          // Even though \/ is considered a legal escape in JSON, a bare
          // slash is also legal, so I see no reason to escape it.
          // (I hope I am not misunderstanding something.)
      default:
          result += *c;
      }
    }
    return result;
}

// ハッシュ文字列を作りだす。
// FIXME: 今はboostのハッシュ関数を手軽なので使っているが、SHA-1などより強いハッシュ関数を使いたい。あえて、それとわかる文字列を生成するようにした。
std::string makeHashString( std::string s)
{
    std::size_t h = hashval(s);
    std::ostringstream ss;
    ss << "boosthash_fixme_" << h;
    return ss.str();
}

std::size_t hashval( std::string s)
{
    return boost::hash<std::string>()(s);
}

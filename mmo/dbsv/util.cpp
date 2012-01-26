// 通信ミドルウェアのヘッダ
#include "vce2.h"

#include "util.h"

// VCE のセッションのアルゴリズムの種類を選択するコールバック関数
void * MySelectAlgorithmCallback(vce::VCE * api, vce::Session * s, vce::Listener * l, void * userdata)
{
	// TCP セッションを使用する
	return vce::SelectSession::SelectTcpStreamSession(api, s, l);
}

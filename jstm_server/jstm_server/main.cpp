#include "header.h"
#include "iocp_server.h"


int main() {

	cout << sizeof(Monster) << endl;;
	cout << sizeof(DirectX::XMFLOAT4X4) << endl;;
	cout << sizeof(unsigned short) << endl;;
	cout << sizeof(sc_packet_monster_pos) << endl;;

	Iocp_server *g_iocp_server = new Iocp_server;

}

// 클라 네트워크매니저 추가할것
//	 - 플레이어 스탯(hp, gold), 웨이브, 포탈라이프 업데이트패킷 수신 - 거의완료
//	 - 클라네트워크매니저에서 게임시작패킷보낼때 스테이지번호 전달하기and 서버에서 처리 - 1/2
//
// 
// 게임중간에 난입한 플레이어한테 모든 함정 정보 보내기
// 웨이브 처리
// 
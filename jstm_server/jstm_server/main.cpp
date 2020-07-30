#include "header.h"
#include "iocp_server.h"


int main() {

	cout << sizeof(DirectX::XMFLOAT4X4) << endl;;
	cout << sizeof(unsigned short) << endl;;
	cout << sizeof(sc_packet_monster_pos) << endl;;

	Iocp_server *g_iocp_server = new Iocp_server;

}

// 클라 네트워크매니저 추가할것
//	 - 플레이어 스탯(hp, gold) 업데이트패킷 수신
//	 - 웨이브, 포탈라이프 등 게임정보 업데이트 패킷 수신
//	 - 
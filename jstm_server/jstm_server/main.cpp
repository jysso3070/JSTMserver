#include "header.h"
#include "iocp_server.h"


int main() {

	cout << sizeof(DirectX::XMFLOAT4X4) << endl;;
	cout << sizeof(unsigned short) << endl;;
	cout << sizeof(sc_packet_monster_pos) << endl;;

	Iocp_server *g_iocp_server = new Iocp_server;

}

// Ŭ�� ��Ʈ��ũ�Ŵ��� �߰��Ұ�
//	 - �÷��̾� ����(hp, gold) ������Ʈ��Ŷ ����
//	 - ���̺�, ��Ż������ �� �������� ������Ʈ ��Ŷ ����
//	 - 
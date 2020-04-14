#include "packet_manager.h"


packet_manager::packet_manager()
{
}


packet_manager::~packet_manager()
{
}

void packet_manager::send_packet(int client_id, SOCKET client_socket, void * buf)
{
	char* packet = reinterpret_cast<char*>(buf);
	int packet_size = packet[0];
	OVER_EX *send_over = new OVER_EX;
	memset(send_over, 0x00, sizeof(OVER_EX));
	send_over->event_type = EV_SEND;
	memcpy(send_over->net_buf, packet, packet_size);
	send_over->wsabuf[0].buf = send_over->net_buf;
	send_over->wsabuf[0].len = packet_size;

	int ret = WSASend(client_socket, send_over->wsabuf, 1, 0, 0, &send_over->over, 0);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("WSASend Error :", err_no);
	}
}

void packet_manager::send_id_packet(int client_id, SOCKET client_socket)
{
	sc_packet_send_id packet;
	packet.id = client_id;
	packet.size = sizeof(packet);
	packet.type = SC_SEND_ID;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::t_send_pos_packet(int client_id, SOCKET client_socket, short p_x, short p_y)
{
	sc_packet_pos packet;
	packet.id = client_id;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = p_x;
	packet.y = p_y;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_put_player(int client_id, SOCKET client_socket, int new_player_id)
{
	sc_packet_put_player packet;
	packet.new_player_id = new_player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	//packet.world_pos = player_pos;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_pos_packet(int client_id, SOCKET client_socket, DirectX::XMFLOAT4X4 player_pos)
{
	sc_packet_pos packet;
	packet.id = client_id;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.world_pos = player_pos;
	packet.x = 0;
	packet.y = 0;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_room_list_pakcet(int client_id, SOCKET client_socket, short room_num, int host_id, int guest_id)
{
	sc_pakcet_room_list packet;
	packet.room_num = room_num;
	packet.host_id = host_id;
	packet.guest_id = guest_id;
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::error_display(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::cout << L"¿¡·¯: " << lpMsgBuf << std::endl;

	while (true);
	LocalFree(lpMsgBuf);
}

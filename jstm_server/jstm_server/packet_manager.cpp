#include "packet_manager.h"


packet_manager::packet_manager()
{
}


packet_manager::~packet_manager()
{
}

void packet_manager::send_packet(int client_id, SOCKET client_socket, void * buf)
{
	unsigned short* packet = reinterpret_cast<unsigned short*>(buf);
	unsigned short packet_size = packet[0];
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
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = p_x;
	packet.y = p_y;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_put_player_packet(int client_id, SOCKET client_socket, int new_player_id)
{
	sc_packet_put_player packet;
	packet.new_player_id = new_player_id;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	//packet.world_pos = player_pos;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_pos_packet(int client_id, SOCKET client_socket, int mover_id, DirectX::XMFLOAT4X4 player_pos, short animation_state)
{
	sc_packet_pos packet;
	packet.mover_id = mover_id;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.world_pos = player_pos;
	packet.animation_state = animation_state;
	packet.x = 0;
	packet.y = 0;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_remove_player_packet(int client_id, SOCKET client_socket, int leave_player_id)
{
	sc_packet_remove_player packet;
	packet.leave_player_id = leave_player_id;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;
	//packet.world_pos = player_pos;
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_room_info_pakcet(int client_id, SOCKET client_socket, GAME_ROOM* game_room)
{
	sc_packet_room_info packet;
	packet.type = SC_SEND_ROOM_LIST;
	packet.room_number = game_room->room_number;
	packet.room_state = game_room->room_state;
	packet.stage_number = game_room->stage_number;
	for (short i = 0; i < 4; ++i) {
		packet.players_id[i] = game_room->players_id[i];
	}
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_make_room_ok(int client_id, SOCKET client_socket, short room_number)
{
	sc_packet_make_room_ok packet;
	packet.type = SC_MAKE_ROOM_OK;
	packet.id = client_id;
	packet.room_number = room_number;
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_join_room_ok(int client_id, SOCKET client_socket, short room_number, GAME_ROOM* game_room)
{
	sc_packet_join_room_ok packet;
	packet.type = SC_JOIN_ROOM_OK;
	packet.id = client_id;
	packet.room_number = room_number;
	for (short i = 0; i < 4; ++i) {
		packet.players_id[i] = game_room->players_id[i];
	}
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_trap_info_packet(int client_id, SOCKET client_socket, short trap_id, DirectX::XMFLOAT3 trap_pos, char trap_type)
{
	sc_packet_trap_info packet;
	packet.type = SC_TRAP_INFO;
	packet.trap_type = trap_type;
	packet.trap_id = trap_id;
	packet.trap_pos = trap_pos;
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_monster_pos(int client_id, SOCKET client_socket, MONSTER mon_arr[])
{
	sc_packet_monster_pos packet;
	packet.type = SC_MONSTER_POS;
	memcpy_s(packet.monsterArr, sizeof(packet.monsterArr), mon_arr, sizeof(packet.monsterArr));
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
	//packet.monsterArr = mon_arr;
}

void packet_manager::send_game_end(int client_id, SOCKET client_socket, bool clearFlag)
{
	sc_packet_game_end packet;
	packet.type = SC_GAME_END;
	packet.clear = clearFlag;
	packet.size = sizeof(packet);
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_stat_change(int client_id, SOCKET client_socket, short hp, short gold)
{
	sc_packet_stat_change packet;
	packet.type = SC_PLAYER_STAT_CHANGE;
	packet.id = client_id;
	packet.hp = hp;
	packet.gold = gold;
	packet.size = sizeof(packet);
	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_game_info_update(int client_id, SOCKET client_socket, short wave, short portalLife)
{
	sc_packet_game_info_update packet;
	packet.type = SC_GAME_INFO_UPDATE;
	packet.id = client_id;
	packet.wave = wave;
	packet.portalLife = portalLife;
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_game_start(int client_id, SOCKET client_socket, short stage_number, short wave, short portalLife)
{
	sc_packet_game_start packet;
	packet.type = SC_GAME_START;
	packet.stage_number = stage_number;
	packet.wave = wave;
	packet.portalLife = portalLife;
	packet.size = sizeof(packet);

	send_packet(client_id, client_socket, &packet);
}

void packet_manager::send_nameLogin_result(int client_id, SOCKET client_socket, char result)
{
	sc_packet_nameLogin_result packet;
	packet.type = SC_NAMELOGIN_RESULT;
	packet.id = client_id;
	packet.result = result;
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

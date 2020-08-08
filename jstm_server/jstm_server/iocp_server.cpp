#include "iocp_server.h"

Iocp_server::Iocp_server()
{
	Database_manager *db_manager = new Database_manager;
	m_Database_manager = db_manager;

	//m_Timer = new Timer;
	//m_Timer->Reset();

	cout << "monstersize: " << sizeof(Monster) << endl;
	serverInitialize();
	init_wPos();

	make_thread();

}


Iocp_server::~Iocp_server()
{
	//delete m_Timer;
	//m_Timer = nullptr;

	WSACleanup();
}

void Iocp_server::serverInitialize()
{
	//WSADATA WSAData;
	//WSAStartup(MAKEWORD(2, 2), &WSAData);

	m_iocp_Handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	m_new_user_id = 0;
	m_new_room_num = 1;
	m_new_trap_id = 1;

	m_Server_manager->get_server_ipAddress();
	m_Server_manager->get_cpu_count();
	init_DB();

	init_socket();

}

void Iocp_server::make_thread()
{
	thread accept_thread{ &Iocp_server::run_acceptThread, this};
	thread mainThread_1{ &Iocp_server::run_mainThread, this};
	thread mainThread_2{ &Iocp_server::run_mainThread, this};
	thread mainThread_3{ &Iocp_server::run_mainThread, this};
	thread mainThread_4{ &Iocp_server::run_mainThread, this };
	thread mainThread_5{ &Iocp_server::run_mainThread, this };
	thread mainThread_6{ &Iocp_server::run_mainThread, this };
	thread mainThread_7{ &Iocp_server::run_mainThread, this };
	thread eventQueueThread{ &Iocp_server::run_eventQueueThread, this};

	//thread packet_count_thread{ &Iocp_server::run_packet_countThread, this };
	//thread collision_thread{}

	accept_thread.join();
	mainThread_1.join();
	mainThread_2.join();
	mainThread_3.join();
	mainThread_4.join();
	mainThread_5.join();
	mainThread_6.join();
	mainThread_7.join();
	eventQueueThread.join();

	//packet_count_thread.join();

}

void Iocp_server::init_DB()
{
	m_Database_manager->sql_load_database();
	//m_database_manager->sql_update_data(1, 3);
}

void Iocp_server::init_socket()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// ���� ���� bind
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		cout << "bind fail \n";
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	m_accept_socket = listenSocket;
}

void Iocp_server::init_wPos()
{
	ZeroMemory(&default_wPos, sizeof(default_wPos));
	default_wPos._11 = 1; default_wPos._12 = 0; default_wPos._13 = 0; default_wPos._14 = 0;
	default_wPos._21 = 0; default_wPos._22 = 1; default_wPos._23 = 0; default_wPos._24 = 0;
	default_wPos._31 = 0; default_wPos._32 = 0; default_wPos._33 = 1; default_wPos._34 = 0;
	default_wPos._41 = -3000.f; default_wPos._42 = -50.f; default_wPos._43 = 0.f; default_wPos._44 = 0;
}

void Iocp_server::run_acceptThread()
{

	// ���� ��� ����
	listen(m_accept_socket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(m_accept_socket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {	// ���Ͽ�����н�
			printf("err - accept fail \n");
			break;
		}
		else {
			printf("socket accept success \n");
		}

		int user_id = m_new_user_id++;
		PLAYER_INFO *new_player = new PLAYER_INFO;
		ZeroMemory(new_player, sizeof(new_player));
		new_player->id = user_id;
		new_player->socket = clientSocket;
		new_player->room_number = -1;
		new_player->animation_state = 0;
		new_player->hp = 200;
		new_player->gold = 500;
		new_player->damageCooltime = false;
		new_player->recv_over.wsabuf[0].len = MAX_BUFFER;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;
		new_player->recv_over.event_type = EV_RECV;
		new_player->player_state = PLAYER_STATE_in_lobby;
		new_player->player_world_pos = default_wPos;
		new_player->is_connect = true;

		m_map_player_info.insert(make_pair(user_id, new_player)); // �÷��̾� map�� �μ�Ʈ

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_iocp_Handle, user_id, 0); // iocp ���

		m_Packet_manager->send_id_packet(user_id, clientSocket);

		cout << "���ο� �÷��̾� ����" << endl;
		//m_packet_manager->send_pos_packet(user_id, clientSocket, )

		//for (auto c : m_map_player_info) {
		//	if (c.second->id == user_id) { 
		//		// �ڱ� �ڽŵ� ǲ �÷��̾�?
		//	}
		//	else {
		//		if (c.second->is_connect == true) { // ���ӵǾ� �ִ� �÷��̾��� üũ
		//			// ���ο� �÷��̾������� ������ �÷��̾�鿡�� ����
		//			m_Packet_manager->send_put_player_packet(c.second->id, c.second->socket, user_id);
		//			//���ο� �÷��̾�� ������ �÷��̾� ������ ����
		//			m_Packet_manager->send_put_player_packet(user_id, clientSocket, c.second->id);
		//		}
		//		
		//	}
		//}

		send_all_room_list(user_id); // ��� �� ���� ����

		///////
		m_map_player_info[user_id]->x = 300;
		m_map_player_info[user_id]->y = 300;

		EVENT ev{ user_id, chrono::high_resolution_clock::now() + 10s, EV_TEST, 0 };
		add_event_to_queue(ev);

		/*EVENT tev{ -10, chrono::high_resolution_clock::now() + 5s, EV_MONSTER_THREAD_RUN, 0 };
		add_event_to_eventTimer(tev);*/

		//////


		memset(&m_map_player_info[user_id]->recv_over.over, 0, sizeof(m_map_player_info[user_id]->recv_over.over));
		flags = 0;
		int ret = WSARecv(clientSocket, m_map_player_info[user_id]->recv_over.wsabuf, 1, NULL,
			&flags, &(m_map_player_info[user_id]->recv_over.over), NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				m_Server_manager->socket_error_display("WSARecv Error : ", err_no);
		}
	}

}

void Iocp_server::run_mainThread()
{
	while (true) {
		DWORD num_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		GetQueuedCompletionStatus(m_iocp_Handle, &num_byte, (PULONG_PTR)p_key, &p_over, INFINITE);

		// Ŭ���̾�Ʈ�� �������� ���
		if (0 == num_byte) {
			SOCKET client_s = m_map_player_info[key]->socket;
			closesocket(client_s);
			m_map_player_info[key]->is_connect = false;
			m_map_player_info[key]->player_state = PLAYER_STATE_default;
			process_disconnect_client(key);
			continue;
		}

		OVER_EX *over_ex = reinterpret_cast<OVER_EX *> (p_over);

		if (EV_RECV == over_ex->event_type) {
			SOCKET client_s = m_map_player_info[key]->socket;
			over_ex->net_buf[num_byte] = 0;
			//process_packet(key, over_ex->net_buf);

			unsigned int cur_packet_size = 0;
			unsigned int saved_packet_size = 0;
			DWORD buf_byte = num_byte;

			char * temp = reinterpret_cast<char*>(over_ex->net_buf);
			char tempBuf[MAX_BUFFER];

			while (buf_byte != 0)
			{
				if (cur_packet_size == 0) {
					cur_packet_size = temp[0]; // ù ��Ŷ������ ����
				}
				if (buf_byte + saved_packet_size >= cur_packet_size) {
					// ù ��Ŷ ������� ���ι��� ���� + ������ִ� ��Ŷ������� ũ�� ��Ŷ��ó���ϰ�
					// ó���� ��Ŷũ�⸸ŭ ����������, ���� ��Ŷ������� ����
					memcpy(tempBuf + saved_packet_size, temp, cur_packet_size - saved_packet_size);
					process_packet(key, tempBuf);
					temp += cur_packet_size - saved_packet_size;
					buf_byte -= cur_packet_size - saved_packet_size;
					cur_packet_size = 0;
					saved_packet_size = 0;
				}
				else {
					memcpy(tempBuf + saved_packet_size, temp, buf_byte);
					saved_packet_size += buf_byte;
					buf_byte = 0;
				}
			}

			DWORD flags = 0;
			memset(&over_ex->over, 0x00, sizeof(WSAOVERLAPPED));
			WSARecv(client_s, over_ex->wsabuf, 1, 0, &flags, &over_ex->over, 0);
		}
		if (EV_TEST == over_ex->event_type) {
			cout << "test event ! \n";
			char tname[11] = "jys";
			m_Database_manager->check_nameLogin(tname);
			//std::string new_name = "qqq";
			//m_database_manager->sql_insert_new_data(m_database_manager->m_list_player_db.size(), new_name);
			/*get_player_db();
			for (auto d : m_list_player_db) {
				cout << "name: " << d.name <<"."<< endl;
			}*/
			delete over_ex;
		}
		else if (EV_GEN_1stWAVE_MONSTER == over_ex->event_type) {
			short stage_number = *(short *)(over_ex->net_buf);
			process_gen_monster(key, stage_number);
			delete over_ex;
		}
		else if (EV_MONSTER_DEAD == over_ex->event_type) {
			short monster_id = *(short *)(over_ex->net_buf);
			short room_number = (short)key;
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_isLive(false);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			//cout << monster_id << "�� false \n";
			delete over_ex;
		}
		else if (EV_MONSTER_NEEDLE_TRAP_COLLISION == over_ex->event_type) {
			short monster_id = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			delete over_ex;
		}
		else if (EV_MONSTER_FIRE_TRAP_COLLISION == over_ex->event_type) {
			short monster_id = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			delete over_ex;
		}
		else if (EV_MONSTER_ARROW_TRAP_COLLISION == over_ex->event_type) {
			short monster_id = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			delete over_ex;
		}
		else if (EV_MONSTER_SLOW_TRAP_COLLISION == over_ex->event_type) {
			short monster_id = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			m_map_monsterPool[room_number][monster_id].set_buffType(TRAP_BUFF_NONE);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			delete over_ex;
		}
		else if (EV_WALLTRAP_COLLTIME == over_ex->event_type) {
			short trap_index = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_trap[room_number][trap_index].set_wallTrapOn(false);
			delete over_ex;
		}
		else if (EV_CHECK_WAVE_END == over_ex->event_type) {
			check_wave_end(key);
			delete over_ex;
		}
		else if (EV_GEN_MONSTER == over_ex->event_type) {
			short stage_number = m_map_game_room[key]->stage_number;
			process_gen_monster(key, stage_number);
			delete over_ex;
		}
		else if (EV_MONSTER_THREAD_RUN == over_ex->event_type) {
			process_monster_move(key);
			delete over_ex;
		}
		else if (EV_MONSTER_ATTACK == over_ex->event_type) {
			short room_number = (short)key;
			short monster_id = *(short *)(over_ex->net_buf);
			check_monster_attack(room_number, monster_id);
			delete over_ex;
		}
		else if (EV_PLAYER_DAMAGE_COOLTIME == over_ex->event_type) {
			m_map_player_info[key]->damageCooltime = false;
			delete over_ex;
		}
		else if (EV_PROTALLIFE_UPDATE == over_ex->event_type) {
			send_protalLife_update(key);
			delete over_ex;
		}
		else if (PLAYER_GAME_START == over_ex->event_type) {
			m_map_player_info[key]->player_state = PLAYER_STATE_playing_game;
			delete over_ex;
		}
	}
}

void Iocp_server::run_eventQueueThread()
{
	while (true) {
		m_eventTimer_lock.lock();
		while (true == m_eventTimer_queue.empty()) {	// �̺�Ʈ ť�� ��������� ��õ��� ����ٰ� �ٽ� �˻�
			m_eventTimer_lock.unlock();
			this_thread::sleep_for(10ms);
			m_eventTimer_lock.lock();
		}
		const EVENT &ev = m_eventTimer_queue.top();
		if (ev.wakeup_time > chrono::high_resolution_clock::now()) {
			m_eventTimer_lock.unlock();
			this_thread::sleep_for(10ms);
			continue;
		}

		EVENT p_ev = ev;
		m_eventTimer_queue.pop();
		m_eventTimer_lock.unlock();

		// �̺�Ʈ ���� �з��ؼ� iocp�� �̺�Ʈ�� �����ش�
		if (EV_MOVE == p_ev.event_type) { 
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MOVE;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_TEST == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_TEST;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_THREAD_RUN == p_ev.event_type) {
			//m_monsterThread_run = true;
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_THREAD_RUN;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_GEN_1stWAVE_MONSTER == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_GEN_1stWAVE_MONSTER;
			*(short *)(over_ex->net_buf) = p_ev.target_obj; // stage number;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
			//gen_monster(p_ev.obj_id, 1, 1, 1);
		}
		else if (EV_MONSTER_DEAD == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_DEAD;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_NEEDLE_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_NEEDLE_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_SLOW_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_SLOW_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_FIRE_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_FIRE_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_ARROW_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_ARROW_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_WALLTRAP_COLLTIME == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_WALLTRAP_COLLTIME;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_CHECK_WAVE_END == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_CHECK_WAVE_END;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_GEN_MONSTER == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_GEN_MONSTER;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_ATTACK == p_ev.event_type) { // target_id = monster idx
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_ATTACK;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_PLAYER_DAMAGE_COOLTIME == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_PLAYER_DAMAGE_COOLTIME;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_PROTALLIFE_UPDATE == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_PROTALLIFE_UPDATE;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (PLAYER_GAME_START == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = PLAYER_GAME_START;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
	}
}

void Iocp_server::process_monster_move(const short room_number)
{
	//auto start = chrono::high_resolution_clock::now();
	auto mon_pool = m_map_monsterPool[room_number];
	MONSTER monsterPacketArr[MAX_MONSTER];
	ZeroMemory(monsterPacketArr, sizeof(monsterPacketArr));
	for (short i = 0; i < MAX_MONSTER; ++i) {
		m_map_game_room[room_number]->monsterThread_lock.lock();
		monsterPacketArr[i].id = i;
		monsterPacketArr[i].isLive = false;
		monsterPacketArr[i].type = mon_pool[i].get_monster_type();
		monsterPacketArr[i].hp = 0;
		monsterPacketArr[i].animation_state = 0;
		monsterPacketArr[i].world_pos = default_wPos;
		m_map_game_room[room_number]->monsterThread_lock.unlock();
		if (mon_pool[i].get_isLive() == false) { // �������ʿ���� �� ����
			//monsterPacketArr[i].isLive = mon_pool[i].get_isLive();
			continue;
		}

		// ��Ż�� ������ ����
		if (mon_pool[i].get_arrivePortal() == true) {
			mon_pool[i].set_isLive(false);
			/*m_map_game_room[room_number]->monsterThread_lock.lock();
			monsterPacketArr[i].isLive = false;
			monsterPacketArr[i].animation_state = mon_pool[i].get_animation_state();
			monsterPacketArr[i].type = mon_pool[i].get_monster_type();
			monsterPacketArr[i].hp = mon_pool[i].get_HP();
			monsterPacketArr[i].world_pos = mon_pool[i].get_4x4position();
			m_map_game_room[room_number]->monsterThread_lock.unlock();*/
#ifndef TESTMODE
			m_map_game_room[room_number]->portalLife -= 1;
#endif
			// ��Ż������ ������Ʈ�ϴ� ��Ŷ ����
			//EVENT ev_portalLifeUpdate{ room_number, chrono::high_resolution_clock::now() + 32ms, EV_PROTALLIFE_UPDATE, 0 };
			//add_event_to_queue(ev_portalLifeUpdate);
			//continue;
		}

		// ���� ü�� 0 ���� ��� �̺�Ʈ �߰�
		if (mon_pool[i].get_HP() <= 0) {
			mon_pool[i].set_animation_state(M_ANIM_DEATH);
			add_monster_dead_event(room_number, i);
			m_map_game_room[room_number]->monsterThread_lock.lock();
			monsterPacketArr[i].isLive = mon_pool[i].get_isLive();
			monsterPacketArr[i].animation_state = mon_pool[i].get_animation_state();
			monsterPacketArr[i].type = mon_pool[i].get_monster_type();
			monsterPacketArr[i].hp = mon_pool[i].get_HP();
			monsterPacketArr[i].world_pos = mon_pool[i].get_4x4position();
			m_map_game_room[room_number]->monsterThread_lock.unlock();
			continue;
		}
		
		if (mon_pool[i].get_isLive() == true)
		{
			// Ÿ���÷��̾ ������ �����ȿ� �ִ� �÷��̾� ��ġ
			if (mon_pool[i].get_target_id() == -1) {
				int near_id = -1;
				float near_dis = 300.f;
				for (int player_index = 0; player_index < 4; ++player_index) {
					int player_id = m_map_game_room[room_number]->players_id[player_index];
					if (player_id == -1) { continue; }
					if (m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
						float dis = Vector3::Distance(m_map_player_info[player_id]->get_pos(), mon_pool[i].get_position());
						if (dis <= 200.f) { // ��׷� ���� ��
							if (near_dis > dis) {
								near_id = player_id;
								near_dis = dis;
							}
						}
					}
				}
				mon_pool[i].set_target_id(near_id);
			}
			// Ÿ���� ������ ���� �ൿ
			if (mon_pool[i].get_target_id() != -1) {
				int target_id = mon_pool[i].get_target_id();
				if (m_map_player_info[target_id]->player_state != PLAYER_STATE_playing_game) {
					mon_pool[i].set_target_id(-1);
					continue;
				}
				float dis = Vector3::Distance(m_map_player_info[target_id]->get_pos(), mon_pool[i].get_position());
				if (dis <= 200.f && dis >= ORC_ATT_RANGE) { //��׷� ����
					mon_pool[i].set_target_id(target_id);
					mon_pool[i].set_aggro_direction(m_map_player_info[target_id]->get_pos());
					mon_pool[i].move_forward(MONSTER_MOVE_DISTANCE, mon_pool);
					mon_pool[i].set_animation_state(M_ANIM_RUN);
				}
				else if (dis < ORC_ATT_RANGE) { // ���ݹ���
					mon_pool[i].set_target_id(target_id);
					mon_pool[i].set_aggro_direction(m_map_player_info[target_id]->get_pos());
					mon_pool[i].set_animation_state(M_ANIM_ATT);
					if (mon_pool[i].get_attackCooltime() == false &&
						m_map_player_info[mon_pool[i].get_target_id()]->damageCooltime == false) {
						EVENT ev_monAttck{ room_number, chrono::high_resolution_clock::now() + 1s, EV_MONSTER_ATTACK, i };
						add_event_to_queue(ev_monAttck);
						mon_pool[i].set_attackCooltime(true);
					}
				}
				else {
					mon_pool[i].set_target_id(-1);
				}
			}
			// Ÿ���� ������ �ൿ
			else {
				//mon_pool[i].process_move_path();
				mon_pool[i].process_move_path_t();
				mon_pool[i].move_forward(MONSTER_MOVE_DISTANCE, mon_pool);
			}

			// trap collision
			auto coopyTrapPool = m_map_trap[room_number];
			for (short trap_idx = 0; trap_idx < MAX_TRAP; ++trap_idx) {
				if (mon_pool[i].get_isTrapCooltime() == true) { break; }
				if (coopyTrapPool[trap_idx].get_enable() == false) { continue; }

				if (coopyTrapPool[trap_idx].get_type() == TRAP_NEEDLE) {
					float trap_dis = Vector3::Distance(coopyTrapPool[trap_idx].get_position(), mon_pool[i].get_position());
					if (trap_dis < TRAP_NEEDLE_RANGE) {
						cout << "needle ���� �ǰ�" << endl;
						//mon_pool[i].get_monsterLock().lock();
						mon_pool[i].set_trap_cooltime(true);
						mon_pool[i].decrease_hp(TRAP_NEEDLE_ATT);
						//mon_pool[i].get_monsterLock().unlock();
						EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_NEEDLE_TRAP_COLLISION, room_number };
						add_event_to_queue(trap_ev);
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_SLOW) {
					float trap_dis = Vector3::Distance(coopyTrapPool[trap_idx].get_position(), mon_pool[i].get_position());
					if (trap_dis < TRAP_SLOW_RANGE) {
						cout << "slow ���� �ǰ�" << endl;
						// �����ǰ���Ÿ������, 3���Ŀ� ��Ÿ�� �����ϴ� �̺�Ʈ �߰�
						//mon_pool[i].get_monsterLock().lock();
						mon_pool[i].set_trap_cooltime(true);
						mon_pool[i].set_buffType(TRAP_BUFF_SLOW);
						//mon_pool[i].get_monsterLock().unlock();
						EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_SLOW_TRAP_COLLISION, room_number };
						add_event_to_queue(trap_ev);
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_FIRE) {
					volatile bool trapColli = false;
					if(coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MX) {
						if (mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x &&
							mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 200 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 30) {
							mon_pool[i].set_trap_cooltime(true);
							trapColli = true;
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_FIRE_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							//cout << "MX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PX) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 200 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 30) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_FIRE_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MZ) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 30 &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 30 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 200) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_FIRE_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MZ�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PZ) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 30 &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 200) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_FIRE_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PZ�� ������ �ǰ� \n";
						}
					}
					if (trapColli == true) {
						cout << "�� ������ �ǰ� \n";
						if (coopyTrapPool[trap_idx].get_wallTrapOn() == false) {
							coopyTrapPool[trap_idx].set_wallTrapOn(true);
							EVENT wallTrapCool{ trap_idx, chrono::high_resolution_clock::now() + 2s, EV_WALLTRAP_COLLTIME, room_number };
							add_event_to_queue(wallTrapCool);
							for (int i = 0; i < 2; ++i) {
								int player_id = m_map_game_room[room_number]->players_id[i];
								if (player_id == -1) { continue; }
								if (m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
									m_Packet_manager->send_wallTrapOn(player_id, m_map_player_info[player_id]->socket, trap_idx);
								}
							}
							cout << "�� ������ ��Ŷ���� \n";
						}
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_ARROW) {
					volatile bool trapColli = false;
					if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MX) {
						if (mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x &&
							mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 200 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 30) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_ARROW_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PX) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 200 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 30) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_ARROW_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MZ) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 30 &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 30 &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - 200) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_ARROW_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MZ�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PZ) {
						if (mon_pool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - 30 &&
							mon_pool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + 30 &&
							mon_pool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z &&
							mon_pool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + 200) {
							mon_pool[i].set_trap_cooltime(true);
							EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_ARROW_TRAP_COLLISION, room_number };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PZ�� ������ �ǰ� \n";
						}
					}
					if (trapColli == true) {
						cout << "�� ȭ������ �ǰ� \n";
						if (coopyTrapPool[trap_idx].get_wallTrapOn() == false) {
							coopyTrapPool[trap_idx].set_wallTrapOn(true);
							EVENT wallTrapCool{ trap_idx, chrono::high_resolution_clock::now() + 2s, EV_WALLTRAP_COLLTIME, room_number };
							add_event_to_queue(wallTrapCool);
							for (int i = 0; i < 2; ++i) {
								int player_id = m_map_game_room[room_number]->players_id[i];
								if (player_id == -1) { continue; }
								if (m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
									m_Packet_manager->send_wallTrapOn(player_id, m_map_player_info[player_id]->socket, trap_idx);
								}
							}
							cout << "�� ȭ������ ��Ŷ���� \n";
						}
					}
				}

				//if (mon_pool[i].get_isTrapCooltime() == true) { break; }
				//if (m_map_trap[room_number][trap_idx].get_enable() == false) { continue; }

				//if (m_map_trap[room_number][trap_idx].get_type() == TRAP_NEEDLE) {
				//	float trap_dis = Vector3::Distance(m_map_trap[room_number][trap_idx].get_position(), mon_pool[i].get_position());
				//	if (trap_dis < TRAP_NEEDLE_RANGE) {
				//		cout << "needle ���� �ǰ�" << endl;
				//		mon_pool[i].set_trap_cooltime(true);
				//		mon_pool[i].decrease_hp(TRAP_NEEDLE_ATT);
				//		EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_NEEDLE_TRAP_COLLISION, room_number };
				//		add_event_to_queue(trap_ev);
				//	}
				//}
				//else if (m_map_trap[room_number][trap_idx].get_type() == TRAP_SLOW) {
				//	float trap_dis = Vector3::Distance(m_map_trap[room_number][trap_idx].get_position(), mon_pool[i].get_position());
				//	if (trap_dis < TRAP_SLOW_RANGE) {
				//		cout << "slow ���� �ǰ�" << endl;
				//		// �����ǰ���Ÿ������, 3���Ŀ� ��Ÿ�� �����ϴ� �̺�Ʈ �߰�
				//		mon_pool[i].set_trap_cooltime(true);
				//		mon_pool[i].set_buffType(TRAP_BUFF_SLOW);
				//		EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_SLOW_TRAP_COLLISION, room_number };
				//		add_event_to_queue(trap_ev);
				//	}
				//}
				//else if (m_map_trap[room_number][trap_idx].get_type() == TRAP_FIRE) {
				//	float trap_dis = Vector3::Distance(m_map_trap[room_number][trap_idx].get_position(), mon_pool[i].get_position());
				//	if (trap_dis < TRAP_FIRE_RANGE) {
				//		cout << "fire ���� �ǰ�" << endl;
				//		/*mon_pool[i].set_trap_cooltime(true);
				//		mon_pool[i].decrease_hp(TRAP_NEEDLE_ATT);
				//		EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_NEEDLE_TRAP_COLLISION, room_number };
				//		add_event_to_queue(trap_ev);*/
				//	}
				//}
				//else if (m_map_trap[room_number][trap_idx].get_type() == TRAP_ARROW) {
				//	float trap_dis = Vector3::Distance(m_map_trap[room_number][trap_idx].get_position(), mon_pool[i].get_position());
				//	if (trap_dis < TRAP_ARROW_RANGE) {
				//		cout << "arrow ���� �ǰ�" << endl;
				//		/*mon_pool[i].set_trap_cooltime(true);
				//		mon_pool[i].decrease_hp(TRAP_NEEDLE_ATT);
				//		EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_NEEDLE_TRAP_COLLISION, room_number };
				//		add_event_to_queue(trap_ev);*/
				//	}
				//}
			}
		}

		if (mon_pool[i].get_arrivePortal() == true) {
			mon_pool[i].set_isLive(false);
		}


		// ��Ŷ�� �� ���͹迭 �� ����
		m_map_game_room[room_number]->monsterThread_lock.lock();
		if (mon_pool[i].get_isLive() == true) {
			monsterPacketArr[i].isLive = true;
		}
		else {
			monsterPacketArr[i].isLive = false;
		}
		monsterPacketArr[i].animation_state = mon_pool[i].get_animation_state();
		monsterPacketArr[i].type = mon_pool[i].get_monster_type();
		monsterPacketArr[i].hp = mon_pool[i].get_HP();
		monsterPacketArr[i].world_pos = mon_pool[i].get_4x4position();
		m_map_game_room[room_number]->monsterThread_lock.unlock();
	}

	for (int i = 0; i < 2; ++i) {
		int player_id = m_map_game_room[room_number]->players_id[i];
		if (player_id == -1) { continue; }
		if (m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
			m_Packet_manager->send_monster_pos(player_id, m_map_player_info[player_id]->socket, monsterPacketArr);
		}
	}

	if (m_map_game_room[room_number]->wave_on == true) {
		EVENT ev{ room_number, chrono::high_resolution_clock::now() + 50ms, EV_MONSTER_THREAD_RUN, 0 };
		add_event_to_queue(ev);
	}
	//cout << "mon run \n";
	//auto end = chrono::high_resolution_clock::now();
	//cout << "time: " << (end - start).count() << "ns" << endl;
}

void Iocp_server::run_packet_countThread()
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	while (true) {
		std::chrono::duration<float> sec = std::chrono::system_clock::now() - start;
		if (sec.count() > 1) {
			cout << "1�ʰ� �̵���Ŷ����Ƚ��" << pakcetCount << endl;
			start = std::chrono::system_clock::now();
			pakcetCount = 0;
		}
	}
}

void Iocp_server::add_event_to_queue(EVENT & ev)
{
	m_eventTimer_lock.lock();
	m_eventTimer_queue.push(ev);
	m_eventTimer_lock.unlock();
}

void Iocp_server::t_process_player_move(const int& id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = m_map_player_info[id]->x;
	short y = m_map_player_info[id]->y;
	switch (packet[1]) {
	case CS_UP:
		y -= 10;
		break;
	case CS_DOWN:
		y += 10;
		break;
	case CS_LEFT:
		x -= 10;
		break;
	case CS_RIGHT:
		x += 10;
		break;
	default:
		break;
	}

	m_map_player_info[id]->x = x;
	m_map_player_info[id]->y = y;

	m_Packet_manager->t_send_pos_packet(id, m_map_player_info[id]->socket, x, y);
	cout << "x: " << m_map_player_info[id]->x << ", y: " << m_map_player_info[id]->y << endl;

}

void Iocp_server::process_player_move(const int& id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);
	cs_packet_pos *pos_packet = reinterpret_cast<cs_packet_pos*>(buff);

	switch (packet[1]) {
	case CS_UP:
		break;
	case CS_DOWN:
		break;
	case CS_LEFT:
		break;
	case CS_RIGHT:
		break;
	default:
		break;
	}

	if (id == 0) {
		++pakcetCount;
	}

	m_map_player_info[id]->player_world_pos = pos_packet->player_world_pos;
	m_map_player_info[id]->animation_state = pos_packet->animation_state;
	//m_map_player_info[id]->player_state = PLAYER_STATE_playing_game;

	if (m_map_game_room[m_map_player_info[id]->room_number]->players_id == NULL) {
		return;
	}

	short room_number = m_map_player_info[id]->room_number;
	for (short i = 0; i < 2; ++i) {
		int other_id = m_map_game_room[room_number]->players_id[i];
		if (other_id == -1) { continue; }
		if (m_map_player_info[other_id]->is_connect == true && 
			m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game && other_id != id) {
			//m_Packet_manager->send_put_player_packet(other_id, m_map_player_info[other_id]->socket, id);
			m_Packet_manager->send_pos_packet(other_id, m_map_player_info[other_id]->socket, id,
				m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
		}
	}

}

void Iocp_server::process_make_room(const int& id)
{
	short room_num = m_new_room_num++;
	GAME_ROOM *new_room = new GAME_ROOM;
	ZeroMemory(new_room, sizeof(new_room));
	new_room->room_number = room_num;
	new_room->enable = true;
	new_room->room_state = R_STATE_in_room;
	new_room->wave_count = 0;
	new_room->stage_number = 1;
	new_room->portalLife = 20;
	new_room->wave_on = false;
	new_room->players_id[0] = id;
	new_room->players_id[1] = -1;
	new_room->players_id[2] = -1;
	new_room->players_id[3] = -1;

	m_map_player_info[id]->roomList_lock.lock();
	m_map_game_room.insert(make_pair(room_num, new_room)); // gameroom map�� ����
	m_map_player_info[id]->roomList_lock.unlock();

	m_map_player_info[id]->room_number = room_num;
	m_map_player_info[id]->player_state = PLAYER_STATE_in_room;

	m_Packet_manager->send_make_room_ok(id, m_map_player_info[id]->socket, room_num);

	for (auto client : m_map_player_info) {
		if (client.second->is_connect == true /*&& client.second->player_state == PLAYER_STATE_in_lobby*/ ){
			m_Packet_manager->send_room_info_pakcet(client.second->id, client.second->socket,
				new_room);
		}
	}

	cout << "make room success \n";
	for (auto room : m_map_game_room) {
		if (room.second->enable == false) { continue; }
		cout << "room info \n";
		cout << "room number: " << room.second->room_number << "\n";
		for (int i = 0; i < 4; ++i) {
			cout << "player " << i + 1 << " id: " << room.second->players_id[i]<<"\n";
		}
	}
}

void Iocp_server::process_join_room(const int& id, void *buff)
{
	cs_packet_request_join_room *join_room_packet = reinterpret_cast<cs_packet_request_join_room*>(buff);

	// �ش� ���ȣ�� ���ڸ��� id �־��ֱ�?
	short r_number = join_room_packet->room_number;

	bool joinflag = false;
	m_map_player_info[id]->roomList_lock.lock();
	for (int i = 0; i < 4; ++i) {
		if (m_map_game_room[r_number]->players_id[i] == -1) {
			m_map_game_room[r_number]->players_id[i] = id;
			joinflag = true;
			break;
		}
	}
	m_map_player_info[id]->roomList_lock.unlock();

	if (joinflag == true) {
		m_map_player_info[id]->room_number = r_number;
		m_map_player_info[id]->player_state = PLAYER_STATE_in_room;
		m_Packet_manager->send_join_room_ok(id, m_map_player_info[id]->socket, r_number, m_map_game_room[r_number]);


		// �ٲ� ������ ��� Ŭ���̾�Ʈ�鿡�� ����
		for (auto client : m_map_player_info) {
			if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby) {
				m_Packet_manager->send_room_info_pakcet(id, client.second->socket, m_map_game_room[r_number]);
			}
		}

		cout << "make room success \n";
		auto copyRoom = m_map_game_room;
		for (auto room : copyRoom) {
			cout << "room info \n";
			cout << "room number: " << room.second->room_number << "\n";
			for (int i = 0; i < 4; ++i) {
				cout << "player " << i + 1 << " id: " << room.second->players_id[i] << "\n";
			}
		}
	}
	else {
		cout << "join room fail \n";
	}

}

void Iocp_server::process_leaveRoom(const int & id, void * buff)
{
	cs_packet_leaveRoom *leaveRoom_packet = reinterpret_cast<cs_packet_leaveRoom*>(buff);
	short room_number = m_map_player_info[id]->room_number;
	if (room_number != -1) {
		for (short i = 0; i < 4; ++i) { // �濡�ִ� id ��Ͽ��� id ����
			if (m_map_game_room[room_number]->players_id[i] == id) {
				m_map_game_room[room_number]->players_id[i] = -1;
			}
		}
		m_map_player_info[id]->room_number = -1;
		m_map_player_info[id]->player_state = PLAYER_STATE_in_room;
		// �ڽſ��� �泪���� ������Ŷ ����
		m_Packet_manager->send_leaveRoom_ok(id, m_map_player_info[id]->socket);

		for (short i = 0; i < 4; ++i) { // �濡 ���� �÷��̾�� �������� ���� ����
			int p_id = m_map_game_room[room_number]->players_id[i];
			if (p_id != -1) {
				if (m_map_player_info[p_id]->is_connect == true && m_map_player_info[p_id]->player_state == PLAYER_STATE_in_room
					&& p_id != id) {
					m_Packet_manager->send_room_info_pakcet(p_id, m_map_player_info[p_id]->socket, m_map_game_room[room_number]);
				}
			}
		}


		bool roomEmpty = true;
		for (short i = 0; i < 4; ++i) { // ���� ������� üũ
			int p_id = m_map_game_room[room_number]->players_id[i];
			if (p_id != -1) {
				roomEmpty = false;
				break;
			}
		}
		if (roomEmpty == true) { // ���� ������� ����?
			m_map_game_room[room_number]->enable = false;
			for (auto client : m_map_player_info) {
				if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby) {
					m_Packet_manager->send_room_info_pakcet(client.first, client.second->socket, m_map_game_room[room_number]);
				}
			}

			//m_map_game_room.unsafe_erase(room_number);
		}

		// �κ�� ���� �÷��̾�� ��ü ������ ����
		for (auto room : m_map_game_room) {
			m_Packet_manager->send_room_info_pakcet(id, m_map_player_info[id]->socket, room.second);
		}

		for (auto room : m_map_game_room) {
			if (room.second->enable == false) { continue; }
			cout << "room info \n";
			cout << "room number: " << room.second->room_number << "\n";
			for (int i = 0; i < 4; ++i) {
				cout << "player " << i + 1 << " id: " << room.second->players_id[i] << "\n";
			}
		}

	}
}

void Iocp_server::process_client_state_change(const int& id, void * buff)
{
	cout << "state change" << endl;
	cs_packet_client_state_change *packet = reinterpret_cast<cs_packet_client_state_change*>(buff);
	//packet->stage_number = 2;


	if (packet->change_state == PLAYER_STATE_playing_game) {	// ���� state ����
		m_map_player_info[id]->hp = 200;
		m_map_player_info[id]->gold = 500;
		short myroom_num = m_map_player_info[id]->room_number;
		m_Packet_manager->send_game_start(id, m_map_player_info[id]->socket, 1,
			m_map_game_room[myroom_num]->wave_count, m_map_game_room[myroom_num]->portalLife);

		if (m_map_game_room[m_map_player_info[id]->room_number]->room_state == R_STATE_in_room) {
			m_map_player_info[id]->roomList_lock.lock();
			m_map_game_room[m_map_player_info[id]->room_number]->room_state = R_STATE_gameStart;
			m_map_game_room[m_map_player_info[id]->room_number]->stage_number = packet->stage_number;
			m_map_player_info[id]->roomList_lock.unlock();
			process_game_start(m_map_player_info[id]->room_number, packet->stage_number);
		}

		EVENT ev_playerStart{ id, chrono::high_resolution_clock::now() + 3s, PLAYER_GAME_START, 0 };
		add_event_to_queue(ev_playerStart);
		
		//m_map_player_info[id]->player_state = packet->change_state;

		for (short i = 0; i < 4; ++i) {	// ���� ���� Ŭ���̾�Ʈ���� put player ��ȣ ����
			int other_id = m_map_game_room[myroom_num]->players_id[i];
			if (other_id == -1) { continue; }
			if (m_map_player_info[other_id]->is_connect == true &&
				/*m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game &&*/ other_id != id) {
				//cout << id << " put "<< other_id << endl;
				m_Packet_manager->send_put_player_packet(other_id, m_map_player_info[other_id]->socket, id, 
					m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
				m_Packet_manager->send_put_player_packet(id, m_map_player_info[id]->socket, other_id, 
					m_map_player_info[other_id]->player_world_pos, m_map_player_info[other_id]->animation_state);
			}
		}
	}
}

void Iocp_server::process_install_trap(const int& id, void * buff)
{
	cs_packet_install_trap *packet = reinterpret_cast<cs_packet_install_trap*>(buff);

	short room_num = m_map_player_info[id]->room_number;

	/*short new_trapId = m_map_trapIdPool[room_num];
	auto &trapPool = m_map_trap[room_num];
	trapPool[new_trapId].set_4x4position(packet->trap_world_pos);
	trapPool[new_trapId].set_enable(true);
	trapPool[new_trapId].set_trap_type(packet->trap_type);
	m_map_trapIdPool[room_num] += 1;*/
	m_map_player_info[id]->roomList_lock.lock();
	short new_trapId = m_map_trapIdPool[room_num];
	m_map_trapIdPool[room_num] += 1;
	m_map_player_info[id]->roomList_lock.unlock();
	m_map_trap[room_num][new_trapId].set_trap_id(packet->trap_local_id);
	m_map_trap[room_num][new_trapId].set_4x4position(packet->trap_pos);
	m_map_trap[room_num][new_trapId].set_trap_type(packet->trap_type);
	if(packet->trap_type == TRAP_FIRE || packet->trap_type == TRAP_ARROW){
		check_trapDir(room_num, new_trapId, packet->trap_pos);
	}
	m_map_trap[room_num][new_trapId].set_enable(true);

	cout << "trap install" << endl;
	//m_map_player_info[id]->gold -= TRAP_COST;
	m_Packet_manager->send_stat_change(id, m_map_player_info[id]->socket, -1000, m_map_player_info[id]->gold);

	// ��ġ�� Ʈ�� ���� ����
	for (short i = 0; i < 2; ++i) {
		int other_id = m_map_game_room[room_num]->players_id[i];
		if (other_id == -1) { continue; }
		if (m_map_player_info[other_id]->is_connect == true &&
			m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game /*&& other_id != id*/) {
			m_Packet_manager->send_trap_info_packet(other_id, m_map_player_info[other_id]->socket, new_trapId, packet->trap_local_id, packet->trap_pos,
				packet->trap_type);
		}
	}
}

void Iocp_server::check_trapDir(const short & room_number, const short & trap_index, const XMFLOAT4X4 & _4x4pos)
{
	if (_4x4pos._11 == 0.0f && _4x4pos._12 == 1.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 0.0f && _4x4pos._33 == 1.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_MX);
		cout << "MX" << endl;
	}
	else if (_4x4pos._11 == 0.0f && _4x4pos._12 == -1.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 0.0f && _4x4pos._33 == 1.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_PX);
		cout << "PX" << endl;
	}
	else if (_4x4pos._11 == 1.0f && _4x4pos._12 == 0.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 1.0f && _4x4pos._33 == 0.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_MZ);
		cout << "MZ" << endl;
	}
	else if (_4x4pos._11 == 1.0f && _4x4pos._12 == 0.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == -1.0f && _4x4pos._33 == 0.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_PZ);
		cout << "PZ" << endl;
	}
}

void Iocp_server::process_player_shoot(const int & id, void * buff)
{
	cs_packet_shoot *packet = reinterpret_cast<cs_packet_shoot*>(buff);
	short player_room_number = m_map_player_info[id]->room_number;
	if (player_room_number == -1) { return; }

	if (m_map_monsterPool[player_room_number][packet->monster_id].get_isLive() == true) {
		if (packet->headShot == true) {
			m_map_monsterPool[player_room_number][packet->monster_id].decrease_hp(PLAYER_ATT*2);
		}
		else {
			m_map_monsterPool[player_room_number][packet->monster_id].decrease_hp(PLAYER_ATT);
		}
	}
}

void Iocp_server::process_game_start(const short& room_number, const short& stage_number)
{
	m_map_game_room[room_number]->portalLife = 20;
	m_map_game_room[room_number]->wave_count = 0;

	auto p = m_map_monsterPool.find(room_number);
	if (p == m_map_monsterPool.end()) {
		cout << "first make monpool \n";
		Monster *monsterArr = new Monster[MAX_MONSTER];
		ZeroMemory(monsterArr, sizeof(monsterArr));
		for (int i = 0; i < MAX_MONSTER; ++i) {
			monsterArr[i].set_id(i);
			monsterArr[i].set_isLive(false);
			monsterArr[i].set_monster_type(TYPE_DEFAULT);
			monsterArr[i].set_buffType(TRAP_BUFF_NONE);
			monsterArr[i].set_4x4position(default_wPos);
			monsterArr[i].set_HP(0);
			//DirectX::XMFLOAT4X4 w_pos;
			/*w_pos._41 = -200.f;
			w_pos._42 = -50.f;
			w_pos._43 = 150.f;
			monsterArr[i].set_4x4position(w_pos);*/
		}
		Trap *trapArr = new Trap[MAX_TRAP];
		ZeroMemory(trapArr, sizeof(trapArr));
		for (int i = 0; i < MAX_TRAP; ++i) {
			trapArr[i].set_enable(false);
			trapArr[i].set_4x4position(default_wPos);
			trapArr[i].set_trap_type(TRAP_NEEDLE);
			trapArr[i].set_wallDir(TRAP_DEFAULT);
			trapArr[i].set_wallTrapOn(false);
		}

		m_map_game_room[room_number]->gameRoom_lock.lock();
		m_map_monsterPool.insert(make_pair(room_number, monsterArr));
		m_map_trap.insert(make_pair(room_number, trapArr));
		m_map_trapIdPool.insert(make_pair(room_number, 0));
		m_map_game_room[room_number]->gameRoom_lock.unlock();
	}

	/*for (auto m : m_map_monsterPool) {
		cout << "----------room number: " << m.first << endl;
		for (int i = 0; i < MAX_MONSTER; ++i) {
			cout << "monster id: " << m.second[i].get_monster_id() << endl;
		}
	}*/
	//
	EVENT g_ev{ room_number, chrono::high_resolution_clock::now() + 10s, EV_GEN_1stWAVE_MONSTER, stage_number };
	add_event_to_queue(g_ev);
	
}

void Iocp_server::process_game_end(const short & room_number, const bool& clearFlag)
{
	for (short i = 0; i < MAX_MONSTER; ++i) {
		m_map_monsterPool[room_number][i].set_isLive(false);
	}
	for (short i = 0; i < MAX_TRAP; ++i) {
		m_map_trap[room_number][i].set_enable(false);
	}
	for (int p_id : m_map_game_room[room_number]->players_id) {
		if (p_id != -1 && m_map_player_info[p_id]->is_connect == true &&
			m_map_player_info[p_id]->player_state == PLAYER_STATE_playing_game ) {
			m_map_player_info[p_id]->player_state = PLAYER_STATE_in_room;
			m_Packet_manager->send_game_end(p_id, m_map_player_info[p_id]->socket, clearFlag);
		}
	}
	m_map_game_room[room_number]->room_state = R_STATE_in_room;
}


void Iocp_server::check_wave_end(const short& room_number)
{
	if (m_map_game_room[room_number]->enable == false) { // �� ������Ȳ
		return;
	}


	m_map_game_room[room_number]->gameRoom_lock.lock();
	short pLife = m_map_game_room[room_number]->portalLife;
	send_protalLife_update(room_number);
	m_map_game_room[room_number]->gameRoom_lock.unlock();
	if (pLife <= 0) { // ��Ż������ 0����
		m_map_game_room[room_number]->wave_on = false;
		process_game_end(room_number, false);
		return;
	}

	cout <<"roomnum:"<<room_number<< " check wave end \n";
	bool end_flag = true;
	auto copyMonsterPool = m_map_monsterPool[room_number];
	for (int i = 0; i < MAX_MONSTER; ++i) {
		if (copyMonsterPool[i].get_isLive() == true) {
			end_flag = false;
			break;
		}
	}

	if (end_flag == true) { // wave�� ����Ǹ�
		m_map_game_room[room_number]->wave_on = false;
		if (m_map_game_room[room_number]->wave_count == lastWAVE) { // ������ ���̺� ����� �������� ��Ŵ
			process_game_end(room_number, true);
			return;
		}
		// ���̺� ī��Ʈ �ø���
		// ���� ���̺� ���� �� ��Ű��
		m_map_game_room[room_number]->wave_count += 1;
		for (short p_idx = 0; p_idx < 4; ++p_idx) {
			int temp_id = m_map_game_room[room_number]->players_id[p_idx];
			if (temp_id != -1) {
				if (m_map_player_info[temp_id]->is_connect == true &&
					m_map_player_info[temp_id]->player_state == PLAYER_STATE_playing_game) {
					m_Packet_manager->send_game_info_update(temp_id, m_map_player_info[temp_id]->socket,
						m_map_game_room[room_number]->wave_count, -1000);
					m_Packet_manager->send_wave_end(temp_id, m_map_player_info[temp_id]->socket);
				}
			}
		}

		EVENT ev{ room_number, chrono::high_resolution_clock::now() + 5s, EV_GEN_MONSTER, 0 };
		add_event_to_queue(ev);
	}
	else if (end_flag == false) { // ����ȵ�
		// n���Ŀ� �ٽ� üũ�ϴ� �̺�Ʈ ����
		EVENT ev{ room_number, chrono::high_resolution_clock::now() + 3s, EV_CHECK_WAVE_END, 0 };
		add_event_to_queue(ev);
	}
}

void Iocp_server::add_monster_dead_event(const short & room_number, const short & monster_id)
{
	int room_num = room_number;
	int mon_id = monster_id;
	EVENT ev{ room_num, chrono::high_resolution_clock::now() + 1s, EV_MONSTER_DEAD, mon_id };
	add_event_to_queue(ev);
}

void Iocp_server::send_all_room_list(const int& id)
{
	for (auto room : m_map_game_room) {
		if (room.second->enable == false) { continue; }
		m_Packet_manager->send_room_info_pakcet(id, m_map_player_info[id]->socket,
			room.second);
	}
}

void Iocp_server::send_protalLife_update(const short & room_number)
{
	for (short i = 0; i < 2; ++i) {
		int p_id = m_map_game_room[room_number]->players_id[i];
		if (p_id == -1) { continue; }
		if (m_map_player_info[p_id]->is_connect == true && m_map_player_info[p_id]->player_state == PLAYER_STATE_playing_game) {
			m_Packet_manager->send_game_info_update(p_id, m_map_player_info[p_id]->socket,
				-1000, m_map_game_room[room_number]->portalLife);
		}
	}
}

void Iocp_server::get_player_db()
{
	for (auto d : m_Database_manager->m_list_player_db) {
		PLAYER_DB db;
		db.DB_key_id = d.DB_key_id;
		strcpy_s(db.name, sizeof(d.name), d.name);
		db.level = d.level;

		m_list_player_db.emplace_back(db);
	}
}

void Iocp_server::process_disconnect_client(const int& leaver_id)
{
	m_map_player_info[leaver_id]->roomList_lock.lock();
	short check_roomNum = -1; 
	if (m_map_player_info[leaver_id]->room_number != -1) { // �÷��̾ �濡 ������ ���� ��
		check_roomNum = m_map_player_info[leaver_id]->room_number;
		for (int i = 0; i < 4; ++i) {
			if (m_map_game_room[check_roomNum]->players_id[i] == leaver_id) { // leaver�� ���̵�� ������ -1�� ��ü
				m_map_game_room[check_roomNum]->players_id[i] = -1;
				break;
			}
			
		}
	}
	m_map_player_info[leaver_id]->roomList_lock.unlock();
	m_map_player_info[leaver_id]->room_number = -1; // �泪����

	for (auto c : m_map_player_info) {
		if (c.second->id == leaver_id) { continue; }
		if (c.second->is_connect == true) {
			// ��� �÷��̾�� ��������� Ŭ���̾�Ʈ�� ���̵� �����ش�
			m_Packet_manager->send_remove_player_packet(c.second->id, c.second->socket, leaver_id);
		}
	}
	if (check_roomNum != -1) { // ���� ������� �˻�
		//int copy_players[4];
		//memcpy_s(copy_players, sizeof(copy_players), m_map_game_room[check_roomNum]->players_id, sizeof(m_map_game_room[check_roomNum]->players_id));
		//if (copy_players[0] == -1 && copy_players[1] == -1 && copy_players[2] == -1 && copy_players[3] == -1) {
		//	for (int i = 0; i < 100; ++i) {
		//		// �濡 �÷��̾ ������ ���� �� false��
		//		if (m_map_monsterPool[check_roomNum] == nullptr) { return; }
		//		m_map_monsterPool[check_roomNum][i].set_isLive(false);
		//	}
		//}
		bool roomEmpty = true;
		for (short i = 0; i < 4; ++i) {
			if (m_map_game_room[check_roomNum]->players_id[i] != -1) { // leaver�� ���̵�� ������ -1�� ��ü
				roomEmpty = false;
				break;
			}
		}
		if (roomEmpty == true) {
			m_map_game_room[check_roomNum]->enable = false;
			m_map_game_room[check_roomNum]->wave_on = false;
			for (auto client : m_map_player_info) {
				if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby) {
					m_Packet_manager->send_room_info_pakcet(client.first, client.second->socket, m_map_game_room[check_roomNum]);
				}
			}
		}
	}
}

void Iocp_server::process_nameLogin(const int & id, void * buff)
{
	cs_packet_namelogin *login_packet = reinterpret_cast<cs_packet_namelogin*>(buff);
	int ret = m_Database_manager->check_nameLogin(login_packet->name);
	if (ret > 0) {
		cout << "login success" << endl;
		m_map_player_info[id]->DB_key = ret;
		m_Packet_manager->send_nameLogin_result(id, m_map_player_info[id]->socket, NAMELOGIN_SUC);
	}
	else {
		cout << "login fail" << endl;
	}
	
}

void Iocp_server::check_monster_attack(const short & room_number, const short & monster_id)
{
	int target_id = m_map_monsterPool[room_number][monster_id].get_target_id();
	if (target_id == -1) { return; }
	if (m_map_player_info[target_id]->damageCooltime == true) { return; }

	if (target_id != -1) { //Ÿ���� �����Ҷ�
		if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_ORC) {
			if (Vector3::Distance(m_map_player_info[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < ORC_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (m_map_player_info[target_id]->damageCooltime == false) {
					cout << "���ݼ���\n";
					m_map_player_info[target_id]->hp -= ORC_ATT;
					if (m_map_player_info[target_id]->hp < 0) {
						m_map_player_info[target_id]->hp = 0;
					}
					m_Packet_manager->send_stat_change(target_id, m_map_player_info[target_id]->socket, m_map_player_info[target_id]->hp, -1000);
				}

				m_map_player_info[target_id]->damageCooltime = true;
				EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_SHAMAN) {

		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_STRONGORC) {
			if (Vector3::Distance(m_map_player_info[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < STRONGORC_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (m_map_player_info[target_id]->damageCooltime == false) {
					cout << "���ݼ���\n";
					m_map_player_info[target_id]->hp -= STRONGORC_ATT;
					if (m_map_player_info[target_id]->hp < 0) {
						m_map_player_info[target_id]->hp = 0;
					}
					m_Packet_manager->send_stat_change(target_id, m_map_player_info[target_id]->socket, m_map_player_info[target_id]->hp, -1000);
				}

				m_map_player_info[target_id]->damageCooltime = true;
				EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_RIDER) {
			if (Vector3::Distance(m_map_player_info[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < RIDER_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (m_map_player_info[target_id]->damageCooltime == false) {
					cout << "���ݼ���\n";
					m_map_player_info[target_id]->hp -= RIDER_ATT;
					if (m_map_player_info[target_id]->hp < 0) {
						m_map_player_info[target_id]->hp = 0;
					}
					m_Packet_manager->send_stat_change(target_id, m_map_player_info[target_id]->socket, m_map_player_info[target_id]->hp, -1000);
				}

				m_map_player_info[target_id]->damageCooltime = true;
				EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
	}
	m_map_monsterPool[room_number][monster_id].set_attackCooltime(false);
}


void Iocp_server::process_packet(const int& id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = m_map_player_info[id]->x;
	short y = m_map_player_info[id]->y;
	switch (packet[1]){
	case CS_UP:
		t_process_player_move(id, buff);
		break;
	case CS_DOWN:
		t_process_player_move(id, buff);
		break;
	case CS_LEFT:
		t_process_player_move(id, buff);
		break;
	case CS_RIGHT:
		t_process_player_move(id, buff);
		break;
	case CS_MAKE_ROOM:
		process_make_room(id);
		break;
	case CS_REQUEST_JOIN_ROOM:
		process_join_room(id, buff);
		break;
	case CS_INSTALL_TRAP:
		process_install_trap(id, buff);
		break;
	case CS_TEST:
		cout << "�׽�Ʈ ��Ŷ ���� Ȯ�� \n";
		break;
	case CS_POS:
		process_player_move(id, buff);
		//cout << "�÷��̾� �̵� ��Ŷ Ȯ��" << endl;
		break;
	case CS_CLIENT_STATE_CHANGE:
		process_client_state_change(id, buff);
		break;
	case CS_REQUEST_NAMELOGIN:
		process_nameLogin(id, buff);
		break;
	case CS_SHOOT:
		process_player_shoot(id, buff);
		break;
	case CS_LEAVE_ROOM:
		process_leaveRoom(id, buff);
		break;
	default:
		break;
	}


}

void Iocp_server::process_gen_monster(const short& room_number, const short& stage_number)
{
	if (m_map_monsterPool.find(room_number) == m_map_monsterPool.end()) {
		cout << "room does not exist" << endl;
		return;
	}
	short wave = 0;
	if (m_map_game_room[room_number]->wave_count == 0) {
		m_map_game_room[room_number]->wave_count = wave + 1;
	}
	wave = m_map_game_room[room_number]->wave_count;
	//m_map_game_room[room_number]->wave_count = 1;
	if (m_map_game_room[room_number]->stage_number == 1) { // stage 1
		XMFLOAT3 line1 = stage1_line1_start;
		XMFLOAT3 line2 = stage1_line2_start;
		XMFLOAT3 line3 = stage1_line3_start;
		XMFLOAT3 line4 = stage1_line4_start;
		XMFLOAT3 line5 = stage1_line5_start;
		XMFLOAT3 line6 = stage1_line6_start;
		switch (wave)
		{
		case 1:		// wave1
		{
			for (int i = 0; i < 30; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 15) { // line 123
					if (i >= 14) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y, 
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 30) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y, 
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 2:		// wave2
		{
			for (int i = 0; i < 36; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 18) { // line 123
					if (i >= 15) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 36) { // line 456
					if (i >= 33) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 3:		// wave3
		{
			for (int i = 0; i < 36; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 18) { // line 123
					if (i >= 12) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 36) { // line 456
					if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 4:
		{
			short waveMax = 42;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 21) { // line 123
					if (i >= 15 && i < 18) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 18) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 36 && i < 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 5:
		{
			short waveMax = 42;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 21) { // line 123
					if (i >= 15 && i < 18) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 18) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 36 && i < 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 6:
		{
			short waveMax = 48;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 24) { // line 123
					if (i >= 15 && i < 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 39 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 7:
		{
			short waveMax = 48;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 24) { // line 123
					if (i >= 15 && i < 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 39 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 8:
		{
			short waveMax = 54;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 27) { // line 123
					if (i >= 15 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 9:
		{
			short waveMax = 54;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 27) { // line 123
					if (i >= 15 && i < 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 21) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 48) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 48) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 10:
		{
			short waveMax = 54;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 27) { // line 123
					if (i >= 11 && i < 19) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 19) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 38 && i < 46) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 46) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 11:
		{
			short waveMax = 54;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 27) { // line 123
					if (i >= 15 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 12:
		{
			short waveMax = 54;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 27) { // line 123
					if (i >= 15 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line6.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		default:
			break;
		}
	}
	else if (m_map_game_room[room_number]->stage_number == 2) { // stage2
		XMFLOAT3 line1 = stage2_line1_start;
		XMFLOAT3 line4 = stage2_line4_start;
		XMFLOAT3 line7 = stage2_line7_start;
		XMFLOAT3 line10 = stage2_line10_start;
		switch (wave) {
		case 1:
		{
			short waveMax = 28;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 7) { // line 123
					m_map_monsterPool[room_number][i].gen_sequence(2, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(2, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(2, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(2, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % 5) * MONSTER_GEN_DISTANCE)));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			cout << "stage2 gen" << endl;
			break;
		}
		default:
			break;
		}
	}
	else if (m_map_game_room[room_number]->stage_number == 3) { // stage3
		XMFLOAT3 line1 = stage3_line1_start;
		XMFLOAT3 line4 = stage3_line4_start;
		XMFLOAT3 line9 = stage3_line9_start;
		XMFLOAT3 line12 = stage3_line12_start;
		switch (wave) {
		case 1:
		{
			short waveMax = 28;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 7) { // line 123
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3( (line1.x - (i % 5) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre) )) );
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 5) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 5) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 5) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 2:
		{
			short waveMax = 28;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 7) { // line 123
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 5) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 5) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 5) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 5) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 3:
		{
			short waveMax = 36;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 9) { // line 123
					if (i >= 7) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 7) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 18) { // line 456
					if (i >= 16) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 7) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 27) { // line 789
					if (i >= 25) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 7) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 34) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 7) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 4:
		{
			short waveMax = 36;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 9) { // line 123
					if (i >= 6) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 7) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 18) { // line 456
					if (i >= 15) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 7) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 27) { // line 789
					if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 7) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 33) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 7) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 5:
		{
			short waveMax = 44;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 11) { // line 123
					if (i >= 8) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 8) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 22) { // line 456
					if (i >= 19) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 8) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 33) { // line 789
					if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 8) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 41) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 8) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 6:
		{
			short waveMax = 52;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 13) { // line 123
					if (i >= 8 && i < 11) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 11) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 9) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 26) { // line 456
					if (i >= 19 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 9) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 39) { // line 789
					if (i >= 30 && i < 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 9) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 41 && i < 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 9) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 7:
		{
			short waveMax = 60;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 15) { // line 123
					if (i >= 8 && i < 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 9) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 30) { // line 456
					if (i >= 23 && i < 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 9) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 45) { // line 789
					if (i >= 38 && i < 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 9) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 53 && i < 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 9) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 8:
		{
			short waveMax = 64;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 16) { // line 123
					if (i >= 8 && i < 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 10) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 32) { // line 456
					if (i >= 24 && i < 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 10) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 48) { // line 789
					if (i >= 40 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 10) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 56 && i < 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 10) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 9:
		{
			short waveMax = 68;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 17) { // line 123
					if (i >= 8 && i < 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 10) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 34) { // line 456
					if (i >= 25 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 10) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 51) { // line 789
					if (i >= 42 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 10) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 59 && i < 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 10) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		case 10:
		{
			short waveMax = 68;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 17) { // line 123
					if (i >= 7 && i < 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 13) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 1);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line1.x - (i % 10) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 34) { // line 456
					if (i >= 24 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line4.x + (i % 10) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < 51) { // line 789
					if (i >= 41 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line9.x + (i % 10) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 53 && i < 59) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 59) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10);
					m_map_monsterPool[room_number][i].set_position(XMFLOAT3((line12.x - (i % 10) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_isLive(true);
				}
			}
			break;
		}
		default:
			break;
		}
	}

	cout<<"room:" << room_number<<" stage:"<< stage_number<<" wave:"<< wave <<" gen complete" << endl;;
	EVENT ev{ room_number, chrono::high_resolution_clock::now() + 1s, EV_MONSTER_THREAD_RUN, 0 };
	add_event_to_queue(ev);
	m_map_game_room[room_number]->wave_on = true;

	EVENT ev_waveCheck{ room_number, chrono::high_resolution_clock::now() + 5s, EV_CHECK_WAVE_END, 0 };
	add_event_to_queue(ev_waveCheck);
}

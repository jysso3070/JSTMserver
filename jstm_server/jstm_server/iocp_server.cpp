#include "iocp_server.h"

Iocp_server::Iocp_server()
{
	Database_manager *db_manager = new Database_manager;
	m_Database_manager = db_manager;

	m_Timer = new Timer;
	m_Timer->Reset();

	cout << "monstersize: " << sizeof(Monster) << endl;
	serverInitialize();

	make_thread();

}


Iocp_server::~Iocp_server()
{
	delete m_Timer;
	m_Timer = nullptr;

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
	thread accept_thread{ &Iocp_server::do_accept_thread, this};
	thread worker_thread_1{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_2{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_3{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_4{ &Iocp_server::do_worker_thread, this };
	thread eventTimer_thread{ &Iocp_server::do_eventTimer_thread, this};

	thread monster_thread{ &Iocp_server::do_monster_thread, this };

	thread packet_count_thread{ &Iocp_server::do_packet_count, this };
	//thread collision_thread{}

	accept_thread.join();
	worker_thread_1.join();
	worker_thread_2.join();
	worker_thread_3.join();
	worker_thread_4.join();
	eventTimer_thread.join();

	monster_thread.join();

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

	// 소켓 설정 bind
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		cout << "bind fail \n";
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	m_accept_socket = listenSocket;
}

void Iocp_server::do_accept_thread()
{

	// 수신 대기 설정
	listen(m_accept_socket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(m_accept_socket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {	// 소켓연결실패시
			printf("err - accept fail \n");
			break;
		}
		else {
			printf("socket accept success \n");
		}

		int user_id = m_new_user_id++;
		PLAYER_INFO *new_player = new PLAYER_INFO;
		new_player->id = user_id;
		new_player->socket = clientSocket;
		new_player->room_number = -1;
		new_player->recv_over.wsabuf[0].len = MAX_BUFFER;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;
		new_player->recv_over.event_type = EV_RECV;
		new_player->is_connect = true;
		new_player->player_state = PLAYER_STATE_default;

		m_map_player_info.insert(make_pair(user_id, new_player)); // 플레이어 map에 인서트

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_iocp_Handle, user_id, 0); // iocp 등록

		m_Packet_manager->send_id_packet(user_id, clientSocket);

		cout << "새로운 플레이어 접속" << endl;
		//m_packet_manager->send_pos_packet(user_id, clientSocket, )

		//for (auto c : m_map_player_info) {
		//	if (c.second->id == user_id) { 
		//		// 자기 자신도 풋 플레이어?
		//	}
		//	else {
		//		if (c.second->is_connect == true) { // 접속되어 있는 플레이언지 체크
		//			// 새로운 플레이어정보를 기존의 플레이어들에게 전송
		//			m_Packet_manager->send_put_player_packet(c.second->id, c.second->socket, user_id);
		//			//새로운 플레이어에게 기존의 플레이어 정보를 전송
		//			m_Packet_manager->send_put_player_packet(user_id, clientSocket, c.second->id);
		//		}
		//		
		//	}
		//}

		//send_all_room_list(user_id); // 모든 방 정보 전송

		///////
		m_map_player_info[user_id]->x = 300;
		m_map_player_info[user_id]->y = 300;

		EVENT ev{ user_id, chrono::high_resolution_clock::now() + 10s, EV_TEST, 0 };
		add_event_to_eventTimer(ev);

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

void Iocp_server::do_worker_thread()
{
	while (true) {
		DWORD num_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		GetQueuedCompletionStatus(m_iocp_Handle, &num_byte, (PULONG_PTR)p_key, &p_over, INFINITE);

		// 클라이언트가 종료했을 경우
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

			unsigned int in_packet_size = 0;
			unsigned int saved_packet_size = 0;
			DWORD rest_byte = num_byte;

			char * temp = reinterpret_cast<char*>(over_ex->net_buf);
			char tempBuf[MAX_BUFFER];

			while (rest_byte != 0)
			{
				if (in_packet_size == 0) {
					in_packet_size = temp[0];
				}
				if (rest_byte + saved_packet_size >= in_packet_size) {
					memcpy(tempBuf + saved_packet_size, temp, in_packet_size - saved_packet_size);
					process_packet(key, tempBuf);
					temp += in_packet_size - saved_packet_size;
					rest_byte -= in_packet_size - saved_packet_size;
					in_packet_size = 0;
					saved_packet_size = 0;
				}
				else {
					memcpy(tempBuf + saved_packet_size, temp, rest_byte);
					saved_packet_size += rest_byte;
					rest_byte = 0;
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
			process_gen_monster(key, 1, 1, 1);
			delete over_ex;
		}
		else if (EV_MONSTER_DEAD == over_ex->event_type) {
			short monster_id = *(short *)(over_ex->net_buf);
			short room_number = (short)key;
			m_map_monsterPool[room_number][monster_id].set_monster_isLive(false);
			delete over_ex;
		}
		else if (EV_MONSTER_TRAP_COLLISION == over_ex->event_type) {
			short monster_id = (short)key;
			short room_number = *(short *)(over_ex->net_buf);
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			delete over_ex;
		}
		else if (EV_CHECK_WAVE_END == over_ex->event_type) {
			check_wave_end(key);
			delete over_ex;
		}
		else if (EV_GEN_MONSTER == over_ex->event_type) {
			process_gen_monster(key, 1, 1, 1);
			delete over_ex;
		}
	}
}

void Iocp_server::do_eventTimer_thread()
{
	while (true) {
		m_eventTimer_lock.lock();
		while (true == m_eventTimer_queue.empty()) {	// 이벤트 큐가 비어있으면 잠시동안 멈췄다가 다시 검사
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

		// 이벤트 별로 분류해서 iocp에 이벤트를 보내준다
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
			m_monsterThread_run = true;
		}
		else if (EV_GEN_1stWAVE_MONSTER == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_GEN_1stWAVE_MONSTER;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
			//gen_monster(p_ev.obj_id, 1, 1, 1);
		}
		else if (EV_MONSTER_DEAD == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_DEAD;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_MONSTER_TRAP_COLLISION;
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
	}
}

void Iocp_server::do_monster_thread()
{
	int cnt = 0;
	while (true) {
		if (m_monsterThread_run == false) {
			continue;
		}
		//cout << "thread run start " << endl;
		//cout << "room cnt: " << m_map_monsterPool.size() << endl;
		auto start = chrono::high_resolution_clock::now();
		for (auto &mon_pool : m_map_monsterPool) {
			MONSTER monsterPacketArr[MAX_MONSTER];
			ZeroMemory(monsterPacketArr, sizeof(monsterPacketArr));
			//memset(monsterPacketArr, 0x00, sizeof(monsterPacketArr));
			for (short i = 0; i < MAX_MONSTER; ++i) {
				monsterPacketArr[i].id = i;
				monsterPacketArr[i].isLive = false;
				if (mon_pool.second[i].get_isLive() == false) { // 연산할필요없는 것 제외
					continue; }

				// 몬스터 체력 0 이하 사망 이벤트 추가
				if (mon_pool.second[i].get_HP() <= 0) {
					add_monster_dead_event(mon_pool.first, i);
					monsterPacketArr[i].isLive = mon_pool.second[i].get_isLive();
					monsterPacketArr[i].state = -1;
					monsterPacketArr[i].animation_state = mon_pool.second[i].get_animation_state();
					monsterPacketArr[i].type = mon_pool.second[i].get_monster_type();
					monsterPacketArr[i].hp = mon_pool.second[i].get_HP();
					monsterPacketArr[i].world_pos = mon_pool.second[i].get_4x4position();
					continue; 
				}

				// 타겟플레이어가 없을때 범위안에 있는 플레이어 서치
				if (mon_pool.second[i].get_target_id() == -1) {
					int near_id = -1;
					float near_dis = 300.f;
					for (int player_index = 0; player_index < 4; ++player_index) {
						int player_id = m_map_game_room[mon_pool.first]->players_id[player_index];
						if (player_id == -1) { continue; }
						if (m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
							float dis = Vector3::Distance(m_map_player_info[player_id]->get_pos(), mon_pool.second[i].get_position());
							if (dis <= 200.f) { // 어그로 범위 내
								if (near_dis > dis) {
									near_id = player_id;
									near_dis = dis;
								}
							}
						}
					}
					mon_pool.second[i].set_target_id(near_id);
				}
				// 타겟이 있을때 몬스터 행동
				if (mon_pool.second[i].get_target_id() != -1) {
					int target_id = mon_pool.second[i].get_target_id();
					if(m_map_player_info[target_id]->player_state != PLAYER_STATE_playing_game){
						mon_pool.second[i].set_target_id(-1);
						continue;
					}
					float dis = Vector3::Distance(m_map_player_info[target_id]->get_pos(), mon_pool.second[i].get_position());
					if (dis <= 200.f && dis >= 70.f) { //어그로 범위
						mon_pool.second[i].set_target_id(target_id);
						mon_pool.second[i].set_aggro_direction(m_map_player_info[target_id]->get_pos());
						mon_pool.second[i].move_forward(5.f);
						mon_pool.second[i].set_animation_state(2);
					}
					else if (dis < 70.f) { // 공격범위
						mon_pool.second[i].set_target_id(target_id);
						mon_pool.second[i].set_aggro_direction(m_map_player_info[target_id]->get_pos());
						mon_pool.second[i].set_animation_state(3);
					}
					else {
						mon_pool.second[i].set_target_id(-1);
					}
				}
				// 타겟이 없을때 행동
				else {
					mon_pool.second[i].process_move_path();
				}

				// trap collision
				for (int trap_idx = 0; trap_idx < MAX_TRAP; ++trap_idx) {
					if (mon_pool.second[i].get_isTrapCooltime() == true) { break; }
					if (m_map_trap[mon_pool.first][trap_idx].get_enable() == false) {
						continue;
					}
					float trap_dis = Vector3::Distance(m_map_trap[mon_pool.first][trap_idx].get_position(), mon_pool.second[i].get_position());
					if (trap_dis < TRAP_COLLISION_RANGE) {
						cout << "함정 피격" << endl;
						mon_pool.second[i].set_trap_cooltime(true);
						// 함정피격쿨타임적용, 3초후에 쿨타임 해제하는 이벤트 추가
						EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_TRAP_COLLISION, mon_pool.first };

					}
				}

				


				// 패킷에 들어갈 몬스터배열 값 지정
				monsterPacketArr[i].isLive = mon_pool.second[i].get_isLive();
				monsterPacketArr[i].state = -1;
				monsterPacketArr[i].animation_state = mon_pool.second[i].get_animation_state();
				monsterPacketArr[i].type = mon_pool.second[i].get_monster_type();
				monsterPacketArr[i].hp = mon_pool.second[i].get_HP();
				monsterPacketArr[i].world_pos = mon_pool.second[i].get_4x4position();
			}
			for (int i = 0; i < 4; ++i) {
				int player_id = m_map_game_room[mon_pool.first]->players_id[i];
				if (player_id != -1 && m_map_player_info[player_id]->player_state == PLAYER_STATE_playing_game) {
					m_Packet_manager->send_monster_pos(player_id, m_map_player_info[player_id]->socket, monsterPacketArr);
				}
			}
		}
		auto end = chrono::high_resolution_clock::now();
		
		//cout << "thread run end: " << cnt << endl;
		//cout << "time: " << (end - start).count() << "ns" << endl;
		
		EVENT ev{ -10, chrono::high_resolution_clock::now() + 32ms, EV_MONSTER_THREAD_RUN, 0 };
		add_event_to_eventTimer(ev);
		++cnt;
		m_monsterThread_run = false;
	}
}

void Iocp_server::do_packet_count()
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	while (true) {
		std::chrono::duration<float> sec = std::chrono::system_clock::now() - start;
		if (sec.count() > 1) {
			cout << "1초간 이동패킷수신횟수" << pakcetCount << endl;
			start = std::chrono::system_clock::now();
			pakcetCount = 0;
		}
	}
}

void Iocp_server::add_event_to_eventTimer(EVENT & ev)
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

	//for (auto c : m_map_player_info) {
	//	if (c.second->id == id) {
	//	}
	//	else {
	//		if (c.second->is_connect == true && c.second->player_state == PLAYER_STATE_playing_game) {
	//			m_Packet_manager->send_pos_packet(c.second->id, c.second->socket, id, 
	//				m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
	//			//cout << "내위치 다른플레이어에게 보내기" << endl;
	//		}
	//	}
	//}
	if (m_map_game_room[m_map_player_info[id]->room_number]->players_id == NULL) {
		return;
	}

	for (int other_id : m_map_game_room[m_map_player_info[id]->room_number]->players_id) {
		if (other_id != -1 && m_map_player_info[other_id]->is_connect == true && 
			m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game && other_id != id) {
			//m_Packet_manager->send_put_player_packet(other_id, m_map_player_info[other_id]->socket, id);
			m_Packet_manager->send_pos_packet(other_id, m_map_player_info[other_id]->socket, id,
				m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
		}
	}

	/*for (int i = 0; i < 4; ++i) {
		int other_id = m_map_game_room[m_map_player_info[id]->room_number]->players_id[i];
		if (other_id != -1 && m_map_player_info[other_id]->is_connect == true &&
			m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game && other_id != id) {
			m_Packet_manager->send_put_player_packet(other_id, m_map_player_info[other_id]->socket, id);
			m_Packet_manager->send_pos_packet(other_id, m_map_player_info[other_id]->socket, id,
				m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
		}
	}*/
}

void Iocp_server::process_make_room(const int& id)
{
	short room_num = m_new_room_num++;
	GAME_ROOM *new_room = new GAME_ROOM;
	new_room->room_number = room_num;
	new_room->room_state = R_STATE_in_room;
	new_room->wave_count = 0;
	new_room->stage_number = 1;
	new_room->portalLife = 0;
	new_room->players_id[0] = id;
	new_room->players_id[1] = -1;
	new_room->players_id[2] = -1;
	new_room->players_id[3] = -1;

	m_map_player_info[id]->roomList_lock.lock();
	m_map_game_room.insert(make_pair(room_num, new_room)); // gameroom map에 삽입
	m_map_player_info[id]->roomList_lock.unlock();

	m_map_player_info[id]->room_number = room_num;
	m_map_player_info[id]->player_state = PLAYER_STATE_in_room;

	m_Packet_manager->send_make_room_ok(id, m_map_player_info[id]->socket, room_num);

	for (auto client : m_map_player_info) {
		if (client.second->is_connect == true /*&& client.second->player_state == PLAYER_STATE_in_lobby*/ ){
			m_Packet_manager->send_room_info_pakcet(client.second->id, client.second->socket,
				*new_room);
		}
	}

	cout << "make room success \n";
	for (auto room : m_map_game_room) {
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

	// 해당 방번호에 빈자리에 id 넣어주기?
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

		// 바뀐 방정보 모든 클라이언트들에게 전송
		for (auto client : m_map_player_info) {
			if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby) {
				m_Packet_manager->send_room_info_pakcet(id, client.second->socket, *m_map_game_room[r_number]);
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

void Iocp_server::process_client_state_change(const int& id, void * buff)
{
	cs_packet_client_state_change *packet = reinterpret_cast<cs_packet_client_state_change*>(buff);
	m_map_player_info[id]->player_state = packet->change_state;
	if (packet->change_state == PLAYER_STATE_playing_game) {	// 방의 state 변경
		if (m_map_game_room[m_map_player_info[id]->room_number]->room_state == R_STATE_in_room) {
			m_map_player_info[id]->roomList_lock.lock();
			m_map_game_room[m_map_player_info[id]->room_number]->room_state = R_STATE_wait_first_wave;
			m_map_player_info[id]->roomList_lock.unlock();
			process_game_start(m_map_player_info[id]->room_number, 1);
		}

		for (int i = 0; i < 4; ++i) {	// 같은 방의 클라이언트에게 put player 상호 전송
			int other_id = m_map_game_room[m_map_player_info[id]->room_number]->players_id[i];
			if (other_id != -1 && m_map_player_info[other_id]->is_connect == true &&
				m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game && other_id != id) {
				m_Packet_manager->send_put_player_packet(other_id, m_map_player_info[other_id]->socket, id);
				m_Packet_manager->send_put_player_packet(id, m_map_player_info[id]->socket, other_id);
			}
		}
	}
}

void Iocp_server::process_install_trap(const int& id, void * buff)
{
	cs_packet_install_trap *packet = reinterpret_cast<cs_packet_install_trap*>(buff);


	// 플레이어의 방번호를 가지고 함정정보
	short room_num = m_map_player_info[id]->room_number;

	m_map_player_info[id]->roomList_lock.lock();
	/*short new_trapId = m_map_trapIdPool[room_num];
	auto &trapPool = m_map_trap[room_num];
	trapPool[new_trapId].set_4x4position(packet->trap_world_pos);
	trapPool[new_trapId].set_enable(true);
	trapPool[new_trapId].set_trap_type(packet->trap_type);
	m_map_trapIdPool[room_num] += 1;*/
	short new_trapId = m_new_trap_id++;
	m_map_trap[room_num][new_trapId].set_4x4position(packet->trap_world_pos);
	m_map_trap[room_num][new_trapId].set_enable(true);
	m_map_trap[room_num][new_trapId].set_trap_type(packet->trap_type);
	m_map_player_info[id]->roomList_lock.unlock();

	cout << "trap install" << endl;

	// 설치한 트랩 정보 전송
	for (int other_id : m_map_game_room[room_num]->players_id) {
		if (other_id != -1 && m_map_player_info[other_id]->is_connect == true &&
			m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game && other_id != id) {
			m_Packet_manager->send_trap_info_packet(other_id, m_map_player_info[other_id]->socket, new_trapId, packet->trap_world_pos,
				packet->trap_type);
		}
	}
}

void Iocp_server::process_game_start(const short& room_number, const short& stage_number)
{
	m_map_game_room[room_number]->portalLife = 20;

	Monster *monsterArr = new Monster[MAX_MONSTER];
	for (int i = 0; i < MAX_MONSTER; ++i) {
		monsterArr[i].set_id(i);
		monsterArr[i].set_monster_isLive(false);
		monsterArr[i].set_monster_type(M_TYPE_ORC);
		DirectX::XMFLOAT4X4 w_pos;
		w_pos._41 = -200.f;
		w_pos._42 = -50.f;
		w_pos._43 = 150.f;
		monsterArr[i].set_4x4position(w_pos);
	}
	m_map_monsterPool.insert(make_pair(room_number, monsterArr));

	Trap *trapArr = new Trap[MAX_TRAP];
	for (int i = 0; i < MAX_TRAP; ++i) {
		trapArr[i].set_enable(false);
	}
	m_map_trap.insert(make_pair(room_number, trapArr));
	m_map_trapIdPool.insert(make_pair(room_number, 0));

	/*for (auto m : m_map_monsterPool) {
		cout << "----------room number: " << m.first << endl;
		for (int i = 0; i < MAX_MONSTER; ++i) {
			cout << "monster id: " << m.second[i].get_monster_id() << endl;
		}
	}*/
	//
	EVENT g_ev{ room_number, chrono::high_resolution_clock::now() + 10s, EV_GEN_1stWAVE_MONSTER, 0 };
	add_event_to_eventTimer(g_ev);
	
}

void Iocp_server::process_game_end(const short & room_number, const bool& clearFlag)
{
	for (short i = 0; i < MAX_MONSTER; ++i) {
		m_map_monsterPool[room_number][i].set_monster_isLive(false);
	}
	for (int p_id : m_map_game_room[room_number]->players_id) {
		if (p_id != -1 && m_map_player_info[p_id]->is_connect == true &&
			m_map_player_info[p_id]->player_state == PLAYER_STATE_playing_game ) {
			m_map_player_info[p_id]->player_state = PLAYER_STATE_in_room;
			m_Packet_manager->send_game_end(p_id, m_map_player_info[p_id]->socket, clearFlag);
		}
	}

}

void Iocp_server::process_gen_monster(const short& room_number, const short& wave_number, const short& stage_number, const short & path_num)
{
	if (m_map_monsterPool.find(room_number) == m_map_monsterPool.end()) {
		cout << "room does not exist" << endl;
		return;
	}
	short wave = 0;
	if (m_map_game_room[room_number]->wave_count == 0) {
		m_map_game_room[room_number]->wave_count =  wave + 1;
	}
	wave = m_map_game_room[room_number]->wave_count;
	//m_map_game_room[room_number]->wave_count = 1;
	if (m_map_game_room[room_number]->stage_number == 1) // stage 1
	{
		switch (wave)
		{
		case 1:
			for (int i = 0; i < 30; ++i) {
				if (i < 5) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(1);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line1_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
				else if (i < 10) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(2);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line2_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
				else if (i < 15) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(3);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line3_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
				else if (i < 20) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(4);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line4_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
				else if (i < 25) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(5);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line5_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
				else if (i < 30) {
					m_map_monsterPool[room_number][i].set_monster_type(M_TYPE_ORC);
					m_map_monsterPool[room_number][i].set_stage_number(1);
					m_map_monsterPool[room_number][i].arrive_portal = false;
					m_map_monsterPool[room_number][i].set_pathLine(6);
					m_map_monsterPool[room_number][i].set_checkPoint(0);
					m_map_monsterPool[room_number][i].set_position(stage1_line6_start);
					m_map_monsterPool[room_number][i].set_animation_state(2);
					m_map_monsterPool[room_number][i].set_HP(100);
					m_map_monsterPool[room_number][i].set_monster_isLive(true);
				}
			}
			break;
		default:
			break;
		}
	}

	cout << "gen complete" << endl;;
}

void Iocp_server::check_wave_end(const short& room_number)
{
	bool end_flag = true;
	for (int i = 0; i < MAX_MONSTER; ++i) {
		if (m_map_monsterPool[room_number][i].get_isLive() == true) {
			end_flag = false;
			break;
		}
	}

	if (end_flag == true) { // wave가 종료되면
		if (m_map_game_room[room_number]->wave_count == 20) { // 마지막 웨이브 종료

		}
		// 웨이브 카운트 올리고
		// 다음 웨이브 몬스터 젠 시키기
		EVENT ev{ room_number, chrono::high_resolution_clock::now() + 5s, EV_GEN_MONSTER, 0 };
		add_event_to_eventTimer(ev);
	}
	else if (end_flag == false) { // 종료안됨
		// n초후에 다시 체크하는 이벤트 생성
		EVENT ev{ room_number, chrono::high_resolution_clock::now() + 5s, EV_MONSTER_DEAD, 0 };
		add_event_to_eventTimer(ev);
	}
}

void Iocp_server::add_monster_dead_event(const short & room_number, const short & monster_id)
{
	int room_num = room_number;
	int mon_id = monster_id;
	EVENT ev{ room_num, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_DEAD, mon_id };
	add_event_to_eventTimer(ev);
}

void Iocp_server::send_all_room_list(const int& id)
{
	auto copyRoom = m_map_game_room;
	for (auto room_info : copyRoom) {
		m_Packet_manager->send_room_info_pakcet(id, m_map_player_info[id]->socket,
			*room_info.second);
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
	if (m_map_player_info[leaver_id]->room_number != -1) { // 플레이어가 방에 접속해 있을 때
		check_roomNum = m_map_player_info[leaver_id]->room_number;
		for (int i = 0; i < 4; ++i) {
			if (m_map_game_room[m_map_player_info[leaver_id]->room_number]->players_id[i] == leaver_id) { // leaver의 아이디와 같으면 -1로 대체
				m_map_game_room[m_map_player_info[leaver_id]->room_number]->players_id[i] = -1;
				break;
			}
			
		}
	}
	m_map_player_info[leaver_id]->roomList_lock.unlock();
	m_map_player_info[leaver_id]->room_number = -1; // 방나가기

	for (auto c : m_map_player_info) {
		if (c.second->id == leaver_id) {
		}
		else {
			if (c.second->is_connect == true) {
				// 모든 플레이어에게 접속종료된 클라이언트의 아이디를 보내준다
				m_Packet_manager->send_remove_player_packet(c.second->id, c.second->socket, leaver_id);
			}
		}
	}
	if (check_roomNum != -1) { // 방이 비었는지 검사
		int copy_players[4];
		memcpy_s(copy_players, sizeof(copy_players), m_map_game_room[check_roomNum]->players_id, sizeof(m_map_game_room[check_roomNum]->players_id));
		if (copy_players[0] == -1 && copy_players[1] == -1 && copy_players[2] == -1 && copy_players[3] == -1) {
			for (int i = 0; i < 100; ++i) {
				// 방에 플레이어가 없으면 몬스터 다 false로
				if (m_map_monsterPool[check_roomNum] == nullptr) { return; }
				m_map_monsterPool[check_roomNum][i].set_monster_isLive(false);
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
	}
	else {
		cout << "login fail" << endl;
	}
	
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
		cout << "테스트 패킷 전송 확인 \n";
		break;
	case CS_POS:
		process_player_move(id, buff);
		//cout << "플레이어 이동 패킷 확인" << endl;
		break;
	case CS_CLIENT_STATE_CHANGE:
		process_client_state_change(id, buff);
		break;
	case CS_REQUEST_NAMELOGIN:
		process_nameLogin(id, buff);
		break;
	default:
		break;
	}


}

void Iocp_server::send_id_packet(int id)
{
	sc_packet_send_id packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_SEND_ID;
	m_Packet_manager->send_packet(id, m_map_player_info[id]->socket, &packet);
}

void Iocp_server::send_pos_packet(int id)
{
	sc_packet_pos packet;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = m_map_player_info[id]->x;
	packet.y = m_map_player_info[id]->y;
	m_Packet_manager->send_packet(id, m_map_player_info[id]->socket, &packet);
}

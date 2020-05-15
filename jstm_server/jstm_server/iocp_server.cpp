#include "iocp_server.h"

Iocp_server::Iocp_server()
{
	Database_manager *db_manager = new Database_manager;
	m_Database_manager = db_manager;

	m_Timer = new Timer;
	m_Timer->Reset();

	Initialize();

	make_thread();
}


Iocp_server::~Iocp_server()
{
	delete m_Timer;
	m_Timer = nullptr;

	WSACleanup();
}

void Iocp_server::Initialize()
{
	//WSADATA WSAData;
	//WSAStartup(MAKEWORD(2, 2), &WSAData);

	m_iocp_Handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	m_new_user_id = 0;
	m_new_room_num = 1;

	m_Server_manager->get_server_ipAddress();
	m_Server_manager->get_cpu_count();
	//init_DB();

	init_socket();
}

void Iocp_server::make_thread()
{
	thread accept_thread{ &Iocp_server::do_accept_thread, this};
	thread worker_thread_1{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_2{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_3{ &Iocp_server::do_worker_thread, this};
	thread worker_thread_4{ &Iocp_server::do_worker_thread, this };
	thread worker_thread_5{ &Iocp_server::do_worker_thread, this };
	thread eventTimer_thread{ &Iocp_server::do_eventTimer_thread, this};

	thread tempthread{ &Iocp_server::doTempThread, this };
	//thread collision_thread{}

	accept_thread.join();
	worker_thread_1.join();
	worker_thread_2.join();
	worker_thread_3.join();
	worker_thread_4.join();
	worker_thread_5.join();
	eventTimer_thread.join();

	tempthread.join();

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

		for (auto c : m_map_player_info) {
			if (c.second->id == user_id) { 
				// 자기 자신도 풋 플레이어?
			}
			else {
				if (c.second->is_connect == true) { // 접속되어 있는 플레이언지 체크
					// 새로운 플레이어정보를 기존의 플레이어들에게 전송
					m_Packet_manager->send_put_player_packet(c.second->id, c.second->socket, user_id);
					//새로운 플레이어에게 기존의 플레이어 정보를 전송
					m_Packet_manager->send_put_player_packet(user_id, clientSocket, c.second->id);
				}
				
			}
		}

		//send_all_room_list(user_id); // 모든 방 정보 전송

		///////
		m_map_player_info[user_id]->x = 300;
		m_map_player_info[user_id]->y = 300;

		EVENT ev{ user_id, chrono::high_resolution_clock::now() + 3s, EV_TEST, 0 };
		add_event_to_eventTimer(ev);
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

		SOCKET client_s = m_map_player_info[key]->socket;
		// 클라이언트가 종료했을 경우
		if (0 == num_byte) { 
			closesocket(client_s);
			m_map_player_info[key]->is_connect = false;
			process_disconnect_client(key);
			continue;
		}

		OVER_EX *over_ex = reinterpret_cast<OVER_EX *> (p_over);

		if (EV_RECV == over_ex->event_type) {
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

			//std::string new_name = "qqq";
			//m_database_manager->sql_insert_new_data(m_database_manager->m_list_player_db.size(), new_name);
			/*get_player_db();
			for (auto d : m_list_player_db) {
				cout << "name: " << d.name <<"."<< endl;
			}*/
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
		if (EV_TEST == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_type = EV_TEST;
			PostQueuedCompletionStatus(m_iocp_Handle, 1, p_ev.obj_id, &over_ex->over);
		}
	}
}

void Iocp_server::do_monster_thread()
{
	while (m_monsterThread_run) {

	}
}

void Iocp_server::doTempThread()
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	while (true) {
		std::chrono::duration<float> sec = std::chrono::system_clock::now() - start;
		if (sec.count() > 1) {
			//cout << "1초간 이동패킷수신횟수" << pakcetCount << endl;
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

void Iocp_server::t_process_player_move(int id, void * buff)
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

void Iocp_server::process_player_move(int id, void * buff)
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

	for (auto c : m_map_player_info) {
		if (c.second->id == id) {
		}
		else {
			if (c.second->is_connect == true && c.second->player_state == PLAYER_STATE_playing_game) {
				m_Packet_manager->send_pos_packet(c.second->id, c.second->socket, id, 
					m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
				//cout << "내위치 다른플레이어에게 보내기" << endl;
			}
		}
	}

	//m_Packet_manager->send_pos_packet(id, m_map_player_info[id]->socket, id, 
		//m_map_player_info[id]->player_world_pos, m_map_player_info[id]->animation_state);
}

void Iocp_server::process_make_room(int id)
{
	short room_num = m_new_room_num++;
	GAME_ROOM *new_room = new GAME_ROOM;
	new_room->room_number = room_num;
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
		if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby){
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

void Iocp_server::process_join_room(int id, void *buff)
{
	cs_packet_request_join_room *join_room_packet = reinterpret_cast<cs_packet_request_join_room*>(buff);

	// 해당 방번호에 빈자리에 id 넣어주기?
	short r_number = join_room_packet->room_number;

	m_map_player_info[id]->roomList_lock.lock();

	for (int i = 0; i < 4; ++i) {
		if (m_map_game_room[r_number]->players_id[i] == -1) {
			m_map_game_room[r_number]->players_id[i] = id;
			break;
		}
	}
	
	m_map_player_info[id]->roomList_lock.unlock();

	// 바뀐 방정보 모든 클라이언트들에게 전송
	for (auto client : m_map_player_info) {
		if (client.second->is_connect == true && client.second->player_state == PLAYER_STATE_in_lobby) {
			m_Packet_manager->send_room_info_pakcet(id, client.second->socket, *m_map_game_room[r_number]);
		}
	}

	cout << "make room success \n";
	for (auto room : m_map_game_room) {
		cout << "room info \n";
		cout << "room number: " << room.second->room_number << "\n";
		for (int i = 0; i < 4; ++i) {
			cout << "player " << i + 1 << " id: " << room.second->players_id[i] << "\n";
		}
	}

}

void Iocp_server::process_client_state_change(int id, void * buff)
{
	cs_packet_client_state_change *packet = reinterpret_cast<cs_packet_client_state_change*>(buff);
	m_map_player_info[id]->player_state = packet->change_state;
}

void Iocp_server::process_install_trap(int id, void * buff)
{
	cs_packet_install_trap *packet = reinterpret_cast<cs_packet_install_trap*>(buff);

	Trap *t = new Trap;
	t->set_trap_pos(packet->trap_world_pos);
	t->set_trap_type(packet->trap_type);

	// 플레이어의 방번호를 가지고 함정정보 insert
	m_map_player_info[id]->roomList_lock.lock();
	m_map_trap[1].emplace_back(*t);
	m_map_player_info[id]->roomList_lock.unlock();

	// 설치한 트랩 정보 전송
	for (auto c : m_map_player_info) {
		if (c.second->is_connect == true) {
			m_Packet_manager->send_trap_info_packet(c.second->id, c.second->socket, t->get_pos(), t->get_type() );
		}
	}
}

void Iocp_server::send_all_room_list(int id)
{
	for (auto room_info : m_map_game_room) {
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

void Iocp_server::process_disconnect_client(int leaver_id)
{
	m_map_player_info[leaver_id]->roomList_lock.lock();
	if (m_map_player_info[leaver_id]->room_number != -1) { // 플레이어가 방에 접속해 있을 때
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
}


void Iocp_server::process_packet(int id, void * buff)
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

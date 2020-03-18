#include "iocp_server.h"


iocp_server::iocp_server()
{
	database_manager *db_manager = new database_manager;
	m_database_manager = db_manager;


	Initialize();

	make_thread();
}


iocp_server::~iocp_server()
{
}

void iocp_server::Initialize()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	m_iocp_Handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	m_new_user_id = 0;
	m_new_room_num = 1;

	get_this_cpu_count();
	get_server_IPaddress();
	init_DB();
}

void iocp_server::get_server_IPaddress()
{
	// Winsock Start - winsock.dll 로드
	WSADATA	WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cout << "Error - Can not load 'winsock.dll file\n'";
		return;
	}

	/*int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	char				hostname[50];
	char				ipaddr[50];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	gethostname(hostname, sizeof(hostname));
	status = getaddrinfo(hostname, "80", &hints, &servinfo);
	inet_ntop(servinfo->ai_family,servinfo->ai_addr, ipaddr, sizeof(ipaddr));
	cout << ipaddr << endl;*/

	/*PHOSTENT	hostinfo;
	char				hostname[50];
	char				ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int err_no = gethostname(hostname, sizeof(hostname));
	if (err_no == 0) {
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostinfo->h_addr_list[0])));
	}
	WSACleanup();
	cout << "Server IP Address" << ipaddr << endl;*/
}

void iocp_server::get_this_cpu_count()
{
	// CPU , Thread 개수 확인
	SYSTEM_INFO	si; // CPU 개수 확인용
	GetSystemInfo(&si); // 시스템 정보를 받아온다.
	int CpuCore = static_cast<int>(si.dwNumberOfProcessors);
	int NumWorkerThread = static_cast<int>(CpuCore * 2 - 2);
	cout << "CPU Core Count: " << CpuCore << "\tThread: " << NumWorkerThread << endl;
}


void iocp_server::make_thread()
{
	thread accept_thread{ &iocp_server::do_accept_thread, this };
	thread worker_thread{ &iocp_server::do_worker_thread, this };
	thread timer_thread{ &iocp_server::do_timer_thread, this };

	accept_thread.join();
	worker_thread.join();
	timer_thread.join();

}

void iocp_server::init_DB()
{
	m_database_manager->sql_load_database();
	//m_database_manager->sql_update_data(1, 3);
}

void iocp_server::do_accept_thread()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	// 소켓 생성
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	// 서버 정보 생성
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

	// 수신 대기 설정
	listen(listenSocket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
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
		new_player->is_connect = true;
		new_player->recv_over.wsabuf[0].len = MAX_BUFFER;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;
		new_player->recv_over.event_type = EV_RECV;

		m_player_info.insert(make_pair(user_id, new_player));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_iocp_Handle, user_id, 0);

		m_packet_manager->send_id_packet(user_id, clientSocket);

		send_all_room_list(user_id);

		///////
		m_player_info[user_id]->x = 300;
		m_player_info[user_id]->y = 300;

		EVENT ev{ user_id, chrono::high_resolution_clock::now() + 3s, EV_TEST, 0 };
		add_timer(ev);
		//////


		memset(&m_player_info[user_id]->recv_over.over, 0, sizeof(m_player_info[user_id]->recv_over.over));
		flags = 0;
		int ret = WSARecv(clientSocket, m_player_info[user_id]->recv_over.wsabuf, 1, NULL,
			&flags, &(m_player_info[user_id]->recv_over.over), NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				error_display("WSARecv Error :", err_no);
		}
	}

}

void iocp_server::do_worker_thread()
{
	while (true) {
		DWORD num_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		GetQueuedCompletionStatus(m_iocp_Handle, &num_byte, (PULONG_PTR)p_key, &p_over, INFINITE);

		SOCKET client_s = m_player_info[key]->socket;
		// 클라이언트가 종료했을 경우
		if (0 == num_byte) { 
			closesocket(client_s);
			m_player_info[key]->is_connect = false;

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

			std::string new_name = "qqq";
			m_database_manager->sql_insert_new_data(m_database_manager->m_list_player_db.size(), new_name);
			/*get_player_db();
			for (auto d : m_list_player_db) {
				cout << "name: " << d.name <<"."<< endl;
			}*/
		}
	}
}

void iocp_server::do_timer_thread()
{
	while (true) {
		m_timer_lock.lock();
		while (true == m_timer_queue.empty()) {	// 이벤트 큐가 비어있으면 잠시동안 멈췄다가 다시 검사
			m_timer_lock.unlock();
			this_thread::sleep_for(10ms);
			m_timer_lock.lock();
		}
		const EVENT &ev = m_timer_queue.top();
		if (ev.wakeup_time > chrono::high_resolution_clock::now()) {
			m_timer_lock.unlock();
			this_thread::sleep_for(10ms);
			continue;
		}

		EVENT p_ev = ev;
		m_timer_queue.pop();
		m_timer_lock.unlock();

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

void iocp_server::add_timer(EVENT & ev)
{
	m_timer_lock.lock();
	m_timer_queue.push(ev);
	m_timer_lock.unlock();
}

void iocp_server::process_player_move(int id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = m_player_info[id]->x;
	short y = m_player_info[id]->y;
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

	m_player_info[id]->x = x;
	m_player_info[id]->y = y;

	m_packet_manager->send_pos_packet(id, m_player_info[id]->socket, x, y);

}

void iocp_server::process_make_room(int id)
{
	short room_num = m_new_room_num++;
	GAME_ROOM *new_room = new GAME_ROOM;
	new_room->guest_id = -1;
	new_room->host_id = id;
	new_room->room_number = room_num;

	m_list_game_room.emplace_back(*new_room);

	for (auto client : m_player_info) {
		m_packet_manager->send_room_list_pakcet(client.second->id, client.second->socket,
			new_room->room_number, new_room->host_id, new_room->guest_id);
	}

	cout << "make room success \n";
}

void iocp_server::send_all_room_list(int id)
{
	for (auto room_info : m_list_game_room) {
		m_packet_manager->send_room_list_pakcet(id, m_player_info[id]->socket,
			room_info.room_number, room_info.host_id, room_info.guest_id);
	}
}

void iocp_server::get_player_db()
{
	for (auto d : m_database_manager->m_list_player_db) {
		PLAYER_DB db;
		db.DB_key_id = d.DB_key_id;
		strcpy_s(db.name, sizeof(d.name), d.name);
		db.level = d.level;

		m_list_player_db.emplace_back(db);
	}
}


void iocp_server::process_packet(int id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = m_player_info[id]->x;
	short y = m_player_info[id]->y;
	switch (packet[1]){
	case CS_UP:
		process_player_move(id, buff);
		break;
	case CS_DOWN:
		process_player_move(id, buff);
		break;
	case CS_LEFT:
		process_player_move(id, buff);
		break;
	case CS_RIGHT:
		process_player_move(id, buff);
		break;
	case CS_MAKE_ROOM:
		process_make_room(id);
		break;
	case CS_REQUEST_JOIN_ROOM:
		break;
	default:
		break;
	}


}

void iocp_server::send_id_packet(int id)
{
	sc_packet_send_id packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_SEND_ID;
	m_packet_manager->send_packet(id, m_player_info[id]->socket, &packet);
}

void iocp_server::send_pos_packet(int id)
{
	sc_packet_pos packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = m_player_info[id]->x;
	packet.y = m_player_info[id]->y;
	m_packet_manager->send_packet(id, m_player_info[id]->socket, &packet);
}

void iocp_server::error_display(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::cout << L"에러: " << lpMsgBuf << std::endl;

	while (true);
	LocalFree(lpMsgBuf);
}

void iocp_server::error_quit(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
	std::cout << msg;
	std::cout << L"에러: " << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	exit(-1);
}

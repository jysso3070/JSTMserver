#include "iocp_server.h"

Iocp_server::Iocp_server()
{
	Database_manager *db_manager = new Database_manager;
	mDatabaseManager = db_manager;

	serverInitialize();
	init_wPos();

	makeThread();

}


Iocp_server::~Iocp_server()
{

	WSACleanup();
}

void Iocp_server::serverInitialize()
{

	DWORD maxThread = 0;
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, maxThread);

	mNewUserId = 0;
	m_new_room_num = 1;
	m_new_trap_id = 1;

	mServerManager->get_server_ipAddress();
	mServerManager->get_cpu_count();
	//init_DB();

	init_socket();

}

void Iocp_server::makeThread()
{
	mThreadsPool.emplace_back([this]() {run_acceptThread(); });

	for (int i = 0; i < maxWorkerThread; ++i) {
		mThreadsPool.emplace_back([this]() {runMainThread(); });
	}

	mThreadsPool.emplace_back([this]() {run_timerThread(); });

	for (auto& th : mThreadsPool) {
		if (th.joinable()) {
			th.join();
		}
	}

	//thread packet_count_thread{ &Iocp_server::run_packet_countThread, this };
	//packet_count_thread.join();

}

void Iocp_server::init_DB()
{
	mDatabaseManager->sql_load_database();
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
	mAcceptSocket = listenSocket;
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
	listen(mAcceptSocket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(mAcceptSocket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {	// ���Ͽ�����н�
			printf("err - accept fail \n");
			break;
		}
		else {
			printf("socket accept success \n");
		}

		int userId = mNewUserId++;
		PLAYER_INFO *new_player = new PLAYER_INFO;
		ZeroMemory(new_player, sizeof(new_player));
		new_player->id = userId;
		new_player->socket = clientSocket;
		new_player->room_number = -1;
		new_player->animation_state = 0;
		new_player->hp = 200;
		new_player->gold = 500;
		new_player->damageCooltime = false;
		new_player->recv_over.wsabuf[0].len = MAX_BUFFER;
		new_player->recv_over.wsabuf[0].buf = new_player->recv_over.net_buf;
		new_player->recv_over.eventType = EV_RECV;
		new_player->playerState = PLAYER_STATE_in_lobby;
		new_player->player_world_pos = default_wPos;
		new_player->is_connect = true;

		mMapPlayerInfo.insert(make_pair(userId, new_player)); // �÷��̾� map�� �μ�Ʈ

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), mIocpHandle, userId, 0); // iocp ���

		mPacketManager->send_id_packet(userId, clientSocket);

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

		send_all_room_list(userId); // ��� �� ���� ����

		///////
		mMapPlayerInfo[userId]->x = 300;
		mMapPlayerInfo[userId]->y = 300;

		/*EVENT ev{ user_id, chrono::high_resolution_clock::now() + 10s, EV_TEST, 0 };
		add_event_to_queue(ev);*/

		/*EVENT tev{ -10, chrono::high_resolution_clock::now() + 5s, EV_MONSTER_THREAD_RUN, 0 };
		add_event_to_eventTimer(tev);*/

		//////


		memset(&mMapPlayerInfo[userId]->recv_over.over, 0, sizeof(mMapPlayerInfo[userId]->recv_over.over));
		flags = 0;
		int ret = WSARecv(clientSocket, mMapPlayerInfo[userId]->recv_over.wsabuf, 1, NULL,
			&flags, &(mMapPlayerInfo[userId]->recv_over.over), NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				mServerManager->socket_error_display("WSARecv Error : ", err_no);
		}
	}

}

void Iocp_server::runMainThread()
{
	while (true) {
		DWORD ioByte;
		ULONG key;
		PULONG pKey = &key;
		WSAOVERLAPPED* pOver;
		GetQueuedCompletionStatus(mIocpHandle, &ioByte, (PULONG_PTR)pKey, &pOver, INFINITE);

		// Ŭ���̾�Ʈ�� �������� ���
		if (0 == ioByte) {
			SOCKET client_s = mMapPlayerInfo[key]->socket;
			closesocket(client_s);
			mMapPlayerInfo[key]->is_connect = false;
			mMapPlayerInfo[key]->playerState = PLAYER_STATE_default;
			process_disconnect_client(key);
			continue;
		}
		OVER_EX *overEx = reinterpret_cast<OVER_EX *> (pOver);
		switch (overEx->eventType)
		{
		case EV_SEND:
		{
			delete overEx;
			overEx = nullptr;
			break;
		}
		case EV_RECV:
		{
			SOCKET client_s = mMapPlayerInfo[key]->socket;
			overEx->net_buf[ioByte] = 0;
			//process_packet(key, over_ex->net_buf);

			unsigned int cur_packet_size = 0;
			unsigned int saved_packet_size = 0;
			DWORD buf_byte = ioByte;

			char* temp = reinterpret_cast<char*>(overEx->net_buf);
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
			memset(&overEx->over, 0x00, sizeof(WSAOVERLAPPED));
			WSARecv(client_s, overEx->wsabuf, 1, 0, &flags, &overEx->over, 0);
			break;
		}
		case EV_TEST:
		{
			cout << "test event ! \n";
			char tname[11] = "jys";
			mDatabaseManager->check_nameLogin(tname);
			//std::string new_name = "qqq";
			//m_database_manager->sql_insert_new_data(m_database_manager->m_list_player_db.size(), new_name);
			/*get_player_db();
			for (auto d : m_list_player_db) {
				cout << "name: " << d.name <<"."<< endl;
			}*/
			delete overEx;
			break;
		}
		case EV_GEN_1stWAVE_MONSTER:
		{
			short stage_number = *(short*)(overEx->net_buf);
			process_gen_monster(key, stage_number);
			delete overEx;
			break;
		}
		case EV_MONSTER_DEAD:
		{
			short monster_id = *(short*)(overEx->net_buf);
			short room_number = (short)key;
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_isLive(false);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			//cout << monster_id << "�� false \n";
			delete overEx;
			break;
		}
		case EV_MONSTER_NEEDLE_TRAP_COLLISION:
		{
			short monster_id = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			delete overEx;
			break;
		}
		case EV_MONSTER_FIRE_TRAP_COLLISION:
		{
			short monster_id = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			delete overEx;
			break;
		}
		case EV_MONSTER_ARROW_TRAP_COLLISION:
		{
			short monster_id = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			delete overEx;
			break;
		}
		case EV_MONSTER_SLOW_TRAP_COLLISION:
		{
			short monster_id = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().lock();
			m_map_monsterPool[room_number][monster_id].set_trap_cooltime(false);
			m_map_monsterPool[room_number][monster_id].set_buffType(TRAP_BUFF_NONE);
			m_map_monsterPool[room_number][monster_id].get_monsterLock().unlock();
			delete overEx;
			break;
		}
		case EV_WALLTRAP_COLLTIME:
		{
			short trap_index = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_trap[room_number][trap_index].set_wallTrapOn(false);
			delete overEx;
			break;
		}
		case EV_MONSTER_BULLET:
		{
			short monster_id = (short)key;
			short room_number = *(short*)(overEx->net_buf);
			m_map_monsterPool[room_number][monster_id].set_bulletAnim(false);
			delete overEx;
			break;
		}
		case EV_CHECK_WAVE_END:
		{
			check_wave_end(key);
			delete overEx;
			break;
		}
		case EV_GEN_MONSTER:
		{
			short stage_number = mMapGameRoom[key]->stage_number;
			process_gen_monster(key, stage_number);
			delete overEx;
			break;
		}
		case EV_MONSTER_THREAD_RUN:
		{
			processMonsterMove(key);
			delete overEx;
			break;
		}
		case EV_MONSTER_ATTACK:
		{
			short room_number = (short)key;
			short monster_id = *(short*)(overEx->net_buf);
			check_monster_attack(room_number, monster_id);
			delete overEx;
			break;
		}
		case EV_PLAYER_DAMAGE_COOLTIME:
		{
			mMapPlayerInfo[key]->damageCooltime = false;
			delete overEx;
			break;
		}
		case EV_PROTALLIFE_UPDATE:
		{
			send_protalLife_update(key);
			delete overEx;
			break;
		}
		case PLAYER_GAME_START:
		{
			mMapPlayerInfo[key]->playerState = PLAYER_STATE_playing_game;
			delete overEx;
			break;
		}
		default:
			break;
		}
	}
}

void Iocp_server::run_timerThread()
{
	while (true) {
		m_eventTimer_lock.lock();
		while (true == m_gameLogic_queue.empty()) {	// �̺�Ʈ ť�� ��������� ��õ��� ����ٰ� �ٽ� �˻�
			m_eventTimer_lock.unlock();
			this_thread::sleep_for(10ms);
			m_eventTimer_lock.lock();
		}
		const GAME_EVENT &ev = m_gameLogic_queue.top();
		if (ev.wakeup_time > chrono::high_resolution_clock::now()) {
			m_eventTimer_lock.unlock();
			this_thread::sleep_for(10ms);
			continue;
		}

		GAME_EVENT p_ev = ev;
		m_gameLogic_queue.pop();
		m_eventTimer_lock.unlock();

		// �̺�Ʈ ���� �з��ؼ� iocp�� �̺�Ʈ�� �����ش�
		if (EV_MOVE == p_ev.event_type) { 
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MOVE;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_TEST == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_TEST;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_THREAD_RUN == p_ev.event_type) {
			//m_monsterThread_run = true;
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_THREAD_RUN;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_GEN_1stWAVE_MONSTER == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_GEN_1stWAVE_MONSTER;
			*(short *)(over_ex->net_buf) = p_ev.target_obj; // stage number;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
			//gen_monster(p_ev.obj_id, 1, 1, 1);
		}
		else if (EV_MONSTER_DEAD == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_DEAD;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_NEEDLE_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_NEEDLE_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_SLOW_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_SLOW_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_FIRE_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_FIRE_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_ARROW_TRAP_COLLISION == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_ARROW_TRAP_COLLISION;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_WALLTRAP_COLLTIME == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_WALLTRAP_COLLTIME;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_BULLET == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_BULLET;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_CHECK_WAVE_END == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_CHECK_WAVE_END;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_GEN_MONSTER == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_GEN_MONSTER;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_MONSTER_ATTACK == p_ev.event_type) { // target_id = monster idx
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_MONSTER_ATTACK;
			*(short *)(over_ex->net_buf) = p_ev.target_obj;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_PLAYER_DAMAGE_COOLTIME == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_PLAYER_DAMAGE_COOLTIME;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (EV_PROTALLIFE_UPDATE == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = EV_PROTALLIFE_UPDATE;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
		else if (PLAYER_GAME_START == p_ev.event_type) {
			OVER_EX *over_ex = new OVER_EX;
			over_ex->eventType = PLAYER_GAME_START;
			PostQueuedCompletionStatus(mIocpHandle, 1, p_ev.obj_id, &over_ex->over);
		}
	}
}

void Iocp_server::processMonsterMove(const short& roomNumber)
{
	//auto start = chrono::high_resolution_clock::now();
	auto monPool = m_map_monsterPool[roomNumber];
	MONSTER monsterPacketArr[MAX_MONSTER];
	ZeroMemory(monsterPacketArr, sizeof(monsterPacketArr));
	for (short i = 0; i < MAX_MONSTER; ++i) {
		mMapGameRoom[roomNumber]->monsterThread_lock.lock();
		monsterPacketArr[i].id = i;
		monsterPacketArr[i].isLive = false;
		monsterPacketArr[i].type = monPool[i].get_monster_type();
		monsterPacketArr[i].hp = 0;
		monsterPacketArr[i].animation_state = 0;
		monsterPacketArr[i].world_pos = default_wPos;
		mMapGameRoom[roomNumber]->monsterThread_lock.unlock();
		if (monPool[i].get_isLive() == false) { // �������ʿ���� �� ����
			//monsterPacketArr[i].isLive = mon_pool[i].get_isLive();
			continue;
		}

		
		if (monPool[i].get_arrivePortal() == true) {
			monPool[i].set_isLive(false);
			/*m_map_game_room[room_number]->monsterThread_lock.lock();
			monsterPacketArr[i].isLive = false;
			monsterPacketArr[i].animation_state = mon_pool[i].get_animation_state();
			monsterPacketArr[i].type = mon_pool[i].get_monster_type();
			monsterPacketArr[i].hp = mon_pool[i].get_HP();
			monsterPacketArr[i].world_pos = mon_pool[i].get_4x4position();
			m_map_game_room[room_number]->monsterThread_lock.unlock();*/

			// ��Ż������ ������Ʈ�ϴ� ��Ŷ ����
			//EVENT ev_portalLifeUpdate{ room_number, chrono::high_resolution_clock::now() + 32ms, EV_PROTALLIFE_UPDATE, 0 };
			//add_event_to_queue(ev_portalLifeUpdate);
			//continue;
		}

		// ���� ü�� 0 ���� ��� �̺�Ʈ �߰�
		if (monPool[i].get_HP() <= 0) {
			monPool[i].set_animation_state(M_ANIM_DEATH);
			add_monster_dead_event(roomNumber, i);
			mMapGameRoom[roomNumber]->monsterThread_lock.lock();
			monsterPacketArr[i].isLive = monPool[i].get_isLive();
			monsterPacketArr[i].animation_state = monPool[i].get_animation_state();
			monsterPacketArr[i].type = monPool[i].get_monster_type();
			monsterPacketArr[i].hp = monPool[i].get_HP();
			monsterPacketArr[i].world_pos = monPool[i].get_4x4position();
			mMapGameRoom[roomNumber]->monsterThread_lock.unlock();
			continue;
		}
		if (monPool[i].get_bulletAnim() == true) {
			monsterPacketArr[i].isLive = monPool[i].get_isLive();
			monsterPacketArr[i].animation_state = M_ANIM_DAMAGE;
			monsterPacketArr[i].type = monPool[i].get_monster_type();
			monsterPacketArr[i].hp = monPool[i].get_HP();
			monsterPacketArr[i].world_pos = monPool[i].get_4x4position();
			continue;
		}
		
		if (monPool[i].get_isLive() == true)
		{
			// Ÿ���÷��̾ ������ �����ȿ� �ִ� �÷��̾� ��ġ
			if (monPool[i].get_target_id() == -1) {
				int near_id = -1;
				float near_dis = 300.f;
				for (int player_index = 0; player_index < 4; ++player_index) {
					int player_id = mMapGameRoom[roomNumber]->players_id[player_index];
					if (player_id == -1) { continue; }
					if (mMapPlayerInfo[player_id]->playerState == PLAYER_STATE_playing_game) {
						float dis = Vector3::Distance(mMapPlayerInfo[player_id]->get_pos(), monPool[i].get_position());
						if (dis <= 200.f) { // ��׷� ���� ��
							if (near_dis > dis) {
								near_id = player_id;
								near_dis = dis;
							}
						}
					}
				}
				monPool[i].set_target_id(near_id);
			}
			// Ÿ���� ������ ���� �ൿ
			if (monPool[i].get_target_id() != -1) {
				int target_id = monPool[i].get_target_id();
				if (mMapPlayerInfo[target_id]->playerState != PLAYER_STATE_playing_game) {
					monPool[i].set_target_id(-1);
					continue;
				}
				float dis = Vector3::Distance(mMapPlayerInfo[target_id]->get_pos(), monPool[i].get_position());
				if (dis <= 200.f && dis >= ORC_ATT_RANGE) { //��׷� ����
					monPool[i].set_target_id(target_id);
					monPool[i].set_aggro_direction(mMapPlayerInfo[target_id]->get_pos());
					monPool[i].move_forward(MONSTER_MOVE_DISTANCE, monPool);
					monPool[i].set_animation_state(M_ANIM_RUN);
				}
				else if (dis < ORC_ATT_RANGE) { // ���ݹ���
					monPool[i].set_target_id(target_id);
					monPool[i].set_aggro_direction(mMapPlayerInfo[target_id]->get_pos());
					monPool[i].set_animation_state(M_ANIM_ATT);
					if (monPool[i].get_attackCooltime() == false &&
						mMapPlayerInfo[monPool[i].get_target_id()]->damageCooltime == false) {
						GAME_EVENT ev_monAttck{ roomNumber, chrono::high_resolution_clock::now() + 1s, EV_MONSTER_ATTACK, i };
						add_event_to_queue(ev_monAttck);
						monPool[i].set_attackCooltime(true);
					}
				}
				else {
					monPool[i].set_target_id(-1);
					// �����ϰŰ���
					monPool[i].aggro_release();
				}
			}
			// Ÿ���� ������ �ൿ
			else {
				//mon_pool[i].process_move_path();
				monPool[i].process_move_path_t();
				monPool[i].move_forward(MONSTER_MOVE_DISTANCE, monPool);
			}

			// trap collision
			auto coopyTrapPool = m_map_trap[roomNumber];
			for (short trap_idx = 0; trap_idx < MAX_TRAP; ++trap_idx) {
				if (monPool[i].get_isTrapCooltime() == true) { break; }
				if (coopyTrapPool[trap_idx].get_enable() == false) { continue; }

				if (coopyTrapPool[trap_idx].get_type() == TRAP_NEEDLE) {
					float trap_dis = Vector3::Distance(coopyTrapPool[trap_idx].get_position(), monPool[i].get_position());
					if (trap_dis < TRAP_NEEDLE_RANGE) {
						//cout << "needle ���� �ǰ�" << endl;
						//mon_pool[i].get_monsterLock().lock();
						monPool[i].set_trap_cooltime(true);
						monPool[i].decrease_hp(TRAP_NEEDLE_ATT);
						//mon_pool[i].get_monsterLock().unlock();
						GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_NEEDLE_TRAP_COLLISION, roomNumber };
						add_event_to_queue(trap_ev);
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_SLOW) {
					float trap_dis = Vector3::Distance(coopyTrapPool[trap_idx].get_position(), monPool[i].get_position());
					if (trap_dis < TRAP_SLOW_RANGE) {
						//cout << "slow ���� �ǰ�" << endl;
						// �����ǰ���Ÿ������, 3���Ŀ� ��Ÿ�� �����ϴ� �̺�Ʈ �߰�
						//mon_pool[i].get_monsterLock().lock();
						monPool[i].set_trap_cooltime(true);
						monPool[i].set_buffType(TRAP_BUFF_SLOW);
						//mon_pool[i].get_monsterLock().unlock();
						GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 3s, EV_MONSTER_SLOW_TRAP_COLLISION, roomNumber };
						add_event_to_queue(trap_ev);
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_FIRE) {
					volatile bool trapColli = false;
					if(coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MX) {
						if (monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x &&
							monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_FIRE_RANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_FIRE_WIDTHRANGE) {
							monPool[i].set_trap_cooltime(true);
							trapColli = true;
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_FIRE_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							//cout << "MX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PX) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_FIRE_RANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_FIRE_WIDTHRANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_FIRE_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MZ) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_FIRE_RANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_FIRE_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MZ�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PZ) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_FIRE_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_FIRE_RANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_FIRE_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PZ�� ������ �ǰ� \n";
						}
					}
					if (trapColli == true) {
						//cout << "�� ������ �ǰ� \n";
						monPool[i].decrease_hp(TRAP_FIRE_ATT);
						if (coopyTrapPool[trap_idx].get_wallTrapOn() == false) {
							coopyTrapPool[trap_idx].set_wallTrapOn(true);
							GAME_EVENT wallTrapCool{ trap_idx, chrono::high_resolution_clock::now() + 2s, EV_WALLTRAP_COLLTIME, roomNumber };
							add_event_to_queue(wallTrapCool);
							for (int i = 0; i < MAX_ROOMPLAYER; ++i) {
								int player_id = mMapGameRoom[roomNumber]->players_id[i];
								if (player_id == -1) { continue; }
								if (mMapPlayerInfo[player_id]->playerState == PLAYER_STATE_playing_game) {
									mPacketManager->send_wallTrapOn(player_id, mMapPlayerInfo[player_id]->socket, trap_idx);
								}
							}
							//cout << "�� ������ ��Ŷ���� \n";
						}
					}
				}
				else if (coopyTrapPool[trap_idx].get_type() == TRAP_ARROW) {
					volatile bool trapColli = false;
					if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MX) {
						if (monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x &&
							monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_ARROW_RANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_ARROW_WIDTHRANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_ARROW_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PX) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_ARROW_RANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_ARROW_WIDTHRANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_ARROW_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PX�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_MZ) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z - TRAP_ARROW_RANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_ARROW_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "MZ�� ������ �ǰ� \n";
						}
					}
					else if (coopyTrapPool[trap_idx].get_wallDir() == WALL_TRAP_PZ) {
						if (monPool[i].get_position().x > coopyTrapPool[trap_idx].get_position().x - TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().x < coopyTrapPool[trap_idx].get_position().x + TRAP_ARROW_WIDTHRANGE &&
							monPool[i].get_position().z > coopyTrapPool[trap_idx].get_position().z &&
							monPool[i].get_position().z < coopyTrapPool[trap_idx].get_position().z + TRAP_ARROW_RANGE) {
							monPool[i].set_trap_cooltime(true);
							GAME_EVENT trap_ev{ i, chrono::high_resolution_clock::now() + 2s, EV_MONSTER_ARROW_TRAP_COLLISION, roomNumber };
							add_event_to_queue(trap_ev);
							trapColli = true;
							//cout << "PZ�� ������ �ǰ� \n";
						}
					}
					if (trapColli == true) {
						//cout << "�� ȭ������ �ǰ� \n";
						monPool[i].decrease_hp(TRAP_ARROW_ATT);
						if (coopyTrapPool[trap_idx].get_wallTrapOn() == false) {
							coopyTrapPool[trap_idx].set_wallTrapOn(true);
							GAME_EVENT wallTrapCool{ trap_idx, chrono::high_resolution_clock::now() + 2s, EV_WALLTRAP_COLLTIME, roomNumber };
							add_event_to_queue(wallTrapCool);
							for (int i = 0; i < MAX_ROOMPLAYER; ++i) {
								int player_id = mMapGameRoom[roomNumber]->players_id[i];
								if (player_id == -1) { continue; }
								if (mMapPlayerInfo[player_id]->playerState == PLAYER_STATE_playing_game) {
									mPacketManager->send_wallTrapOn(player_id, mMapPlayerInfo[player_id]->socket, trap_idx);
								}
							}
							//cout << "�� ȭ������ ��Ŷ���� \n";
						}
					}
				}
			}
		}

		// ��Ż�� ������ ����
		if (monPool[i].get_arrivePortal() == true) {
			monPool[i].set_isLive(false);
#ifndef TESTMODE
			m_map_game_room[room_number]->portalLife -= 1;
#endif
		}


		// ��Ŷ�� �� ���͹迭 �� ����
		mMapGameRoom[roomNumber]->monsterThread_lock.lock();
		if (monPool[i].get_isLive() == true) {
			monsterPacketArr[i].isLive = true;
		}
		else {
			monsterPacketArr[i].isLive = false;
		}
		monsterPacketArr[i].animation_state = monPool[i].get_animation_state();
		monsterPacketArr[i].type = monPool[i].get_monster_type();
		monsterPacketArr[i].hp = monPool[i].get_HP();
		monsterPacketArr[i].world_pos = monPool[i].get_4x4position();
		mMapGameRoom[roomNumber]->monsterThread_lock.unlock();
	}

	for (int i = 0; i < MAX_ROOMPLAYER; ++i) {
		int playerId = mMapGameRoom[roomNumber]->players_id[i];
		if (playerId == PLAYER_NONE) { continue; }
		if (mMapPlayerInfo[playerId]->playerState == PLAYER_STATE_playing_game) {
			mPacketManager->sendMonsterPos(playerId, mMapPlayerInfo[playerId]->socket, monsterPacketArr);
		}
	}

	if (mMapGameRoom[roomNumber]->wave_on == true) {
		GAME_EVENT ev{ roomNumber, chrono::high_resolution_clock::now() + 50ms, EV_MONSTER_THREAD_RUN, 0 };
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
			//cout << "1�ʰ� �̵���Ŷ����Ƚ��" << pakcetCount << endl;
			start = std::chrono::system_clock::now();
			pakcetCount = 0;
		}
	}
}

void Iocp_server::add_event_to_queue(GAME_EVENT & ev)
{
	m_eventTimer_lock.lock();
	m_gameLogic_queue.push(ev);
	m_eventTimer_lock.unlock();
}

void Iocp_server::t_process_player_move(const int& id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = mMapPlayerInfo[id]->x;
	short y = mMapPlayerInfo[id]->y;
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

	mMapPlayerInfo[id]->x = x;
	mMapPlayerInfo[id]->y = y;

	mPacketManager->t_send_pos_packet(id, mMapPlayerInfo[id]->socket, x, y);
	cout << "x: " << mMapPlayerInfo[id]->x << ", y: " << mMapPlayerInfo[id]->y << endl;

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

	mMapPlayerInfo[id]->player_world_pos = pos_packet->player_world_pos;
	mMapPlayerInfo[id]->animation_state = pos_packet->animation_state;
	//m_map_player_info[id]->player_state = PLAYER_STATE_playing_game;

	if (mMapGameRoom[mMapPlayerInfo[id]->room_number]->players_id == NULL) {
		return;
	}

	short room_number = mMapPlayerInfo[id]->room_number;
	for (short i = 0; i < MAX_ROOMPLAYER; ++i) {
		int other_id = mMapGameRoom[room_number]->players_id[i];
		if (other_id == -1) { continue; }
		if (mMapPlayerInfo[other_id]->is_connect == true && 
			mMapPlayerInfo[other_id]->playerState == PLAYER_STATE_playing_game && other_id != id) {
			mPacketManager->send_pos_packet(other_id, mMapPlayerInfo[other_id]->socket, id,
				mMapPlayerInfo[id]->player_world_pos, mMapPlayerInfo[id]->animation_state);
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

	mMapPlayerInfo[id]->roomList_lock.lock();
	mMapGameRoom.insert(make_pair(room_num, new_room)); // gameroom map�� ����
	mMapPlayerInfo[id]->roomList_lock.unlock();

	mMapPlayerInfo[id]->room_number = room_num;
	mMapPlayerInfo[id]->playerState = PLAYER_STATE_in_room;

	mPacketManager->send_make_room_ok(id, mMapPlayerInfo[id]->socket, room_num);

	for (auto client : mMapPlayerInfo) {
		if (client.second->is_connect == true /*&& client.second->player_state == PLAYER_STATE_in_lobby*/ ){
			mPacketManager->send_room_info_pakcet(client.second->id, client.second->socket,
				new_room);
		}
	}

	cout << "make room success \n";
	for (auto room : mMapGameRoom) {
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
	mMapPlayerInfo[id]->roomList_lock.lock();
	for (int i = 0; i < MAX_ROOMPLAYER; ++i) {
		if (mMapGameRoom[r_number]->players_id[i] == -1) {
			mMapGameRoom[r_number]->players_id[i] = id;
			joinflag = true;
			break;
		}
	}
	mMapPlayerInfo[id]->roomList_lock.unlock();

	if (joinflag == true) {
		mMapPlayerInfo[id]->room_number = r_number;
		mMapPlayerInfo[id]->playerState = PLAYER_STATE_in_room;
		mPacketManager->send_join_room_ok(id, mMapPlayerInfo[id]->socket, r_number, mMapGameRoom[r_number]);


		// �ٲ� ������ ��� Ŭ���̾�Ʈ�鿡�� ����
		for (auto client : mMapPlayerInfo) {
			if (client.second->is_connect == true && client.second->playerState != PLAYER_STATE_playing_game) {
				mPacketManager->send_room_info_pakcet(id, client.second->socket, mMapGameRoom[r_number]);
			}
		}

		cout << "make room success \n";
		auto copyRoom = mMapGameRoom;
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
	short room_number = mMapPlayerInfo[id]->room_number;
	if (room_number != -1) {
		for (short i = 0; i < MAX_ROOMPLAYER; ++i) { // �濡�ִ� id ��Ͽ��� id ����
			if (mMapGameRoom[room_number]->players_id[i] == id) {
				mMapGameRoom[room_number]->players_id[i] = -1;
			}
		}
		mMapPlayerInfo[id]->room_number = -1;
		mMapPlayerInfo[id]->playerState = PLAYER_STATE_in_room;
		// �ڽſ��� �泪���� ������Ŷ ����
		mPacketManager->send_leaveRoom_ok(id, mMapPlayerInfo[id]->socket);

		for (short i = 0; i < MAX_ROOMPLAYER; ++i) { // �濡 ���� �÷��̾�� �������� ���� ����
			int p_id = mMapGameRoom[room_number]->players_id[i];
			if (p_id != -1) {
				if (mMapPlayerInfo[p_id]->is_connect == true && mMapPlayerInfo[p_id]->playerState == PLAYER_STATE_in_room
					&& p_id != id) {
					mPacketManager->send_room_info_pakcet(p_id, mMapPlayerInfo[p_id]->socket, mMapGameRoom[room_number]);
				}
			}
		}


		bool roomEmpty = true;
		for (short i = 0; i < MAX_ROOMPLAYER; ++i) { // ���� ������� üũ
			int p_id = mMapGameRoom[room_number]->players_id[i];
			if (p_id != -1) {
				roomEmpty = false;
				break;
			}
		}
		if (roomEmpty == true) { // ���� ������� ����?
			mMapGameRoom[room_number]->enable = false;
			for (auto client : mMapPlayerInfo) {
				if (client.second->is_connect == true && client.second->playerState == PLAYER_STATE_in_lobby) {
					mPacketManager->send_room_info_pakcet(client.first, client.second->socket, mMapGameRoom[room_number]);
				}
			}

			//m_map_game_room.unsafe_erase(room_number);
		}

		// �κ�� ���� �÷��̾�� ��ü ������ ����
		for (auto room : mMapGameRoom) {
			mPacketManager->send_room_info_pakcet(id, mMapPlayerInfo[id]->socket, room.second);
		}

		for (auto room : mMapGameRoom) {
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
	//cout << "state change" << endl;
	cs_packet_client_state_change *packet = reinterpret_cast<cs_packet_client_state_change*>(buff);
	//packet->stage_number = 2;



	if (packet->change_state == PLAYER_STATE_playing_game) {	// ���� state ����
		mMapPlayerInfo[id]->hp = 200;
		mMapPlayerInfo[id]->gold = 500;
		short myroom_num = mMapPlayerInfo[id]->room_number;
		mPacketManager->send_game_start(id, mMapPlayerInfo[id]->socket, 1,
			mMapGameRoom[myroom_num]->wave_count, mMapGameRoom[myroom_num]->portalLife);

		if (mMapGameRoom[mMapPlayerInfo[id]->room_number]->room_state == R_STATE_in_room) {
			mMapPlayerInfo[id]->roomList_lock.lock();
			mMapGameRoom[mMapPlayerInfo[id]->room_number]->room_state = R_STATE_gameStart;
			mMapGameRoom[mMapPlayerInfo[id]->room_number]->stage_number = packet->stage_number;
			mMapPlayerInfo[id]->roomList_lock.unlock();
			process_game_start(mMapPlayerInfo[id]->room_number, packet->stage_number);
		}

		GAME_EVENT ev_playerStart{ id, chrono::high_resolution_clock::now() + 3s, PLAYER_GAME_START, 0 };
		add_event_to_queue(ev_playerStart);
		
		//m_map_player_info[id]->player_state = packet->change_state;

		for (short i = 0; i < MAX_ROOMPLAYER; ++i) {	// ���� ���� Ŭ���̾�Ʈ���� put player ��ȣ ����
			int other_id = mMapGameRoom[myroom_num]->players_id[i];
			if (other_id == -1) { continue; }
			if (mMapPlayerInfo[other_id]->is_connect == true &&
				/*m_map_player_info[other_id]->player_state == PLAYER_STATE_playing_game &&*/ other_id != id) {
				//cout << id << " put "<< other_id << endl;
				mPacketManager->send_put_player_packet(other_id, mMapPlayerInfo[other_id]->socket, id, 
					mMapPlayerInfo[id]->player_world_pos, mMapPlayerInfo[id]->animation_state);
				mPacketManager->send_put_player_packet(id, mMapPlayerInfo[id]->socket, other_id, 
					mMapPlayerInfo[other_id]->player_world_pos, mMapPlayerInfo[other_id]->animation_state);
			}
		}
	}
}

void Iocp_server::process_install_trap(const int& id, void * buff)
{
	cs_packet_install_trap *packet = reinterpret_cast<cs_packet_install_trap*>(buff);

	short room_num = mMapPlayerInfo[id]->room_number;

	/*short new_trapId = m_map_trapIdPool[room_num];
	auto &trapPool = m_map_trap[room_num];
	trapPool[new_trapId].set_4x4position(packet->trap_world_pos);
	trapPool[new_trapId].set_enable(true);
	trapPool[new_trapId].set_trap_type(packet->trap_type);
	m_map_trapIdPool[room_num] += 1;*/
	mMapPlayerInfo[id]->roomList_lock.lock();
	short new_trapId = m_map_trapIdPool[room_num];
	m_map_trapIdPool[room_num] += 1;
	mMapPlayerInfo[id]->roomList_lock.unlock();
	m_map_trap[room_num][new_trapId].set_trap_id(packet->trap_local_id);
	m_map_trap[room_num][new_trapId].set_4x4position(packet->trap_pos);
	m_map_trap[room_num][new_trapId].set_trap_type(packet->trap_type);
	if(packet->trap_type == TRAP_FIRE || packet->trap_type == TRAP_ARROW){
		check_trapDir(room_num, new_trapId, packet->trap_pos);
	}
	m_map_trap[room_num][new_trapId].set_enable(true);

	//cout << "trap install" << endl;
	mMapPlayerInfo[id]->gold -= TRAP_COST;
#ifdef TESTMODE
	if (mMapPlayerInfo[id]->gold < 0) {
		mMapPlayerInfo[id]->gold = 0;
	}
#endif
	mPacketManager->send_stat_change(id, mMapPlayerInfo[id]->socket, -1000, mMapPlayerInfo[id]->gold);

	// ��ġ�� Ʈ�� ���� ����
	for (short i = 0; i < MAX_ROOMPLAYER; ++i) {
		int other_id = mMapGameRoom[room_num]->players_id[i];
		if (other_id == -1) { continue; }
		if (mMapPlayerInfo[other_id]->is_connect == true &&
			mMapPlayerInfo[other_id]->playerState == PLAYER_STATE_playing_game /*&& other_id != id*/) {
			mPacketManager->send_trap_info_packet(other_id, mMapPlayerInfo[other_id]->socket, new_trapId, packet->trap_local_id, packet->trap_pos,
				packet->trap_type);
		}
	}
}

void Iocp_server::check_trapDir(const short & room_number, const short & trap_index, const XMFLOAT4X4 & _4x4pos)
{
	if (_4x4pos._11 == 0.0f && _4x4pos._12 == 1.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 0.0f && _4x4pos._33 == 1.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_MX);
		//cout << "MX" << endl;
	}
	else if (_4x4pos._11 == 0.0f && _4x4pos._12 == -1.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 0.0f && _4x4pos._33 == 1.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_PX);
		//cout << "PX" << endl;
	}
	else if (_4x4pos._11 == 1.0f && _4x4pos._12 == 0.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == 1.0f && _4x4pos._33 == 0.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_MZ);
		//cout << "MZ" << endl;
	}
	else if (_4x4pos._11 == 1.0f && _4x4pos._12 == 0.0f && _4x4pos._13 == 0.0f && _4x4pos._31 == 0.0f && _4x4pos._32 == -1.0f && _4x4pos._33 == 0.0f) {
		m_map_trap[room_number][trap_index].set_wallDir(WALL_TRAP_PZ);
		//cout << "PZ" << endl;
	}
}

void Iocp_server::process_player_shoot(const int & id, void * buff)
{
	cs_packet_shoot *packet = reinterpret_cast<cs_packet_shoot*>(buff);
	short player_room_number = mMapPlayerInfo[id]->room_number;
	if (player_room_number == -1) { return; }

	if (m_map_monsterPool[player_room_number][packet->monster_id].get_isLive() == true) {
		if (packet->headShot == true) {
			m_map_monsterPool[player_room_number][packet->monster_id].decrease_hp(PLAYER_ATT*2);
			m_map_monsterPool[player_room_number][packet->monster_id].set_animation_state(M_ANIM_DAMAGE);
			m_map_monsterPool[player_room_number][packet->monster_id].set_bulletAnim(true);
			GAME_EVENT ev_monsterBullet{ packet->monster_id, chrono::high_resolution_clock::now() + 500ms, EV_MONSTER_BULLET, player_room_number };
			add_event_to_queue(ev_monsterBullet);
		}
		else {
			m_map_monsterPool[player_room_number][packet->monster_id].decrease_hp(PLAYER_ATT);
			m_map_monsterPool[player_room_number][packet->monster_id].set_animation_state(M_ANIM_DAMAGE);
			m_map_monsterPool[player_room_number][packet->monster_id].set_bulletAnim(true);
			GAME_EVENT ev_monsterBullet{ packet->monster_id, chrono::high_resolution_clock::now() + 500ms, EV_MONSTER_BULLET, player_room_number };
			add_event_to_queue(ev_monsterBullet);
		}
	}
}

void Iocp_server::process_game_start(const short& room_number, const short& stage_number)
{
	mMapGameRoom[room_number]->portalLife = 20;
	mMapGameRoom[room_number]->wave_count = 0;

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

		mMapGameRoom[room_number]->gameRoom_lock.lock();
		m_map_monsterPool.insert(make_pair(room_number, monsterArr));
		m_map_trap.insert(make_pair(room_number, trapArr));
		m_map_trapIdPool.insert(make_pair(room_number, 0));
		mMapGameRoom[room_number]->gameRoom_lock.unlock();
	}

	/*for (auto m : m_map_monsterPool) {
		cout << "----------room number: " << m.first << endl;
		for (int i = 0; i < MAX_MONSTER; ++i) {
			cout << "monster id: " << m.second[i].get_monster_id() << endl;
		}
	}*/
	//
	GAME_EVENT g_ev{ room_number, chrono::high_resolution_clock::now() + 10s, EV_GEN_1stWAVE_MONSTER, stage_number };
	add_event_to_queue(g_ev);
	
}

void Iocp_server::process_game_end(const short & room_number, const bool& clearFlag)
{
	for (short i = 0; i < MAX_MONSTER; ++i) {
		m_map_monsterPool[room_number][i].set_isLive(false);
		m_map_monsterPool[room_number][i].set_monster_type(TYPE_DEFAULT);
		m_map_monsterPool[room_number][i].set_buffType(TRAP_BUFF_NONE);
		m_map_monsterPool[room_number][i].set_4x4position(default_wPos);
	}
	for (short i = 0; i < MAX_TRAP; ++i) {
		m_map_trap[room_number][i].set_enable(false);
		m_map_trap[room_number][i].set_4x4position(default_wPos);
		m_map_trap[room_number][i].set_trap_type(TRAP_NEEDLE);
		m_map_trap[room_number][i].set_wallDir(TRAP_DEFAULT);
		m_map_trap[room_number][i].set_wallTrapOn(false);
	}
	for (int p_id : mMapGameRoom[room_number]->players_id) {
		if (p_id != -1 && mMapPlayerInfo[p_id]->is_connect == true &&
			mMapPlayerInfo[p_id]->playerState == PLAYER_STATE_playing_game ) {
			mMapPlayerInfo[p_id]->playerState = PLAYER_STATE_in_room;
			mPacketManager->send_game_end(p_id, mMapPlayerInfo[p_id]->socket, clearFlag);
		}
	}
	mMapGameRoom[room_number]->room_state = R_STATE_in_room;
}


void Iocp_server::check_wave_end(const short& room_number)
{
	if (mMapGameRoom[room_number]->enable == false) { // �� ������Ȳ
		return;
	}


	mMapGameRoom[room_number]->gameRoom_lock.lock();
	short pLife = mMapGameRoom[room_number]->portalLife;
	send_protalLife_update(room_number);
	mMapGameRoom[room_number]->gameRoom_lock.unlock();
	if (pLife <= 0) { // ��Ż������ 0����
		cout << "plife: " << pLife << endl;
		mMapGameRoom[room_number]->wave_on = false;
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
	bool playerdead = false;
	for (short idx = 0; idx < MAX_ROOMPLAYER; ++idx) {
		int player_id = mMapGameRoom[room_number]->players_id[idx];
		if (player_id == -1) { continue; }
		if (mMapPlayerInfo[player_id]->hp < 0 && mMapPlayerInfo[player_id]->playerState == PLAYER_STATE_playing_game) {
			playerdead = true;
		}
	}
	if (playerdead == true) { // �÷��̾� ��� end
		process_game_end(room_number, false);
		cout << "room" << room_number << "fail \n";
		return;
	}


	if (end_flag == true) { // wave�� ����Ǹ�
		mMapGameRoom[room_number]->wave_on = false;
		if (mMapGameRoom[room_number]->wave_count == lastWAVE) { // ������ ���̺� ����� �������� ��Ŵ
			process_game_end(room_number, true);
			cout << "room" << room_number << "claer \n";
			return;
		}
		// ���̺� ī��Ʈ �ø���
		// ���� ���̺� ���� �� ��Ű��
		mMapGameRoom[room_number]->wave_count += 1;
		for (short p_idx = 0; p_idx < MAX_ROOMPLAYER; ++p_idx) {
			int temp_id = mMapGameRoom[room_number]->players_id[p_idx];
			if (temp_id != -1) {
				if (mMapPlayerInfo[temp_id]->is_connect == true &&
					mMapPlayerInfo[temp_id]->playerState == PLAYER_STATE_playing_game) {
					mMapPlayerInfo[temp_id]->gold += 200;
					mMapPlayerInfo[temp_id]->hp = 200;
					mPacketManager->send_stat_change(temp_id, mMapPlayerInfo[temp_id]->socket, mMapPlayerInfo[temp_id]->hp, mMapPlayerInfo[temp_id]->gold);
					mPacketManager->send_game_info_update(temp_id, mMapPlayerInfo[temp_id]->socket,
						mMapGameRoom[room_number]->wave_count, -1000);
					mPacketManager->send_wave_end(temp_id, mMapPlayerInfo[temp_id]->socket);
				}
			}
		}

		GAME_EVENT ev{ room_number, chrono::high_resolution_clock::now() + 5s, EV_GEN_MONSTER, 0 };
		add_event_to_queue(ev);
	}
	else if (end_flag == false) { // ����ȵ�
		// n���Ŀ� �ٽ� üũ�ϴ� �̺�Ʈ ����
		GAME_EVENT ev{ room_number, chrono::high_resolution_clock::now() + 3s, EV_CHECK_WAVE_END, 0 };
		add_event_to_queue(ev);
	}
}

void Iocp_server::add_monster_dead_event(const short & room_number, const short & monster_id)
{
	int room_num = room_number;
	int mon_id = monster_id;
	GAME_EVENT ev{ room_num, chrono::high_resolution_clock::now() + 1500ms, EV_MONSTER_DEAD, mon_id };
	add_event_to_queue(ev);
}

void Iocp_server::send_all_room_list(const int& id)
{
	for (auto room : mMapGameRoom) {
		if (room.second->enable == false) { continue; }
		mPacketManager->send_room_info_pakcet(id, mMapPlayerInfo[id]->socket,
			room.second);
	}
}

void Iocp_server::send_protalLife_update(const short & room_number)
{
	for (short i = 0; i < MAX_ROOMPLAYER; ++i) {
		int p_id = mMapGameRoom[room_number]->players_id[i];
		if (p_id == -1) { continue; }
		if (mMapPlayerInfo[p_id]->is_connect == true && mMapPlayerInfo[p_id]->playerState == PLAYER_STATE_playing_game) {
			mPacketManager->send_game_info_update(p_id, mMapPlayerInfo[p_id]->socket,
				-1000, mMapGameRoom[room_number]->portalLife);
		}
	}
}

void Iocp_server::get_player_db()
{
	for (auto d : mDatabaseManager->m_list_player_db) {
		PLAYER_DB db;
		db.DB_key_id = d.DB_key_id;
		strcpy_s(db.name, sizeof(d.name), d.name);
		db.level = d.level;

		m_list_player_db.emplace_back(db);
	}
}

void Iocp_server::process_disconnect_client(const int& leaver_id)
{
	mMapPlayerInfo[leaver_id]->roomList_lock.lock();
	short check_roomNum = -1; 
	if (mMapPlayerInfo[leaver_id]->room_number != -1) { // �÷��̾ �濡 ������ ���� ��
		check_roomNum = mMapPlayerInfo[leaver_id]->room_number;
		for (int i = 0; i < MAX_ROOMPLAYER; ++i) {
			if (mMapGameRoom[check_roomNum]->players_id[i] == leaver_id) { // leaver�� ���̵�� ������ -1�� ��ü
				mMapGameRoom[check_roomNum]->players_id[i] = -1;
				break;
			}
			
		}
	}
	mMapPlayerInfo[leaver_id]->roomList_lock.unlock();
	mMapPlayerInfo[leaver_id]->room_number = -1; // �泪����

	for (auto c : mMapPlayerInfo) {
		if (c.second->id == leaver_id) { continue; }
		if (c.second->is_connect == true) {
			// ��� �÷��̾�� ��������� Ŭ���̾�Ʈ�� ���̵� �����ش�
			mPacketManager->send_remove_player_packet(c.second->id, c.second->socket, leaver_id);
		}
	}
	if (check_roomNum != -1) { // ���� ������� �˻�
		bool roomEmpty = true;
		for (short i = 0; i < MAX_ROOMPLAYER; ++i) {
			if (mMapGameRoom[check_roomNum]->players_id[i] != -1) { // leaver�� ���̵�� ������ -1�� ��ü
				roomEmpty = false;
				break;
			}
		}
		if (roomEmpty == true) {
			mMapGameRoom[check_roomNum]->enable = false;
			mMapGameRoom[check_roomNum]->wave_on = false;
			for (auto client : mMapPlayerInfo) {
				if (client.second->is_connect == true && client.second->playerState == PLAYER_STATE_in_lobby) {
					mPacketManager->send_room_info_pakcet(client.first, client.second->socket, mMapGameRoom[check_roomNum]);
				}
			}
		}
	}
}

void Iocp_server::process_nameLogin(const int & id, void * buff)
{
	cs_packet_namelogin *login_packet = reinterpret_cast<cs_packet_namelogin*>(buff);
	int ret = mDatabaseManager->check_nameLogin(login_packet->name);
	if (ret > 0) {
		cout << "login success" << endl;
		mMapPlayerInfo[id]->DB_key = ret;
		mPacketManager->send_nameLogin_result(id, mMapPlayerInfo[id]->socket, NAMELOGIN_SUC);
	}
	else {
		cout << "login fail" << endl;
	}
	
}

void Iocp_server::check_monster_attack(const short & room_number, const short & monster_id)
{
	int target_id = m_map_monsterPool[room_number][monster_id].get_target_id();
	if (target_id == -1) { return; }
	if (mMapPlayerInfo[target_id]->damageCooltime == true) { return; }

	if (target_id != -1) { //Ÿ���� �����Ҷ�
		if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_ORC) {
			if (Vector3::Distance(mMapPlayerInfo[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < ORC_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (mMapPlayerInfo[target_id]->damageCooltime == false) {
					//cout << "���ݼ���\n";
					mMapPlayerInfo[target_id]->hp -= ORC_ATT;
#ifdef TESTMODE
					if (mMapPlayerInfo[target_id]->hp < 10) {
						mMapPlayerInfo[target_id]->hp = 10;
					}
#endif
					mPacketManager->send_stat_change(target_id, mMapPlayerInfo[target_id]->socket, mMapPlayerInfo[target_id]->hp, -1000);
				}

				mMapPlayerInfo[target_id]->damageCooltime = true;
				GAME_EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_SHAMAN) {

		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_STRONGORC) {
			if (Vector3::Distance(mMapPlayerInfo[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < STRONGORC_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (mMapPlayerInfo[target_id]->damageCooltime == false) {
					//cout << "���ݼ���\n";
					mMapPlayerInfo[target_id]->hp -= STRONGORC_ATT;
#ifdef TESTMODE
					if (mMapPlayerInfo[target_id]->hp < 10) {
						mMapPlayerInfo[target_id]->hp = 10;
					}
#endif
					mPacketManager->send_stat_change(target_id, mMapPlayerInfo[target_id]->socket, mMapPlayerInfo[target_id]->hp, -1000);
				}

				mMapPlayerInfo[target_id]->damageCooltime = true;
				GAME_EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
		else if (m_map_monsterPool[room_number][monster_id].get_monster_type() == TYPE_RIDER) {
			if (Vector3::Distance(mMapPlayerInfo[target_id]->get_pos(),
				m_map_monsterPool[room_number][monster_id].get_position()) < RIDER_ATT_RANGE) {
				// ����
				// hp�����ϰ� ��Ŷ����
				if (mMapPlayerInfo[target_id]->damageCooltime == false) {
					//cout << "���ݼ���\n";
					mMapPlayerInfo[target_id]->hp -= RIDER_ATT;
#ifdef TESTMODE
					if (mMapPlayerInfo[target_id]->hp < 10) {
						mMapPlayerInfo[target_id]->hp = 10;
					}
#endif
					mPacketManager->send_stat_change(target_id, mMapPlayerInfo[target_id]->socket, mMapPlayerInfo[target_id]->hp, -1000);
				}

				mMapPlayerInfo[target_id]->damageCooltime = true;
				GAME_EVENT ev{ target_id, chrono::high_resolution_clock::now() + 2s, EV_PLAYER_DAMAGE_COOLTIME, 0 };
				add_event_to_queue(ev);
			}
		}
	}
	m_map_monsterPool[room_number][monster_id].set_attackCooltime(false);
}


void Iocp_server::process_packet(const int& id, void * buff)
{
	char *packet = reinterpret_cast<char *>(buff);

	short x = mMapPlayerInfo[id]->x;
	short y = mMapPlayerInfo[id]->y;
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
	if (mMapGameRoom[room_number]->wave_count == 0) {
		mMapGameRoom[room_number]->wave_count = wave + 1;
	}
	wave = mMapGameRoom[room_number]->wave_count;
	//m_map_game_room[room_number]->wave_count = 1;
	if (mMapGameRoom[room_number]->stage_number == 1) { // stage 1
		XMFLOAT3 line1 = stage1_line1_start;
		XMFLOAT3 line4 = stage1_line4_start;
		switch (wave)
		{
		case 1:		// wave1
		{
			for (int i = 0; i < 30; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 15) { // line 123
					if (i >= 14) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 30) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 36) { // line 456
					if (i >= 33) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 36) { // line 456
					if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 36 && i < 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 36 && i < 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 39) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 39 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 39 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 51) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 42 && i < 48) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 48) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
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
					m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 456
					if (i >= 38 && i < 46) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 46) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		default:
			break;
		}
	}
	else if (mMapGameRoom[room_number]->stage_number == 2) { // stage2
		XMFLOAT3 line1 = stage2_line1_start;
		XMFLOAT3 line4 = stage2_line4_start;
		XMFLOAT3 line7 = stage2_line7_start;
		XMFLOAT3 line10 = stage2_line10_start;
		switch (wave) {
		case 1: {
			short waveMax = 28;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 7) { // line 123
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 2: {
			short waveMax = 28;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 7) { // line 123
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 3: {
			short waveMax = 36;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 9) { // line 123
					if (i >= 7) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 18) { // line 456
					if (i >= 16) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 27) { // line 789
					if (i >= 25) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 34) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 4: {
			short waveMax = 36;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 9) { // line 123
					if (i >= 6) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 18) { // line 456
					if (i >= 15) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 27) { // line 789
					if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 33) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 5: {
			short waveMax = 44;
			for (int i = 0; i < waveMax; ++i) {
				m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
				if (i < 11) { // line 123
					if (i >= 8) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 22) { // line 456
					if (i >= 19) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 33) { // line 789
					if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 41) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 6: {
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
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 26) { // line 456
					if (i >= 21 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 39) { // line 789
					if (i >= 34 && i < 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 47 && i < 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 7: {
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
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 30) { // line 456
					if (i >= 23 && i < 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 45) { // line 789
					if (i >= 38 && i < 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 53 && i < 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 8: {
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
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 32) { // line 456
					if (i >= 24 && i < 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 48) { // line 789
					if (i >= 40 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 56 && i < 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 9: {
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
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 34) { // line 456
					if (i >= 25 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 51) { // line 789
					if (i >= 42 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 59 && i < 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		case 10: {
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
					m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
						(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 34) { // line 456
					if (i >= 24 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
						(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < 51) { // line 789
					if (i >= 41 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
						(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 58 && i < 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
						(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
				}
			}
			break;
		}
		default:
			break;
		}
	} //-----------------------------------------------------------------------------------------------------------------
	else if (mMapGameRoom[room_number]->stage_number == 3) { // stage3
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 14) { // line 456
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 21) { // line 789
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 18) { // line 456
					if (i >= 16) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 27) { // line 789
					if (i >= 25) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 34) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 18) { // line 456
					if (i >= 15) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 27) { // line 789
					if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 33) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 22) { // line 456
					if (i >= 19) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 33) { // line 789
					if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 41) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 26) { // line 456
					if (i >= 19 && i < 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 24) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 39) { // line 789
					if (i >= 34 && i < 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 37) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 47 && i < 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 50) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 30) { // line 456
					if (i >= 23 && i < 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 28) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 45) { // line 789
					if (i >= 38 && i < 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 43) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 53 && i < 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 58) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 32) { // line 456
					if (i >= 24 && i < 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 29) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 48) { // line 789
					if (i >= 40 && i < 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 45) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 56 && i < 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 61) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 34) { // line 456
					if (i >= 25 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 51) { // line 789
					if (i >= 42 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 59 && i < 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
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
					m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
						(line1.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 34) { // line 456
					if (i >= 24 && i < 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 30) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
						(line4.z + (float)stage3_start1_z(dre))));
				}
				else if (i < 51) { // line 789
					if (i >= 41 && i < 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 47) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
						(line9.z + (float)stage3_start1_z(dre))));
				}
				else if (i < waveMax) { // line 10 11 12
					if (i >= 58 && i < 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
					}
					else if (i >= 64) {
						m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
					}
					m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
						(line12.z + (float)stage3_start1_z(dre))));
				}
			}
			break;
		}
		default:
			break;
		}
	}

	cout<<"room:" << room_number<<" stage:"<< stage_number<<" wave:"<< wave <<" gen complete" << endl;;
	GAME_EVENT ev{ room_number, chrono::high_resolution_clock::now() + 1s, EV_MONSTER_THREAD_RUN, 0 };
	add_event_to_queue(ev);
	mMapGameRoom[room_number]->wave_on = true;

	GAME_EVENT ev_waveCheck{ room_number, chrono::high_resolution_clock::now() + 5s, EV_CHECK_WAVE_END, 0 };
	add_event_to_queue(ev_waveCheck);
}

#pragma once
#include "stdafx.h"

#define WINDOW_X	600
#define WINDOW_Y	600

#define SERVER_IP "127.0.0.1"


struct PLAYER_INFO 
{
	int id;
	short x, y;
};

struct OVER_EX {
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char net_buf[MAX_BUFFER];
	//EVENT_TYPE event_type;
};
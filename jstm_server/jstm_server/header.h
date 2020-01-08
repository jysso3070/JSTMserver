#pragma once

#pragma comment(lib, "Ws2_32.lib") 
#include <WS2tcpip.h> 

#include <iostream>

#include <thread>
#include <mutex>

#include <vector>
#include <set>
#include <map>
#include <concurrent_unordered_map.h>

// mssql
#define UNICODE
#include <sqlext.h>  

using namespace std;

// 프로토콜
#include "protocol.h"

// 구조체
#include "struct.h"

#include "iocp_server.h"
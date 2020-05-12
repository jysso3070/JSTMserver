#pragma once

// server socket
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h> 
#include <MSWSock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <iostream>

// mutex thread
#include <thread>
#include <mutex>

// stl
#include <chrono>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <concurrent_unordered_map.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 3D Direct
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// mssql
//#define UNICODE
#include <sqlext.h>  

using namespace std;


// 프로토콜
#include "protocol.h"

// 구조체
//#include "struct.h"

//#include "iocp_server.h"

#include "server_manager.h"



server_manager::server_manager()
{
}


server_manager::~server_manager()
{
}

void server_manager::get_server_ipAddress()
{
	// Winsock Start - winsock.dll 로드
	WSADATA	WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cout << "Error - Can not load 'winsock.dll file\n'";
		return;
	}

	PHOSTENT	hostinfo;
	char				hostname[50];
	char				ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int err_no = gethostname(hostname, sizeof(hostname));
	if (err_no == 0) {
		hostinfo = gethostbyname(hostname);
		strcpy_s(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostinfo->h_addr_list[0])));
	}
	WSACleanup();
	cout << "Server IP Address: \t" << ipaddr << endl;
}

void server_manager::get_cpu_count()
{
	// CPU , Thread 개수 확인
	SYSTEM_INFO	si; // CPU 개수 확인용
	GetSystemInfo(&si); // 시스템 정보를 받아온다.
	int CpuCore = static_cast<int>(si.dwNumberOfProcessors);
	int NumWorkerThread = static_cast<int>(CpuCore * 2 - 2);
	cout << "CPU Core Count: " << CpuCore << "\tThread: " << NumWorkerThread << endl;
}

void server_manager::socket_error_display(const char * msg, int err_no)
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

void server_manager::socket_error_quit(const char * msg, int err_no)
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

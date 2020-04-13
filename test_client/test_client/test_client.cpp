// test_client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "test_client.h"
#include "globals.h"


#define MAX_LOADSTRING 100

// 비동기io
#define	WM_SOCKET			WM_USER + 1

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


WSABUF recv_buf; // WSARecv를 사용하기위해 WSABUF 사용
char buffer[MAX_BUFFER];
PLAYER_INFO my_info;
network_manager net_manager; // 전역클래스
SOCKET serverSocket;
HWND hWnd;

void PacketProccess(void * buf);
void connect_to_server();

void ReadBuffer(SOCKET sock)
{
	int in_packet_size = 0;
	int saved_packet_size = 0;

	DWORD iobyte, ioflag = 0;
	WSARecv(sock, &recv_buf, 1, &iobyte, &ioflag, NULL, NULL);

	char * temp = reinterpret_cast<char*>(buffer);

	while (iobyte != 0)
	{
		if (in_packet_size == 0)
		{
			in_packet_size = temp[0];
		}
		if (iobyte + saved_packet_size >= in_packet_size)
		{
			memcpy(buffer + saved_packet_size, temp, in_packet_size - saved_packet_size);
			PacketProccess(buffer);
			temp += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{
			memcpy(buffer + saved_packet_size, temp, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}


void PacketProccess(void * buf) {
	char* temp = reinterpret_cast<char*>(buf);

	switch (temp[1]) {
	case SC_SEND_ID:
	{
		sc_packet_send_id *send_id_packet = reinterpret_cast<sc_packet_send_id*>(buf);
		my_info.id = send_id_packet->id;
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *pos_packet = reinterpret_cast<sc_packet_pos*>(buf);
		my_info.x = pos_packet->x;
		my_info.y = pos_packet->y;
		break;
	}
	}
}

void connect_to_server() {
	net_manager.init_socket();
	
	serverSocket = net_manager.rq_connect_server("127.0.0.1");

	WSAAsyncSelect(serverSocket, hWnd, WM_SOCKET, FD_READ || FD_CLOSE);
}

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTCLIENT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINDOW_X, WINDOW_Y, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;

	//	dubble buffer
	HDC hdc, memdc;
	HBITMAP BackBit, oldBackBit;
	PAINTSTRUCT ps;
	static RECT rectView;

	// server client
	//static WSADATA WSAData;
	//static SOCKET serverSocket;
	//static sockaddr_in serverAddr;

    switch (message)
    {
	case WM_CREATE:
		{
		// dubble buffer
		GetClientRect(hWnd, &rectView);

		// Socket init
		//WSAStartup(MAKEWORD(2, 0), &WSAData);	//  네트워크 기능을 사용하기 위함, 인터넷 표준을 사용하기 위해
		//serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
		//memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
		//serverAddr.sin_family = AF_INET;
		//serverAddr.sin_port = htons(SERVER_PORT);
		//inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);// ipv4에서 ipv6로 변환
		//connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));


		// WSAAsyncSelect
		//WSAAsyncSelect(serverSocket, hWnd, WM_SOCKET, FD_READ || FD_CLOSE);

		// WSABUF 주소에 데이터받아올 버퍼주소 할당
		recv_buf.len = MAX_BUFFER;
		recv_buf.buf = buffer;




		my_info.x = 300;
		my_info.y = 300;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
		// dubble buffer
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		BackBit = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
		oldBackBit = (HBITMAP)SelectObject(memdc, BackBit);


		Rectangle(memdc, my_info.x-10, my_info.y-10, my_info.x+10, my_info.y+10);


		// 백버퍼 불러오기
		BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);

		DeleteObject(SelectObject(memdc, oldBackBit));
		DeleteDC(memdc);

		EndPaint(hWnd, &ps);

    }
	break;
	case WM_KEYFIRST:
		if (wParam == VK_UP) {
			cs_packet_up packet;
			packet.size = sizeof(packet);
			packet.type = CS_UP;
			send(serverSocket, (char*)&packet, sizeof(packet), 0);
		}
		if (wParam == VK_DOWN) {
			cs_packet_up packet;
			packet.size = sizeof(packet);
			packet.type = CS_DOWN;
			send(serverSocket, (char*)&packet, sizeof(packet), 0);
		}
		if (wParam == VK_LEFT) {
			cs_packet_up packet;
			packet.size = sizeof(packet);
			packet.type = CS_LEFT;
			send(serverSocket, (char*)&packet, sizeof(packet), 0);
		}
		if (wParam == VK_RIGHT) {
			cs_packet_up packet;
			packet.size = sizeof(packet);
			packet.type = CS_RIGHT;
			send(serverSocket, (char*)&packet, sizeof(packet), 0);
		}
		if (wParam == VK_RETURN) {
			connect_to_server();
		}
		if (wParam == VK_SPACE) {
			cs_packet_make_room packet;
			packet.id = my_info.id;
			packet.type = CS_MAKE_ROOM;
			packet.size = sizeof(packet);
			send(serverSocket, (char*)&packet, sizeof(packet), 0);
		}

		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			break;
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			ReadBuffer((SOCKET)wParam);

			break;
		}
		InvalidateRgn(hWnd, NULL, FALSE);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

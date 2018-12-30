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
#include <commctrl.h>
#include <string.h>
#include <stdlib.h>
#include <winsock.h>
#include "c:\lcc\project\tcp\tcpres.h"

#define WS_VERSION_REQD	0x0101
#define WM_ASYNC WM_USER+1


WSADATA stWSAData;
int nRet;
HWND hwndDlg;
SOCKET hSock = INVALID_SOCKET;
char TxtBuf[256];
SOCKADDR_IN rmaddr;
int nAddrSize = sizeof(SOCKADDR);
WORD WSAEvent;

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "tcp";
	RegisterClass(&wc);
	WSAStartup(WS_VERSION_REQD, &stWSAData);
    hSock = socket (AF_INET, SOCK_STREAM, 0);
	return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
}


static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg) {

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDB_SEND:
			SendDlgItemMessage(hwndDlg,IDE_RUN,WM_GETTEXT,(WPARAM)256,(LPARAM)TxtBuf);
			send(hSock,TxtBuf,256,0);
			return 1;

			case IDB_CON:
				rmaddr.sin_family = AF_INET;
				SendDlgItemMessage(hwndDlg,IDE_IP,WM_GETTEXT,(WPARAM)256,(LPARAM)TxtBuf);
				rmaddr.sin_addr.s_addr = inet_addr(TxtBuf);
				rmaddr.sin_port = htons (2049);
				connect (hSock, (struct sockaddr *)&rmaddr,sizeof(rmaddr));

				SendDlgItemMessage(hwndDlg,IDE_MSG,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)"Try connecting with ");
				SendDlgItemMessage(hwndDlg,IDE_MSG,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)TxtBuf);
				SendDlgItemMessage(hwndDlg,IDE_MSG,WM_CHAR,(WPARAM)13,(LPARAM)28);

				nRet=WSAAsyncSelect (hSock,hwndDlg,WM_ASYNC,FD_READ);

			return 1;

		}
		return TRUE;

	case WM_CLOSE:
		nRet = WSACleanup();
		EndDialog(hwndDlg,0);
		return TRUE;

	case WM_ASYNC:
		WSAEvent = WSAGETSELECTEVENT (lParam);
		switch (WSAEvent)
		{
			case FD_READ:

				nRet=recv(hSock,TxtBuf,256,0);
				SendDlgItemMessage(hwndDlg,IDE_MSG,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)TxtBuf);
				SendDlgItemMessage(hwndDlg,IDE_MSG,WM_CHAR,(WPARAM)13,(LPARAM)28);
		}
		return TRUE;
	}
	return FALSE;
}

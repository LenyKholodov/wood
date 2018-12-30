// DF_onebuf.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "new.h"
#include "DSPlayer.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];                    // the main window class name
HWND hWnd;

// Forward declarations of functions included in this code module:
ATOM                            MyRegisterClass(HINSTANCE hInstance);
BOOL                            InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        About(HWND, UINT, WPARAM, LPARAM);

CDSPlayer *Player;
CSoundSource *src;
InitProc();
PlayProc();
DownProc();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
		K_FastInit(NULL);
        // TODO: Place code here.
        MSG msg;
        HACCEL hAccelTable;

        // Initialize global strings
        LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadString(hInstance, IDC_DF_ONEBUF, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance);

        // Perform application initialization:
        if (!InitInstance (hInstance, nCmdShow)) 
        {
                return FALSE;
        }

        hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DF_ONEBUF);

        // Main message loop:
        while (GetMessage(&msg, NULL, 0, 0)) 
        {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
                {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                }
        }

        return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX); 

        wcex.style                      = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc        = (WNDPROC)WndProc;
        wcex.cbClsExtra         = 0;
        wcex.cbWndExtra         = 0;
        wcex.hInstance          = hInstance;
        wcex.hIcon                      = LoadIcon(hInstance, (LPCTSTR)IDI_DF_ONEBUF);
        wcex.hCursor            = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground      = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName       = (LPCTSTR)IDC_DF_ONEBUF;
        wcex.lpszClassName      = szWindowClass;
        wcex.hIconSm            = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

        return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        int wmId, wmEvent;
        PAINTSTRUCT ps;
        HDC hdc;

        switch (message) 
        {
        case WM_COMMAND:
                wmId    = LOWORD(wParam); 
                wmEvent = HIWORD(wParam); 
                // Parse the menu selections:
                switch (wmId)
                {
                case IDM_EXIT:
                        DestroyWindow(hWnd);
                        break;
                case ID_DS_INIT:
                        InitProc();
                        break;
                case ID_DS_PLAY:
                        PlayProc();
                        break;
                case ID_DS_DOWN:
                        DownProc();
                        break;
                default:
                        return DefWindowProc(hWnd, message, wParam, lParam);
                }
                break;
        case WM_PAINT:
                hdc = BeginPaint(hWnd, &ps);
                // TODO: Add any drawing code here...
                EndPaint(hWnd, &ps);
                break;
        case WM_DESTROY:
                PostQuitMessage(0);
                break;
        default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
}

InitProc()
{        				
        Player=new CDSPlayer(hWnd,"pak1.pak","restab.txt");
//        src=Player->CreateSource("footsteps.wav");
}

PlayProc()
{
	float pos[3]={0,0,0};
//	for(int i=0;i<2;i++)
//	{
		src=Player->CreateSource("forest3.wav");
//		pos[2]=i;
		src->Seek(0);
		src->SetPos(pos);
        src->Play();
//	}
}

DownProc()
{
        delete Player;
}
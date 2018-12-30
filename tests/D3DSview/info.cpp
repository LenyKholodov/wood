
// D3DSView

// This file was originally part of the DX7 SDK
// and was altered by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 7.0 SDK
// Date : 16th May 2000
// File : info.cpp

// Website: http://members.aol.com/billybop7
// Email:  billybop7@aol.com 

#include "ddraw.h"
#include "d3dapp.h"
#include "d3dtextr.h"
#include "d3dres.h"
#include "info.hpp"
#include "Debug.hpp"


#define ABOUT_DIALOG_TIMER 1

extern CD3DApplication*	pD3DApp;


void DisplayCredits(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 160, sy = 70;
	int nIndex;
	char  buffer[255];
	HBRUSH holdbrush_color;
	

	PrintMessage("DisplayCredits", NULL);
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,400,260);

	SetBkMode(hdc, TRANSPARENT);
			
	
	strcpy(buffer, "This Direct3D Immediate mode sample");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "was developed with the DirectX 7.0a SDK");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "and Visual C++ 6.0");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "Bill's DirectX Page");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "http://members.aol.com/billybop7");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Email:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "billybop7@aol.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}


void DisplayStats(HWND hwnd)
{
	HDC hdc;
	int mem;	
	int sx1 = 15, sx2 = 320, sy = 70;
	int nIndex;
	char  buffer[255];
	char  buf2[255];
	DDSCAPS2    ddsCaps2; 
	DWORD       total_memory_for_textures; 
	DWORD       free_memory_for_textures;
	DWORD		total_video_memory;
	DWORD		free_video_memory;
	HRESULT     hr; 
	MEMORYSTATUS memStatus;
	HBRUSH holdbrush_color;
	LPDIRECTDRAW7 pDD;
  
  
    
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;

	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,400,260);

	SetBkMode(hdc, TRANSPARENT);
	memStatus.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memStatus);
		
	mem = (int)memStatus.dwMemoryLoad; 
	strcpy(buf2, "System memory in use :");
	strcpy(buffer, ITOA(mem));
	strcat(buffer, " %");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));  
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	mem=(int)memStatus.dwTotalPhys;
	mem = mem /1024;
	itoa(mem, buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total system memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));  
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;
		
	// Create a Direct Draw object for temporary use, so we can call 
	// the GetAvailableVidMem function
	// The first parameter is set to NULL to indicate we want to use
	// the guid of the existing driver.
    hr = DirectDrawCreateEx( NULL, (VOID**)&pDD, IID_IDirectDraw7, NULL );
    if( FAILED(hr) )
    {
        PrintMessage("DisplayStats - DirectDrawCreateEx() - FAILED", NULL);
        return;
    }
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
	hr = pDD->GetAvailableVidMem(&ddsCaps2, &total_memory_for_textures, &free_memory_for_textures); 
	if (FAILED(hr))
	{
		PrintMessage("DisplayRRStats - GetAvailableVidMem DDSCAPS_TEXTURE Failed", NULL);
		return; 
	}
 
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY  ; 
	hr = pDD->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory); 
	if (FAILED(hr))
	{
		PrintMessage("DisplayRRStats - GetAvailableVidMem DDSCAPS_VIDEOMEMORY Failed", NULL);
		return; 
	}

	pDD->Release();

	mem= total_video_memory;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total video memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer)); 
	sy +=20;

	mem= free_video_memory;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Free video memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer)); 
	sy +=20;

	mem= free_memory_for_textures;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Free video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	mem= total_memory_for_textures;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  
}



BOOL CALLBACK AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			SetTimer(hwnd, ABOUT_DIALOG_TIMER, 100, NULL);
			break;

		case WM_LBUTTONDOWN:
			break;
			
		case WM_RBUTTONDOWN:
			break;

		case WM_TIMER:
			KillTimer(hwnd, ABOUT_DIALOG_TIMER);
			DisplayCredits(hwnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDOK:
				case IDCANCEL:      
					EndDialog(hwnd, TRUE);
					break;

				case IDC_BUTTON1:
					DisplayCredits(hwnd);
					break;
				
				case IDC_BUTTON2:
					DisplayStats(hwnd);
					break;
			break;
			}
	break;
		
	}

	return FALSE;
}

 

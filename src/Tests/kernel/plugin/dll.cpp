// dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <fstream.h>

const fstream      logout ("logo",ios::out);               
ostream            dout  = logout.rdbuf();

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                                         )
{
        dout<<"fuck!"<<endl;
    return TRUE;
}

class   Thread;

DWORD PluginInit (Thread*,DWORD)
{
  dout<<"Hello wordl!"<<endl;
        return 0;
}

void    cucu (int x,int y)
{
  dout<<"X: "<<x<<" Y: "<<y<<endl;
}

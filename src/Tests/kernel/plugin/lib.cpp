#define DEBUG
#include <windows.h>
#include <pool.h>

#define DllAccess __declspec( dllexport )

class    Thread;

DllAccess DWORD    PluginInit (Thread*,DWORD)
{
  cout<<"Hello world!"<<endl;
  return 0;
}

/*BOOL WINAPI DLLMain( HINSTANCE hInstDLL, DWORD dwNotification, LPVOID lpReserved )
{
    switch(dwNotification)
    {
        case DLL_PROCESS_ATTACH :
               // DLL initialization code goes here. Formerly this 
               // would be in the LibMain function of a 16-bit DLL.
               //..................................................
//               nProcessCount++;
               return( TRUE );

        case DLL_PROCESS_DETACH :
               // DLL cleanup code goes here. Formerly this would
               // be in the WEP function of a 16-bit DLL.
               //................................................
//               nProcessCount--;
               break;                      

        case DLL_THREAD_ATTACH :
               // Special initialization code for new threads goes here.
               // This is so the DLL can "Thread Protect" itself.
               //.......................................................
//               nThreadCount++;
               break;

        case DLL_THREAD_DETACH :
               // Special cleanup code for threads goes here.
               //............................................
//               nThreadCount--;
               break;
    }

    return( FALSE );
}
  */
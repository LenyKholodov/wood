#include <windows.h>
#include <iostream.h>
#include <conio.h>

void     main ()
{
  MEMORYSTATUS  status;

  status.dwLength = sizeof (status);
  
  GlobalMemoryStatus (&status);

  cout<<SetProcessWorkingSetSize (GetCurrentProcess (),128*1024*1024,300*1024*1024)<<endl;

  void* mem = new char [256*1024*1024];

  memset (mem,0,256*1024*1024);

  cout<<"end fill"<<endl;

  getch ();  
}
#define  DEBUG
#include <thread\app.h>
#include <thread\smpwindow.h>
#include <thread\conwindow.h>
#include <conio.h>

int      start (Application*,char*);
int      halt  (Window*,UINT,WORD,DWORD,DWORD _UserParam);
void     idle  (Application*);

A_Start (start,1000000);

int      start (Application* app,char* cmd)
{
  Window* wnd = new (app->pool()) 
                SimpleWindow (app->pool(),app->instance(),rect_t(50,50,400,400),"Console");
  ConsoleWindow*        con = new (app->pool()) 
                ConsoleWindow (wnd,rect_t (20,20,340,340),"");

  cout                      = *con;

  wnd->InsertHandler    (WM_DESTROY,halt);
  app->SetIdle          (idle);

  int state = app->MainLoop ();

  return 0;
}

void     idle  (Application*)
{
  cout<<"Test console"<<endl;
}

int      halt  (Window*,UINT,WORD,DWORD,DWORD _UserParam)
{
  PostQuitMessage (0);

  return 0;
}

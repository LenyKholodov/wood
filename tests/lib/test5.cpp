#define  DEBUG
#include <thread\app.h>
#include <thread\timer.h>
#include <thread\smpwindow.h>
#include <conio.h>

void     timer (DWORD,clock_t);
int      start (Application*,char*);
void     idle  (Application*);
int      halt  (Window*,UINT,WORD,DWORD,DWORD _UserParam);

A_Start (start,1000000);

int      start (Application* app,char* cmd)
{
  Window* wnd = new (app->pool()) 
                SimpleWindow (app->pool(),app->instance(),rect_t(0,0,400,200),"Very good!");
  Timer   timer (CLK_TCK,timer,13,wnd);

  wnd->InsertHandler    (WM_DESTROY,halt);
  app->SetIdle          (idle);

  int state = app->MainLoop ();

  return 0;
}

void     idle  (Application*)
{
}

int      halt  (Window*,UINT,WORD,DWORD,DWORD _UserParam)
{
  PostQuitMessage (0);

  return 0;
}

void     timer (DWORD value,clock_t)
{
  dout<<"Call timer: "<<value<<endl;
}
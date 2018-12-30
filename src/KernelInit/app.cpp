#include <pch.h>
#include <window.h>
#include <kernel\app.h>
#include <default.h>
#include <debug.h>

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"kernel32.lib")
#pragma comment (lib,"shell32.lib")
#pragma comment (lib,"gdi32.lib")

Application* appPtr = NULL;

Application*    A_GetApplication ()
{
  return appPtr;
}

HINSTANCE       A_GetInstance ()
{
  return appPtr->instance ();
}

///////////////////WinMain//////////////////////////////////////////////////////

int  MainFunc (int argc,char* argv [])
{
  D_Try
  {
    return main (argc,argv);
  }
  D_ExcRet ("WinMain: Error at execute start function",EXIT_FAILURE);
}

int  WINAPI WinMain (HINSTANCE _Instance,HINSTANCE _Fuck,LPSTR _Cmd,int _Show)
{
  char** argv = NULL;
  int    argc = 0;

  Application::ProcessCmd (GetCommandLine (),argc,argv);

  K_FastInit (NULL);

  Application        app;
  int                result = 0;

  appPtr                    = &app;

  app.mInst    =  _Instance;
  app.mCmd     =  GetCommandLine ();
  app.mShow    =  _Show;      
  app.mArgv    = argv;
  app.mArgc    = argc;

  result       = MainFunc (app.mArgc,app.mArgv);

  return result;
}

///////////////////implementation of class Application//////////////////////////

void  Application::ProcessCmd (const char* cmd,int& argc,char**& argv)
{
  D_Try
  {
    const char* first = cmd;
    argc = 0;

    while (*first)
    {
      for (;*first && *first == ' ';first++);
      for (;*first && *first != ' ';first++);
      argc++;
    }

    argv = (char**)malloc (sizeof (char*) * argc); 

    if (!argv)
    {
      exit (1);
      return;
    }

    first       = cmd;
    const char* last  = first;
    
    for (int i=0;*first;i++)
    {
      for (first = last;*first && *first == ' ';first++);
      for (last  = first;*last && *last != ' ';last++);
      
      if  (!*first) break;
      
      argv [i] = (char*)malloc (sizeof (char) * (ulong(last)-ulong(first)+1));

      memcpy (argv [i],first,ulong(last)-ulong(first));
      argv [i][ulong(last)-ulong(first)] = 0;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    argc = 0;
    argv = NULL;
  }
}

Application::Application  ()
          : MemObject (::M_GetPool()),
            mCmd      (NULL),
            mArgv     (NULL),
            mArgc     (0)
{   
  Window::mWindDict  = ext_new Dict<Window*> (GetPool ());
}

Application::~Application    ()
{
  if (Window::mWindDict) delete Window::mWindDict;

  Window::mWindDict = NULL;

  if (mArgv)
  {
    for (int i=0;i<mArgc;i++)
      if (mArgv [i])
      {
        ::free (mArgv [i]);
        mArgv [i] = NULL;
      }  
    ::free (mArgv);
  }  
}

//////////////////////MainLoop//////////////////////////////////////////////////

int     MainLoop      (IdleFn fn)
{
  D_Try
  {
    MSG          msg;
    BOOL         loop = TRUE;
    
    while (loop)
    {      
      if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
        while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
        {
          if (GetMessage (&msg,NULL,0,0))
          { 
            TranslateMessage (&msg);
            DispatchMessage  (&msg);              
          }
          else
            loop = FALSE;
        }
      else if (!fn)
        WaitMessage ();

      if (fn)
      {      
        D_Try
        {    
          (*fn)();
        }
        __except (dbgService->Dump (_exception_info ()))        
        {
          D_Message ("Application: Error at Idle (): "<<fn);
          fn = NULL;                                    
        }                                    
      }
    }  
         
    return msg.wParam;
  }
  D_TestExcRet ("Application: Error at MainLoop ()",1);
/*
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Application: Error at MainLoop () "<<endl;
    #endif
    D_Try
    {
    }
    D_Exception (1)
    {
    }
    return 1;
  }*/
}

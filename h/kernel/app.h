#ifndef __KERNEL__APPLICATION__
#define __KERNEL__APPLICATION__

#include <kernel.h>

#pragma comment (linker,"/subsystem:windows")

#ifndef WOOD_LIB_COMPILE
  #ifdef DEBUG
    #pragma comment (lib,"winappd.lib")
    #pragma comment (lib,"kerneld.lib")
  #else
    #pragma comment (lib,"winapp.lib")
    #pragma comment (lib,"kernel.lib")
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////
///Прототип главной функции
////////////////////////////////////////////////////////////////////////////////

int     main (int,char*[]);

////////////////////////////////////////////////////////////////////////////////
///Функция обработки цикла сообщений с вызовом приоритетной функции
////////////////////////////////////////////////////////////////////////////////

typedef void (*IdleFn)();

int     MainLoop      (IdleFn = NULL);

////////////////////////////////////////////////////////////////////////////////
///Приложение
////////////////////////////////////////////////////////////////////////////////
class   Application: public MemObject
{
  friend int  WINAPI WinMain (HINSTANCE,HINSTANCE,LPSTR,int);
  public:
                        Application     ();
            virtual     ~Application    ();

////////////////////////////////////////////////////////////////////////////////
///Функции пользователя
////////////////////////////////////////////////////////////////////////////////
            HINSTANCE   instance        () const { return mInst; }
            char*       cmd             () const { return mCmd;  }
            char*       argv            (int i) const { return mArgv [i]; }
            int         argc            () const { return mArgc; }
            Pool*       pool            () const { return MemObject::GetPool(); }

  protected:
    static  void        ProcessCmd      (const char* cmd,int& argc,char**& argv);

  protected:
            HINSTANCE           mInst;
            char*               mCmd;
            char**              mArgv;                        
            int                 mShow, mArgc;
};

Application*    A_GetApplication ();
HINSTANCE       A_GetInstance    ();

#endif
#ifndef __KERNEL_PLUGIN__
#define __KERNEL_PLUGIN__

#include <kernel\thread.h>

typedef DWORD (*executer_t)(Thread*,DWORD = 0);

////////////////////////////////////////////////////////////////////////////////
///Плагин
////////////////////////////////////////////////////////////////////////////////
class    Plugin: public Service
{
  public:
                        Plugin  (Thread*,HMODULE);
           virtual      ~Plugin ();

////////////////////////////////////////////////////////////////////////////////
///Взятие функции по имени
////////////////////////////////////////////////////////////////////////////////
           void*        GetProcAddr  (const char*); //farproc

////////////////////////////////////////////////////////////////////////////////
///Загрузка плагинов
////////////////////////////////////////////////////////////////////////////////
   static  void         LoadPlugins  (Thread*,const char* _DirName);

   virtual int          ServiceUID    () const { return mType; }
   virtual const char*  ServiceName   () const { return mName; }

  protected:
        HMODULE         mHandle;
        executer_t      mInit, mDone;
        uint            mType;
        char            mName [128];
};


#endif
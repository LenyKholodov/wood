#ifndef __KERNEL_SERVICE__
#define __KERNEL_SERVICE__

#include <kernel\defs.h>
#include <kernel\ticker.h>
#include <memory\pool.h>

#include <misc\ring.h>
#include <misc\dict.h>
#include <misc\string.h>
#include <misc\ptr.h>
#include <misc\flags.h>

class Thread;
class Service;

enum ServiceFlags {
  SRV_MULTITHREAD        = 1,
  SRV_OWNER_DEBUG_STREAM = 2
};

extern bool SrvMakeCurrent (Service*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Служба (контекст)
/////////////////////////////////////////////////////////////////////////////////////////////
class   Service: public Flags32, public KernelObject, public TickClient
{
  friend class Thread;
  friend bool  SrvMakeCurrent (Service*);
  protected:
                                Service  (CurrentContextPtr* context = NULL);
                     virtual    ~Service ();

  public:
                     enum  {
                       SRV_NO_ERROR = 0
                     };

/////////////////////////////////////////////////////////////////////////////////////////////
///Информация
/////////////////////////////////////////////////////////////////////////////////////////////
    virtual   const char*       ServiceName () const { return "Abstract service"; }

  protected:
///////////////////////////////////////////////////////////////////////////////////////////
///Присоединение службы к нити
///////////////////////////////////////////////////////////////////////////////////////////
    virtual   bool              bind        (Thread*);
    virtual   void              unbind      (Thread*,error_t = 0);

    static    bool              MakeCurrent (Service* srv) { return SrvMakeCurrent (srv); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Отслеживание ошибок
/////////////////////////////////////////////////////////////////////////////////////////////
              int               GetError ();
    virtual   const char*       GetErrorMessage (int error) const { return "Undefined error"; }  

  protected:
              void              SetError (int error = 0);

/////////////////////////////////////////////////////////////////////////////////////////////
///Работа с сисемой раздачи тиков 
///AddTimeEvent - добавление одиночного сообщения
///AddPeriodTimeEvent - добавление периодического сообщения
/////////////////////////////////////////////////////////////////////////////////////////////
              tickevent_t       AddTimeEvent       (clock_t time,tickdata_t event = 0);
              tickevent_t       AddPeriodTimeEvent (clock_t time_offs,tickdata_t event = 0);

              void              DelEvent           (tickevent_t);

    virtual   void              process            (tickdata_t) {}

///////////////////////////////////////////////////////////////////////////////////////////
///Присоединение потока вывода сообщений
///////////////////////////////////////////////////////////////////////////////////////////
              void              BindDebugStream   (std::ostream&);
              void              BindDebugStream   (const char* file_name);
              void              UnbindDebugStream ();
              std::ostream&     GetDebugStream    () const { return dbg; } 

  protected:
        ref_t<std::ostream>  dbg;
    
  private:
        int                 mLastError;
        ThreadRing          mThreadsUse;
        CurrentContextPtr*  mCurContext;
};


#endif
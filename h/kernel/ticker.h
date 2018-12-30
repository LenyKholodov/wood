#ifndef __KERNEL_TICKER_SYSTEM__
#define __KERNEL_TICKER_SYSTEM__

#include <kernel\thread.h>
#include <kernel\critsect.h>
#include <misc\ring.h>
#include <time.h>

typedef __int64    tickevent_t;
typedef DWORD      tickdata_t;

class    TickClient
{
  public:
        virtual ~TickClient () {}
        
    static   void TickEvent (void*,tickdata_t);
    virtual  void process   (tickdata_t) = 0;
};

typedef void (*TickHandler)(void*,tickdata_t);

/////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер раздачи сообщений по времени (милисекунды)
/////////////////////////////////////////////////////////////////////////////////////////////
class    Ticker: public MemObject
{
  friend class TimerThread;
  public:
                Ticker  (Pool* = Memory::M_GetPool());
     virtual    ~Ticker ();

     tickevent_t        AddPeriodEvent (TickClient& client,ulong period,tickdata_t data = 0);
     tickevent_t        AddTimeEvent   (TickClient& client,ulong offs,tickdata_t data = 0);

     tickevent_t        AddPeriodEvent (TickHandler,void* self,ulong period,tickdata_t data = 0);
     tickevent_t        AddTimeEvent   (TickHandler,void* self,ulong offs,tickdata_t data = 0);

     tickevent_t        AddPeriodEvent (TickHandler,ulong period,tickdata_t data = 0);
     tickevent_t        AddTimeEvent   (TickHandler,ulong offs,tickdata_t data = 0);

     void               DelEvent       (tickevent_t);

  private:
     tickdata_t         AddEvent       (TickHandler,void*,ulong,tickdata_t,bool);   
     void               process        ();
     

#if (CLK_TCK == 1000)
     inline  static ulong clock        () { return ::clock (); }
#else
     inline  static ulong clock        () { return float(::clock ())/float (CLK_TCK)*1000.0f; } 
#endif

  private:
    struct tick_t
    {
      bool        autoDel;
      long        offs, period; //int64??
      tickdata_t  data;
      TickHandler fn;
      tickevent_t event;
      void*       self;
    };

    typedef Ring<tick_t>  TickRing;

     void               Ticker::insert (TickRing::diter&);

  private:

    TickRing              mTimeList;
    Thread*               mThread;
    CriticalSection       mCS;
    int                   mProcess;
    long                  mLast, mOffs;
    tickevent_t           mCounter;
}; 

inline tickevent_t Ticker::AddPeriodEvent (TickHandler fn,ulong period,tickdata_t data)
{
  return AddPeriodEvent (fn,NULL,period,data);
}

inline tickevent_t Ticker::AddTimeEvent   (TickHandler fn,ulong offs,tickdata_t data)
{
  return AddTimeEvent (fn,NULL,offs,data);
}

#endif
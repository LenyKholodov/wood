#include <pch.h>
#include <kernel\ticker.h>

class    TimerThread: public Thread
{
  public:
                TimerThread (Ticker* tck): mTicker (tck), Thread (false) { 
                  StartThread ();
                }

  virtual int  process () { 
    if (mTicker) 
    {      
      mTicker->process ();
      return 0;
    }
    return 1;
  }   

  private:
        Ticker* mTicker;
};

tickevent_t     Ticker::AddPeriodEvent (TickClient& client,ulong period,tickdata_t data)
{
  return AddEvent (TickClient::TickEvent,&client,period,data,false);
}

tickevent_t     Ticker::AddTimeEvent   (TickClient& client,ulong offs,tickdata_t data)
{
  return AddEvent (TickClient::TickEvent,&client,offs,data,true);
}

tickevent_t     Ticker::AddPeriodEvent (TickHandler fn,void* self,ulong period,tickdata_t data)
{
  return AddEvent (fn,self,period,data,false);
}

tickevent_t     Ticker::AddTimeEvent   (TickHandler fn,void* self,ulong offs,tickdata_t data)
{
  return AddEvent (fn,self,offs,data,true);
}

void            Ticker::DelEvent       (tickevent_t event)
{
  CSEntry cs (mCS);

  TickRing::diter iter  = mTimeList;
  int             count = mTimeList.count ();
  
  for (int i=0;i<count;i++,iter++)
  {
    if (iter->event == event)
    {
      if (!mTimeList.IsLast (iter))
      {
        TickRing::diter next = iter; next++;

        next->offs    += iter->offs;
      }

      if (mTimeList.IsFirst (iter) && mTimeList.count ())
      {
        TickRing::diter next = iter; next++;

        mOffs          = next->offs;  
      }

      mTimeList.erase (iter);

      return;
    }
  }
}

tickdata_t      Ticker::AddEvent (TickHandler fn,void* self,ulong period,tickdata_t data,bool del)
{
  CSEntry cs (mCS);

  TickRing::diter iter  = mTimeList;  
  int             count = mTimeList.count ();
  tick_t          tick;
  bool            res   = false;

  tick.offs    = tick.period = period;
  tick.fn      = fn;
  tick.self    = self;
  tick.data    = data;
  tick.autoDel = del;
  tick.event   = mCounter;

  if (count)
  {
    for (int i=0;i<count && tick.offs > 0;tick.offs -= iter->offs,iter++,i++);

    iter--;

    if (tick.offs < 0)
    {
      tick.offs  += iter->offs;

      res = mTimeList.InsertBefore (iter,tick);  

      if (res) iter->offs -= tick.offs;      
    }
    else
      res = mTimeList.InsertAfter (iter,tick);
  }
  else
  {
    res = mTimeList.insert (tick);  
    if (res) mOffs = tick.offs;
  }  

  mThread->resume ();

  return res ? mCounter++ : -1;
}

void            Ticker::insert         (TickRing::diter& ins)
{
  CSEntry cs (mCS);

  mTimeList.erase (ins,false);  

  TickRing::diter iter  = mTimeList;  
  int             count = mTimeList.count ();

  ins->offs             = ins->period;

  if (count)
  {
    for (int i=0;i<count && ins->offs > 0;ins->offs -= iter->offs,iter++,i++);

    iter--;

    if (ins->offs < 0)
    {
      ins->offs  += iter->offs;
      iter->offs -= ins->offs;

      mTimeList.InsertBefore (iter,ins);  
    }
    else
      mTimeList.InsertAfter (iter,ins);    
  }
  else
    mTimeList.insert (ins);  
}

void            Ticker::process        ()
{
  D_Try
  {
    mLast    = clock ();
    mOffs    = 0;

    while (mProcess)
    {
      while (mProcess && mOffs + long (mLast - clock ()) > 10)
        mThread->sleep (mOffs + long (mLast - clock ()) - 5);

      mCS.enter ();

      if (mTimeList.count ())
      {        
        if (mOffs + long (mLast - clock ()) > 10)
        {
          mCS.leave ();
          continue;
        }  

        TickRing::diter iter = mTimeList;
        tick_t          tick = iter;

        if (!tick.autoDel) insert (iter);
        else               mTimeList.erase (iter);      

        mCS.leave ();

        mLast = clock ();

        tick.fn (tick.self,tick.data);

        mOffs = mTimeList.count () ? mTimeList.GetFirst ().data ().offs : 0;
      } 
      else 
      {
        mCS.leave ();
        mThread->pause ();   
      }  
    }

    mProcess = 2;
  }
  D_Exc ("Ticker: Error at process");  

  mThread = NULL;
}

Ticker::Ticker  (Pool* pool)
       : mTimeList (pool),
         mThread   (NULL),
         mLast     (0), mOffs (0),
         mCounter  (0),
         mProcess  (1)

{
  mThread = ext_new TimerThread (this);
}

Ticker::~Ticker ()
{
  mProcess = 0;

  if (mThread) 
  {
    for (;mProcess != 2;)
      if (mThread) mThread->resume ();
  }
}


void TickClient::TickEvent (void* obj,tickdata_t data)
{
  __try
  {
    ((TickClient*)obj)->process (data);                    
  }
  __except (1)
  {                    
  }
}
#include <pch.h>
#include <default.h>

extern Ticker* kernelTicker;

tickevent_t    K_SetTimeEvent   (TickClient& client,clock_t offs,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddTimeEvent (client,offs,data) : -1;
}

tickevent_t    K_SetPeriodTimeEvent (TickClient& client,clock_t period,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddPeriodEvent (client,period,data) : -1;
}

void           K_DelTimeEvent       (tickevent_t event)
{
  if (kernelTicker) kernelTicker->DelEvent (event);
}

tickevent_t    K_SetTimeEvent       (TickHandler fn,void* self,clock_t offs,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddTimeEvent (fn,self,offs,data) : -1;
}

tickevent_t    K_SetPeriodTimeEvent (TickHandler fn,void* self,clock_t period,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddPeriodEvent (fn,self,period,data) : -1; 
}

tickevent_t    K_SetTimeEvent       (TickHandler fn,clock_t offs,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddTimeEvent (fn,offs,data) : -1;
}

tickevent_t    K_SetPeriodTimeEvent (TickHandler fn,clock_t period,tickdata_t data)
{
  return kernelTicker ? kernelTicker->AddPeriodEvent (fn,period,data) : -1; 
}

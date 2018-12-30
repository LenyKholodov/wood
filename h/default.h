#ifndef __KERNEL_STATIC_INIT__
#define __KERNEL_STATIC_INIT__

#include <pool.h>
#include <binfile.h>
#include <resource.h>

#include <kernel\thread.h>
#include <kernel\ticker.h>

#include <misc.h>
#include <debug.h>
#include <fstream.h>

typedef bool (*ServiceInitFn)();
typedef void (*ServiceDoneFn)();

/////////////////////////////////////////////////////////////////////////////////////////////
///Запуск поддержки контеста приложения
/////////////////////////////////////////////////////////////////////////////////////////////
bool     K_Init (uint init_count = 0,...);
void     K_Done (uint init_count = 0,...);

/////////////////////////////////////////////////////////////////////////////////////////////
///Работа с временными сообщениями
/////////////////////////////////////////////////////////////////////////////////////////////
tickevent_t    K_SetTimeEvent       (TickClient& client,clock_t offs,tickdata_t data = 0);
tickevent_t    K_SetPeriodTimeEvent (TickClient& client,clock_t period,tickdata_t data = 0);
tickevent_t    K_SetTimeEvent       (TickHandler,void* self,clock_t offs,tickdata_t data = 0);
tickevent_t    K_SetPeriodTimeEvent (TickHandler,void* self,clock_t period,tickdata_t data = 0);
tickevent_t    K_SetTimeEvent       (TickHandler,clock_t offs,tickdata_t data = 0);
tickevent_t    K_SetPeriodTimeEvent (TickHandler,clock_t period,tickdata_t data = 0);
void           K_DelTimeEvent       (tickevent_t);

class   StaticInit
{
  public:
                StaticInit (const char* map_file = NULL) {
                  if (!mCount++)
                  {
                    K_Init ();
                    #ifdef DEBUG
                    SrvMakeCurrent (D_Load (map_file));
                    #endif
                  }  
                }
//                StaticInit (int argc = 0,char** argv = NULL) {
//                  if (!mCount++)
//                  {
//                    K_Init (argc,argv);
//                    #ifdef DEBUG
//                    SrvMakeCurrent (D_Load (NULL));
//                    #endif
//                  }  
//                }
                ~StaticInit () {
                  if (!--mCount)
                  {
                    #ifdef DEBUG
                    D_Done ();
                    #endif
                    K_Done ();
                  }
                }

                void* operator new (size_t size);
                void  operator delete (void* x);

  private:
    static  uint     mCount;
};

#define KernelInit StaticInit
#define K_FastInit(X) StaticInit ____staticInitFastInit (X);

#endif
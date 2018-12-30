#ifndef __KERNEL_THREAD__
#define __KERNEL_THREAD__

#include <kernel\defs.h>
#include <kernel\service.h>
#include <kernel\critsect.h>

class    Thread;

extern Thread*    K_GetCurrentThread();

////////////////////////////////////////////////////////////////////////////////
///Нить                                                                        
////////////////////////////////////////////////////////////////////////////////
class    Thread: public KernelObject
{
   friend class   Service; 
   friend Thread* ::K_GetCurrentThread  ();

   public:
                                Thread  (bool start_thread = true);
                    virtual     ~Thread ();             

//                    enum CreateFlags {
//                         COPY_ENV    = 1,       //Є®ЇЁа®ў ­ЁҐ б«г¦Ў
//                    };                                

////////////////////////////////////////////////////////////////////////////////
///Выполнение нити
////////////////////////////////////////////////////////////////////////////////
     virtual    int             process         () { return 0; }
     virtual    bool            valid           () const { return mPresent; }

  protected:
                bool            StartThread     (); //старт нити
                void            StopThread      (error_t = 0); //полная остановка нити

  public:
////////////////////////////////////////////////////////////////////////////////
///Пауза - возбуждение
////////////////////////////////////////////////////////////////////////////////
                void            pause            ();
                void            resume           ();
                void            sleep            (uint time_milisec) { Sleep (time_milisec); }

////////////////////////////////////////////////////////////////////////////////
///Работа со службами
////////////////////////////////////////////////////////////////////////////////                
                void            RegisterCurrent   (Service&);
                void            UnregisterCurrent (Service&); 
                void            RegisterAutodel   (Service&);
                void            UnregisterAutodel (Service&);


////////////////////////////////////////////////////////////////////////////////
///Информация о количестве запущенных нитей из данной нити (с учётом детей)
////////////////////////////////////////////////////////////////////////////////
                int             count            (bool children_enum = true) const;

  protected:
////////////////////////////////////////////////////////////////////////////////
///Нить - владелец
////////////////////////////////////////////////////////////////////////////////
                HANDLE          handle () const { return mHandle;   }
                DWORD           id     () const { return mThreadId; }
                Thread*         owner  () const { return mOwner;    }

   private:
     static     DWORD   CALLBACK RunThread        (void*);
                void             RegisterThread   (Thread*); 
                void             UnregisterThread  (Thread*); 

   protected:
        HANDLE          mHandle;
        DWORD           mThreadId;

   private:   
        bool            mPresent;
        ServiceRing     mServCur, mServDel;
        ThreadRing      mChildren;
        Thread*         mOwner;
        CriticalSection mCS;

        static  DWORD   mCurrentThread;
        static  int     mTotalThreadCount;
};

#endif
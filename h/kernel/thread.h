#ifndef __KERNEL_THREAD__
#define __KERNEL_THREAD__

#include <kernel\defs.h>
#include <kernel\service.h>
#include <kernel\critsect.h>

class    Thread;

extern Thread*    K_GetCurrentThread();

////////////////////////////////////////////////////////////////////////////////
///����                                                                        
////////////////////////////////////////////////////////////////////////////////
class    Thread: public KernelObject
{
   friend class   Service; 
   friend Thread* ::K_GetCurrentThread  ();

   public:
                                Thread  (bool start_thread = true);
                    virtual     ~Thread ();             

//                    enum CreateFlags {
//                         COPY_ENV    = 1,       //����஢���� �㦡
//                    };                                

////////////////////////////////////////////////////////////////////////////////
///���������� ����
////////////////////////////////////////////////////////////////////////////////
     virtual    int             process         () { return 0; }
     virtual    bool            valid           () const { return mPresent; }

  protected:
                bool            StartThread     (); //����� ����
                void            StopThread      (error_t = 0); //������ ��������� ����

  public:
////////////////////////////////////////////////////////////////////////////////
///����� - �����������
////////////////////////////////////////////////////////////////////////////////
                void            pause            ();
                void            resume           ();
                void            sleep            (uint time_milisec) { Sleep (time_milisec); }

////////////////////////////////////////////////////////////////////////////////
///������ �� ��������
////////////////////////////////////////////////////////////////////////////////                
                void            RegisterCurrent   (Service&);
                void            UnregisterCurrent (Service&); 
                void            RegisterAutodel   (Service&);
                void            UnregisterAutodel (Service&);


////////////////////////////////////////////////////////////////////////////////
///���������� � ���������� ���������� ����� �� ������ ���� (� ������ �����)
////////////////////////////////////////////////////////////////////////////////
                int             count            (bool children_enum = true) const;

  protected:
////////////////////////////////////////////////////////////////////////////////
///���� - ��������
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
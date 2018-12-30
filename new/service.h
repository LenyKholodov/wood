#ifndef __KERNEL_SERVICE__
#define __KERNEL_SERVICE__

#include <kernel\object.h>
#include <misc.h>

class Thread;
class Service;

/////////////////////////////////////////////////////////////////////////////////////////////
///�����⥫� �� ⥪�騩 ���⥪�� ���
/////////////////////////////////////////////////////////////////////////////////////////////
class   CurrentContextPtr
{
  public:
                CurrentContextPtr  (): mCurContext (0xFFFFFFFF) {}
                CurrentContextPtr  (const Service* srv) { set (srv); }
                ~CurrentContextPtr () {
                  if (mCurContext != 0xFFFFFFFF)
                    TlsFree (mCurContext);
                  mCurContext  = 0xFFFFFFFF;
                }

                 operator Service*  () const { return  data (); }
       Service*  operator ->        () const { return  data (); }

       CurrentContextPtr&  operator = (const Service* srv) { return set (srv); }

  protected:
        Service*        data () const { 
          return mCurContext == 0xFFFFFFFF ? NULL : (Service*)TlsGetValue (mCurContext);
        }
        CurrentContextPtr& set (const Service* srv) {
          if (mCurContext == 0xFFFFFFFF)
          {
            mCurContext = TlsAlloc ();            
            if (mCurContext == 0xFFFFFFFF)
              return *this;
          }  

          TlsSetValue (mCurContext,(DWORD)srv);

          return *this;
        }

  private:
        DWORD   mCurContext;
};

template <class T,uint type> class accessptr_t: public CurrentContextPtr
{
  public:
                accessptr_t  (const Service* srv):CurrentContextPtr (srv) {}

        inline T* operator -> () const { return (T*)data (); }
        inline operator T*    () const { return (T*)data (); }
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��㦡� (���⥪��)
/////////////////////////////////////////////////////////////////////////////////////////////
class   Service
{
  protected:
                                Service  (CurrentContextPtr* service_current_context_variable = NULL);
                     virtual    ~Service ();

  public:

  //duplicate

/////////////////////////////////////////////////////////////////////////////////////////////
///���㧪� / ����㧪� ��� ⥪�饩 ���
/////////////////////////////////////////////////////////////////////////////////////////////
    virtual  void               unload      (); //for current thread
    virtual  void               load        (); //for current thread

/////////////////////////////////////////////////////////////////////////////////////////////
///���ଠ��
/////////////////////////////////////////////////////////////////////////////////////////////
    virtual   const char*       ServiceName () const = 0;   

/////////////////////////////////////////////////////////////////////////////////////////////
///��⠭���� ⥪�饩 �㦡� ��� ���
/////////////////////////////////////////////////////////////////////////////////////////////
    static    bool              MakeCurrent (Service* = NULL); 
    static    Service*          GetCurrent  ();

    virtual   bool              bind        (Thread*);
    virtual   void              unbind      (Thread*);

/////////////////////////////////////////////////////////////////////////////////////////////
///��᫥������� �訡��
/////////////////////////////////////////////////////////////////////////////////////////////
              int               GetError ();      
  protected:
              void              SetError (int error = 0)
  public:
    virtual   const char*       GetErrorMessage (int error) const { return "Undefined error"; }  

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///����� � �ᥬ�� ࠧ��� ⨪�� 
/////////////////////////////////////////////////////////////////////////////////////////////
              tick_t            AddActiveEvent     (clock_t period,tickevent_t event = 0);
              tick_t            AddTimeEvent       (clock_t time,tickevent_t event = 0);
              tick_t            AddOffsTimeEvent   (clock_t time_offs,tickevent_t event = 0);

              void              DelEvent           (tick_t);

    virtual   void              tick               (tickevent_t) {}
    
  private:
        bool    mLock;          
        DWORD*  mCurContext;
        int     mLastError;
};

#endif
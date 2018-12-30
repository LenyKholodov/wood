#ifndef __KERNEL_CRITICAL_SECTION__
#define __KERNEL_CRITICAL_SECTION__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////////////////
///Критическая секция (для синхронизации работы нитей)
////////////////////////////////////////////////////////////////////////////////////////////
class    CriticalSection
{
  public:
                        CriticalSection   ();
                        ~CriticalSection  ();                        

////////////////////////////////////////////////////////////////////////////////////////////
///Вошли \ вышли
////////////////////////////////////////////////////////////////////////////////////////////
      void              enter             ();
      void              leave             ();
      bool              tryenter          ();

  private:
    CRITICAL_SECTION    mSection;    
};

class   CSEntry
{
  public:
                CSEntry  (CriticalSection& cs): mCS (cs) { mCS.enter (); }
                ~CSEntry () { mCS.leave (); }

                CriticalSection* operator -> () const { return &mCS; }
                operator CriticalSection*    () const { return &mCS; }

  private:
                CriticalSection& mCS;
};


typedef CriticalSection CritSect; //define

#endif
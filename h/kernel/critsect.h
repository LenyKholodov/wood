#ifndef __KERNEL_CRITICAL_SECTION__
#define __KERNEL_CRITICAL_SECTION__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ (��� ������������� ������ �����)
////////////////////////////////////////////////////////////////////////////////////////////
class    CriticalSection
{
  public:
                        CriticalSection   ();
                        ~CriticalSection  ();                        

////////////////////////////////////////////////////////////////////////////////////////////
///����� \ �����
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
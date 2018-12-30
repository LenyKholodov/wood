#ifndef __KERNEL_MUTEX__
#define __KERNEL_MUTEX__

#include <debug\defs.h>

////////////////////////////////////////////////////////////////////////////////
///�������
////////////////////////////////////////////////////////////////////////////////
class Mutex
{
  public:
              Mutex  ();
              ~Mutex ();

////////////////////////////////////////////////////////////////////////////////
///���������� - �������������
////////////////////////////////////////////////////////////////////////////////
        void  lock   () const;    
        void  unlock () const;
        
  private:
    HANDLE mHandle;
};

#endif
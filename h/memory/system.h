///////////////////////////////////////////////////////////////////////////////
//Name:         memory.h
//System:       Memory
//Abstract:     System pool for global alloc
///////////////////////////////////////////////////////////////////////////////

#include <memory/pool.h>

#ifndef __SYSTEM_POOL__
#define __SYSTEM_POOL__

///////////////////////////////////////////////////////////////////////////////
///Интерфейсный пул доступа к системной памяти
///////////////////////////////////////////////////////////////////////////////
struct  sysnode_t;
class   SystemPool: public Pool
{
  public:
                       SystemPool (); 
    virtual            ~SystemPool();
                       
                       enum Flags {
                         MEM_LOCK,
                         MEM_CACHED
                       };

            void*      alloc   ( size_t, int flags);

    virtual void       reset   ( void  );
    virtual bool       belong  ( void* ) const;

    virtual size_t     size     () const;
    virtual size_t     memavail () const;
    virtual size_t     maxavail  () const;

  protected:
    virtual void*      _alloc ( size_t );
    virtual void       _free  ( void*  );      
  
  private:
    sysnode_t*         mFirst; 
    sysnode_t*         mLast;
};

#endif
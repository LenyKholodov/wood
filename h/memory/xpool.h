#ifndef __MEMORY_SHARED_POOL__
#define __MEMORY_SHARED_POOL__

#include <kernel\critsect.h>
#include <memory\pool.h>

////////////////////////////////////////////////////////////////////////////////
///Пул с разделением доступа
////////////////////////////////////////////////////////////////////////////////
class    SharedPool: public Pool
{
  public:
                             SharedPool  (Pool*,bool autdel = false);
          virtual            ~SharedPool ();

          virtual       void    reset    (void);
          virtual       bool    belong   (void*) const;

          virtual       size_t  size     () const;
          virtual       size_t  memavail () const;
          virtual       size_t  maxavail () const;

          virtual       void    dump     (std::ostream&) const;
          virtual       bool    valid    ()        const;

  protected:
          virtual       void*   _alloc   (size_t);
          virtual       void    _free    (void*);

  private:
        CritSect  mTrans;        
        bool      mAutoDel;
};

#endif
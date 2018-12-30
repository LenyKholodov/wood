/////////////////////////////////////////////////////////////////////////////////////////////
//Name:         pool.h
//System:       Memory
//Abstract:     Abstract class for all memory managers
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_POOL__
#define __MEMORY_POOL__

#include <types.h>
#include <memory\object.h>
#include <iosfwd>

/////////////////////////////////////////////////////////////////////////////////////////////
///Абстрактный пул в памяти
/////////////////////////////////////////////////////////////////////////////////////////////
class   Pool: public MemObject
{
  friend class SharedPool;
        
  public:
                                Pool  (Pool* = NULL);
          virtual               ~Pool ();

                        void*   alloc   ( size_t );
                        void    free    ( void*  );

          virtual       void    reset   ( void   )        = 0;
          virtual       bool    belong  ( void*  ) const  = 0;

          virtual       size_t  size     () const   = 0;
          virtual       size_t  memavail () const   = 0;
          virtual       size_t  maxavail () const   = 0;

          virtual       void    dump    (std::ostream&) const;

          virtual       bool    valid    () const { return size () > 0; }

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Данные заглушки не должны выполнять проверку на корректность
/////////////////////////////////////////////////////////////////////////////////////////////
          virtual       void*   _alloc   ( size_t )             = 0;
          virtual       void    _free    ( void*  )             = 0;

                        Pool*   owner    () const   { return GetPool (); }
};


#endif
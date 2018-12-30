///////////////////////////////////////////////////////////////////////////////
//Name:         temp.h
//System:       Memory
//Abstract:     Temp pool for fast memory alloc
///////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_TEMP_POOL__
#define __MEMORY_TEMP_POOL__

#include <memory/flat.h>

///////////////////////////////////////////////////////////////////////////////
///Temp-pool не имеющий системы очистки 
///////////////////////////////////////////////////////////////////////////////
class   TempPool: public FlatPool
{
  public:
                      TempPool  ( Pool*,size_t );
                      TempPool  ( Pool*        );

   virtual size_t     maxavail () const { return memavail (); }

  protected:
   virtual void*      _alloc ( size_t );
   virtual void       _free  ( void*  ) {}
   
           void*      getpos () const { return mPos; }

  private:
   virtual void       _reset  (void*);
                
  private:
           char*      mPos;
};


#endif
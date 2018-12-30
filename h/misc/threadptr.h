#ifndef __THREAD_PTR__
#define __THREAD_PTR__

#include <types.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Указатель на текущий контекст нити
/////////////////////////////////////////////////////////////////////////////////////////////
class   CurrentContextPtr
{
  public:
                CurrentContextPtr  ();
                CurrentContextPtr  (void* data);
                CurrentContextPtr  (const CurrentContextPtr&);
                ~CurrentContextPtr ();

                                   enum {
                                     NO_LINK = 0xFFFFFFFF
                                   };

                           operator void* () const;
       CurrentContextPtr&  operator =     (void* data);
       CurrentContextPtr&  operator =     (const CurrentContextPtr&);

  protected:
        void*               data   () const;
        CurrentContextPtr&  set    (void*);
        void                unlink ();

  private:
        DWORD   mCurContext;
};

template <class Type> class threadptr_t: public CurrentContextPtr
{
  public:
                threadptr_t  ();
                threadptr_t  (const Type*);

    threadptr_t&   operator =        (const threadptr_t&);
    threadptr_t&   operator =        (const Type*);

    bool           operator ==       (const Type*);
    bool           operator !=       (const Type*);
    bool           operator !        () const;

    Type*          operator ->       () const;
                   operator Type*    () const;
    Type&          operator *        () const;
};

#define accessptr_t threadptr_t

#include "misc/threadptr.inl"

#endif
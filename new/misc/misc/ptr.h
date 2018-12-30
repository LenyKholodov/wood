#ifndef __POINTER_TEMPLATE__
#define __POINTER_TEMPLATE__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////
///Заглушка для указателя по смещению
////////////////////////////////////////////////////////////////////////////////

template <class T> class ptroffs_t
{
  public:
        ptroffs_t (): offs (0) {}
        ptroffs_t (void* x): offs (ulong(x)-ulong (this))  {}
        ptroffs_t (const ptroffs_t& x): offs (x.offs+(ulong(&x)-ulong(this))) {}

        inline ptroffs_t& operator = (const ptroffs_t& x) { offs = x.offs+(ulong(&x)-ulong(this)); return *this; }
        inline ptroffs_t& operator = (void* x) { offs = ulong(x)-ulong (this); return *this; }

        inline T*    operator -> () const { return (T*)((char*)this + offs); }
        inline operator T*       () const { return (T*)((char*)this + offs); }
        inline operator long     () const { return offs;                     }

        inline T*    GetData     () const { return (T*)((char*)this + offs); }
        inline long  GetOffs     () const { return offs; }

        inline T&    operator [] (int i)  const { return ((T*)((char*)this + offs)) [i]; }

  private:
        long    offs;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Для удобства
////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class ptr_t
{
  public:                        
                        ptr_t  (T* obj = NULL): mObj (obj) {}
                        ~ptr_t () {  }

        T* operator = (const T* x) { return mObj = (T*)x; }

        T*  operator -> () const { return mObj; }                
        operator T*     () const { return mObj; }        

  private:
        T*      mObj;
};

template <class T> class ref_t
{
  public:                        
                        ref_t  (T& obj): mObj (&obj) {}
                        ~ref_t () {  }
                        
        T& operator = (T* x) { return *(mObj = x); }

        T*  operator -> () const { return mObj; } 
        operator T&     () const { return *mObj; }

  private:
        T*       mObj;
};

#endif
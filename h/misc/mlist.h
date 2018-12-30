#ifndef  __KERNEL_MEM_LIST__
#define  __KERNEL_MEM_LIST__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////
///Список (только добавление) отображенный на массив в памяти
////////////////////////////////////////////////////////////////////////////////
template <class T> class MemListTmpl
{
  public:
                        MemListTmpl  (uint _ElementsNum,
                                      T*   _Table,
                                      uint _FirstCount = 0
                                     );
                        ~MemListTmpl () {}

              BOOL      empty       () const { return mCount == 0; }
              int       count       () const { return mCount;      }

              T&        operator [] (int i) const { return mTable [i]; }

              class iterator
              {
                friend class MemListTmpl<T>;
                public:
                                iterator (const MemListTmpl& list):mPos (0), mList (&list) {}
                                iterator (const iterator& iter):mPos (iter.mPos), mList (iter.mList) {}

                      iterator& operator =  (const iterator&    iter) { mPos = iter.mPos; mList = iter.mList; return *this;}
                      iterator& operator =  (const MemListTmpl& list) { mPos = 0; mList = &list; return *this;}
                                
                      T&        operator -> () const { return data(); }
                      operator  T&          () const { return data(); }

                      iterator&             operator  ++ ()    { return next(); }
                      const iterator&       operator  ++ (int) { return next(); }
                      iterator&             operator  -- ()    { return prev(); }
                      const iterator&       operator  -- (int) { return prev(); }


                            BOOL            last () { return mPos == mList->count (); }
                            T&              data () const { return mList->mTable [mPos]; }
                            void            reset() { mPos = 0; }

                protected:      
                        iterator&       next    () { mPos = (mPos != mList->count ()-1) ? mPos+1 : 0; return *this; }
                        iterator&       prev    () { mPos = (mPos != 0) ? mPos-1 : mList->count ()-1; return *this; }

                protected:               
                        int                   mPos;
                        const MemListTmpl*    mList;
              };

              friend class  MemListTmpl::iterator;

              BOOL      insert      (const T& obj);
              void      reset       () { mCount = 0; }

  protected:
    T*  alloc () { return mCount < mElementsNum ? mTable + mCount++ : NULL; }

  protected:                                                                     
    int mCount, mElementsNum;
    T*  mTable;
};

template <class T>
MemListTmpl<T>::MemListTmpl  (uint _ElementsNum,T* _Table,uint _FirstCount)
               : mCount (_FirstCount), mTable (_Table), mElementsNum (_ElementsNum)
 {   }

template <class T>
BOOL      MemListTmpl<T>::insert  (const T& obj)
{
  T* dest = alloc ();

  if (!dest) return FALSE;
  
  *dest = obj;

  return TRUE;
}

#endif


#ifndef __MISC_WOOD_STACK__
#define __MISC_WOOD_STACK__

#include <misc\list.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥪ ������饣� ࠧ���
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class Stack: private List<T>
{
  public:
                Stack (Pool* pool = Memory::M_GetPool ()): List<T> (pool) {}

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� / 㤠�����
/////////////////////////////////////////////////////////////////////////////////////////////  
        bool    push  (const T& x,bool exc = false) throw (ExcNoMemory&) { return List<T>::insert (x,exc); }
        T       pop   (bool exc = false) throw (ExcEmpty&);
        bool    pop   (T&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥪ 䨪�஢������ ࠧ���
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class FixedStack: public MemManager
{        
  public:
                FixedStack  (uint count,Pool* = Memory::M_GetPool ());
    virtual     ~FixedStack ();

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� / 㤠�����
/////////////////////////////////////////////////////////////////////////////////////////////  
        bool    push  (const T& x,bool exc = false) throw (ExcFull&);
        T       pop   () throw (ExcEmpty&);
        bool    pop   (T&);    

   virtual bool valid () const { return mStack != NULL; }

  private:
        T*      mStack;
        uint    mPos, mCount;
};

#include <misc\stack.inl>

#endif
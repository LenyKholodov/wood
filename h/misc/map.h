#ifndef __MISC_MAP__
#define __MISC_MAP__

#include <misc\tree.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///�⮡ࠦ����
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class key_t = signed int>
class Map: public BinTree<T,key_t>
{
  public:
                Map (Pool* pool = Memory::M_GetPool ()): BinTree<T,key_t> (pool), mMode (CREATE) {}

                enum NoElementModes {
                  CREATE  = 1,
                  EXCEPT  = 2
                };

/////////////////////////////////////////////////////////////////////////////////////////////
///������஢����                
/////////////////////////////////////////////////////////////////////////////////////////////
        T&       operator [] (key_t key) throw (ExcNoElement&, ExcNoMemory&);

/////////////////////////////////////////////////////////////////////////////////////////////
///������
/////////////////////////////////////////////////////////////////////////////////////////////
        int      GetMode () const { return mMode; }        
        void     SetMode (int mode) { mMode = mode; }
        
  private:
        int     mMode;
};

#include <misc\map.inl>

#endif
#ifndef __ENGINE_SUBSCENE__
#define __ENGINE_SUBSCENE__

#include <pool.h>  

////////////////////////////////////////////////////////////////////////////////////////////
///����業� - �᭮���� ��஢�� ������
////////////////////////////////////////////////////////////////////////////////////////////
class   SubScene: public MemObject
{
  public:
     virtual             SubScene ();

////////////////////////////////////////////////////////////////////////////////////////////
///���஢���� ����業
////////////////////////////////////////////////////////////////////////////////////////////
     virtual  bool       lock     ();                      
     virtual  void       unlock   ();

  protected:

////////////////////////////////////////////////////////////////////////////////////////////
///����७��� ��।������ �����
////////////////////////////////////////////////////////////////////////////////////////////
              void*      alloc    (size_t);
              void       free     (void*);
              Pool*      pool     () const;
              Pool*      M_GetPool () const;

////////////////////////////////////////////////////////////////////////////////////////////
///����㧪� �業�
////////////////////////////////////////////////////////////////////////////////////////////
     virtual  bool       load     ();
     virtual  void       unload   ();             
};

#endif
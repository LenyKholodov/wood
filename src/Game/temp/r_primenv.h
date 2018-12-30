#ifndef __RENDER_PRIMITIVE_ENVIRONMENT__
#define __RENDER_PRIMITIVE_ENVIRONMENT__

#include "defs.h"
#include <pool.h>

////////////////////////////////////////////////////////////////////////////////
///���㦥��� ��� �ਬ�⨢�� ७��ਭ�� (����ࠪ��)
////////////////////////////////////////////////////////////////////////////////
class    PrimitiveEnv: public MemObject
{
  public:
                        PrimitiveEnv  (Pool*);
          virtual       ~PrimiriveEnv ();

////////////////////////////////////////////////////////////////////////////////
///����� � MainRender
////////////////////////////////////////////////////////////////////////////////
          void       insert           (int x,int y,int tile,int detail = 0);
          void       reset            ();

          void       render           ();
          void       bind             (MainRender*);                

          uint       GetPgCount       ();
};

#endif
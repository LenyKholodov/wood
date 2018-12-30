#ifndef __RENDER_PRIMITIVE_ENVIRONMENT__
#define __RENDER_PRIMITIVE_ENVIRONMENT__

#include "defs.h"
#include <pool.h>

////////////////////////////////////////////////////////////////////////////////
///Окружение для примитивов рендеринга (абстракция)
////////////////////////////////////////////////////////////////////////////////
class    PrimitiveEnv: public MemObject
{
  public:
                        PrimitiveEnv  (Pool*);
          virtual       ~PrimiriveEnv ();

////////////////////////////////////////////////////////////////////////////////
///Работа с MainRender
////////////////////////////////////////////////////////////////////////////////
          void       insert           (int x,int y,int tile,int detail = 0);
          void       reset            ();

          void       render           ();
          void       bind             (MainRender*);                

          uint       GetPgCount       ();
};

#endif
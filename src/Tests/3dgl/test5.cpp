#include <graph\mdlutils.h>

void     main ()
{
  static Object3D obj3ds (M_GetPool(),"data/teapot.3ds");  

  ModelCompile comp      (M_GetPool());

  comp.BindFacets        (obj);
}
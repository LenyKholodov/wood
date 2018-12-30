#include "pch.h"
#include <graph\grtypes.h>
#include <kernel.h>

#include "cv_mdlsprite.h"

int      main (int argc,char* argv [])
{
  if (argc < 3)
  {
    cout<<"makesprite.exe <sprite.txt> <sprite.wmd>"<<endl;
    return 1;
  }   

  K_Init ();
  D_Load ("makesprite.map");

  FloatPool pool  (M_GetPool(),32000000);
  Sprite3DModelConv conv  (&pool,argv [1]);

  conv.Save               (argv [2]);  

  D_Done ();
  K_Done ();

  return 0;
}

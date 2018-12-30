#include <iostream.h>

#include "cv_mdlsmp.h"
#include "cv_mdlmulti.h"

void      LoadVec  (vector3d_t& v,char* str);

int      main (int argc,char* argv [])
{
  vector3d_t size (0);
  int        mode = 0;

  if (argc < 3)
  {
    cout<<"3ds2mdl.exe <model.3ds> <model.wmd> -key"<<endl;
    cout<<"\t"<<"-norm\t\tNormalize model to user size"<<endl;
    cout<<"\t"<<"-mode\t\tSet model mode (multi-material: 1, 1-material: 0)"<<endl;
    cout<<"\t"<<"-dir\t\tInformation program about dest dir: model.3ds->models\model.wmd"<<endl;
    return 1;
  }

  int cnt = argc - 3;

  for (int i=3;i<cnt+3;i++)
  {
    if (!strcmp (argv [i],"-norm"))  
    {
      LoadVec  (size,argv [++i]);
      cout<<"...Normalize to "<<size<<endl;
    }  
    if (!strcmp (argv [i],"-mode")) mode = atoi (argv [++i]);
    if (!strcmp (argv [i],
  }                     

  FloatPool     pool  (M_GetPool(),32000000);
  Object3D      obj   (&pool,argv [1]);
  
  switch (mode)
  {
    case 1:
    {
      cout<<"...Create multi-material model"<<endl;
      MultiModelConv  conv  (&pool,obj,size);
      conv.Save             (argv [2]);
      break;
    }  
    case 0:
    {
      cout<<"...Create 1-material model"<<endl;
      SmpModelConv  conv  (&pool,obj,size);
      conv.Save           (argv [2]);
      break;
    }  
  }

  return 0;
}


void      LoadVec  (vector3d_t& v,char* str)
{
  char   dig [16];
  char*  pos = dig;  

  int    value [30] = {0};
  int*   ipos  = value;
  int    state = 0;

  while (*str++)
    if      ((isdigit (*str) || *str == '-') && !state) 
    {
      state  = TRUE;
      pos    = dig;
      *pos++ = *str;
    }  
    else if ((isdigit (*str) || *str == '-') && state)
      *pos++ = *str;
    else if (!(isdigit (*str) || *str == '-') && state) 
    {
      state   = FALSE;
      *pos    = 0;
      *ipos++ = atoi (dig);
    }
  
  v.x  = value [0];
  v.y  = value [1];
  v.z  = value [2];
}
#include "pch.h"
#include <iostream.h>
#include <scripts\us.h>
#include <kernel.h>

#include "cv_mdlmulti.h"
#include "cv_utils.h"

#include <string.h>

istream& operator >> (istream& is,vector3d_t& v)
{
  is>>v.x>>v.y>>v.z;

  return is;
}

void      LoadVec  (vector3d_t& v,char* str);
void      ComputeMatFileName (const char*,char*);

int      main (int argc,char* argv [])
{
//  CONV3DS_CONVERT_ALPHA_TEXTURES = TRUE;

  K_Init            ();  
  D_Load            ("3ds2mdl.map");
  F_AddFastPath     ("textures","textures\\");

  vector3d_t size (.0f);
  char      srcName   [128] = "default.3ds",
            destName  [128] = "default.wmd",
            matFile   [128] = "",
            matPrefix [32]  = "";

  USStreamReader reader (M_GetPool(),cout);

  if (argc < 2)
  {
    cout<<"3ds2mdl.exe <makefile.mk>"<<endl;
    D_Done ();
    K_Done ();
    return 1;
  }

  reader.BindString     ("source",srcName);
  reader.BindString     ("dest",destName);
  reader.BindString     ("mat_prefix",matPrefix);
  reader.BindString     ("mat_file",matFile);
  reader.Bind           ("norm",size);

  reader.read           (argv [1]);

  FloatPool     pool  (M_GetPool(),32000000);
  Object3D      obj   (&pool,srcName,30000000);

  if (matPrefix [0]) obj.SetMatPrefix (matPrefix);
  if (!matFile [0]) ComputeMatFileName (srcName,matFile);
  
  MultiModelConv  conv  (&pool,obj,size);
  conv.Save             (destName,&cout);

  CreateMatFile (obj,matFile);

  D_Done ();
  K_Done ();

  return 0;
}

void      ComputeMatFileName (const char* src,char* dest)
{ 
  int   len = strlen (src);

  for (const char* str = src+len-1;str != src && *str!='.';str--);

  if (str == src)
  {
    strcpy (dest,"materials/default.mat");
    return;
  }

  for (const char* str1 = str;*str1 && *str1 != '/' && *str1 != '\\';str1--);  
  
  if (*str1 == '\\' || *str1 == '/')
   str1++;

  char temp [128];

  memcpy (temp,str1,ulong (str-str1));

  temp [ulong (str-str1)] = 0;

  strcpy (dest,"materials/");
  strcat (dest,temp);
  strcat (dest,".mat");
}
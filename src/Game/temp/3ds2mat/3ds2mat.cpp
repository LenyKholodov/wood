#include <graph\x3f.h>
#include <iostream.h>

#include "cv_utils.h"

void     main (int argc,char* argv [])
{
  if (argc < 2)
  {  
    cout<<"3ds2mat <model.3ds>"<<endl;
    return;    
  }

  int  len   = strlen (argv [1]);
  char dest  [128], filename [128], src [128], dir [128];

  strcpy (dest,argv [1]);
  strcpy (src,argv [1]);
//  strcpy (src,"3ds/");
//  strcat (src,argv [1]);

  char  *s1 = strchr (dest,'/'), 
        *s2 = strchr (dest,'\\'), 
        *s  = dest;

  while (s1 || s2)
  {
    s1 = strchr (s,'/');
    s2 = strchr (s,'\\');
    
    if (s1 || s2) 
    {
      s  = s1 > s2 ? s1 : s2;
      s++;
    }  
  }  
  
  for (char* str = dest+len-1;str != dest && *str!='.';str--);  

  if (str != dest) strcpy (str,".mat");
  else             strcat (dest,".mat");  

  strcpy (dir,dest);
  dir [ulong (str-dest)] = 0;

  strcpy (filename,"materials/");
//  strcat (filename,dest);
  strcat (filename,s);

  dout<<filename<<endl;

  FloatPool     pool  (M_GetPool(),32000000);
  Object3D      obj   (&pool,src);

  obj.SetMatPrefix    (dir);

  CreateMatFile       (obj,filename);  
}
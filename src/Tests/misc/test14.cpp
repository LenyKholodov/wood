#include <iostream.h>
#include <kernel.h>

void     main ()
{
  K_FastInit (NULL);

  char name [] = "c:\\hllo/world/x.exe";
  char dir [128], file [128], ext [128];

  ParseFileName (name,dir,file,ext);  

  cout<<"dir: '"<<dir<<"'"
      <<" file: '"<<file<<"'"      
      <<" ext: '"<<ext<<"'"
      <<endl;
}
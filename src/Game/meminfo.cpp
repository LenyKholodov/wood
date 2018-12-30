#include "pch.h"
#include "cv_mem.h"
#include "g_level.h"

#include <iostream.h>
#include <default.h>

void main (int argc,char* argv [])
{
  K_FastInit ("meminfo.map");

  if (argc < 2)
  {
    cout<<"meminfo.exe <levelname.map>"<<endl;
    return ;
  }  

  Pool*        pool  = M_GetPool (); 

  BinFile*     file  = F_OpenFile (argv [1],BF_OPEN_RW);
  GameLevel*   level = new (pool) GameLevel (pool,file);    

  BuildMemoryInfo (*level);

  delete level;
  delete file;  
}


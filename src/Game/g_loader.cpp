#include "pch.h"
#include "g_loader.h"

#include <string.h>

BOOL       LevelLoader::Load         ()
{
  D_Try
  {
    M_PushPool (objPool);

    BOOL state = FALSE;

    D_Try
    {
      state = LoadData ();
    }
    D_Exc ("Error at LoadData");

    M_PopPool ();

    return state;
  }
  D_ExcRet ("LevelLoader: Error at Load",FALSE);
}

void       LevelLoader::Unload       ()
{
  D_Try
  {
    UnloadData ();
  }
  D_Exc ("LevelLoader: Error at Unload");
}

LevelLoader::LevelLoader  (Pool* _Pool,const char* name,size_t _PoolSize)
            : MemObject    (_Pool),
              objPool      (_PoolSize ? ext_new FloatPool (_Pool,_PoolSize) :
                            _Pool),
              level        (objPool,name)
{
  D_Try
  {     
    if (!::IsValid (objPool) || !::IsValid (&level))
    {
      D_Message ("LevelLoader: No enough memory or wrong game level");
      return;
    }    

    memset (&data.pool,0,sizeof (data.pool));
  }
  D_Exc ("GameLoader: Error at construct");
}

LevelLoader::~LevelLoader ()
{
  D_Try
  {
    Unload ();

    if (objPool && objPool != GetPool()) delete objPool;
  }
  D_Exc ("LevelLoader: Error at destruct");
}

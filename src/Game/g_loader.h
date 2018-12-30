#ifndef __LEVEL_LOADER__
#define __LEVEL_LOADER__

#include "g_level.h"

class    LandscapeMap;
//class    MatEnv;
class    TreeEnv;
class    ModelEnv;
class    VertexBuffer;

////////////////////////////////////////////////////////////////////////////////
///Глобальные объекты
////////////////////////////////////////////////////////////////////////////////
class    LevelLoader: public MemObject
{
  public:
    Pool*           objPool;            //Пул под объекты
    GameLevel       level;              //Игровой уровень

    union Data{                         //Список объектов
      struct Obj{
        LandscapeMap*   mLandMap;
        VertexBuffer*   mVertexBuffer;
      }obj;
      char      pool [sizeof (void*) * LOADER_NUM_OF_OBJECTS];
    }data;                    

  public:
                        LevelLoader  (Pool*,const char*,size_t _PoolSize = 0);
    virtual             ~LevelLoader ();    

    virtual bool        valid        () const { return level.valid () && IsValid (); }

////////////////////////////////////////////////////////////////////////////////
///Загрузка и выгрузка определяются пользователем
////////////////////////////////////////////////////////////////////////////////
             BOOL       Load         ();
             void       Unload       ();
             BOOL       IsValid      () const;

             Data::Obj* operator ->  () const { return (Data::Obj*)&data.obj; }
             Data::Obj* access       () const { return (Data::Obj*)&data.obj; }
             operator   Data::Obj*   () const { return (Data::Obj*)&data.obj; }

  protected:
             BOOL       LoadData     ();
             void       UnloadData   ();
};

#endif
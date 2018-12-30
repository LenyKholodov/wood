#ifndef __RENDER_TREE_ENV__
#define __RENDER_TREE_ENV__

#include "r_treerender.h"
#include "r_treekeeper.h"
#include "r_treemap.h"

class    LevelLoader;
class    SBuffer;
class    Camera;

////////////////////////////////////////////////////////////////////////////////
///Окружение деревьев
////////////////////////////////////////////////////////////////////////////////
class    TreeEnv: public MemObject
{
  friend class MainRender;
  friend class TreeRender;
  public:
                TreeEnv  (Pool*,
//                          LevelLoader&,
                          MainRender*,  
                          size_t _MeshCacheSize,
                          uint   _VertexCacheSize
                         );
       virtual  ~TreeEnv ();

       virtual  bool valid () const { return mKeeper.valid () && mRender.valid (); }

////////////////////////////////////////////////////////////////////////////////
///Изменение масштаба
////////////////////////////////////////////////////////////////////////////////
       void     SetScale (const vector3d_t& v) { mRender.SetScale (v); }

  protected:
////////////////////////////////////////////////////////////////////////////////
///Установка функций окружения
////////////////////////////////////////////////////////////////////////////////
     void       SetSBuffer       (const SBuffer*);
     void       SetFrustum       (const frustum_t&);
     void       SetCamera        (const Camera&);

////////////////////////////////////////////////////////////////////////////////
///Работа с MainRender
////////////////////////////////////////////////////////////////////////////////
     void       insert           (int x,int y,int tile,int detail = 0);
     void       reset            ();

     void       render           ();
     void       bind             (MainRender*); 

     uint       GetPgCount       () { return mRender.GetTPS (); }

  private:
     LevelLoader&       mLoader;
     TreeKeeper         mKeeper;     
     TreeRender         mRender;     
};

#endif
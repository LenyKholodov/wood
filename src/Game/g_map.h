#ifndef __G_MAP__
#define __G_MAP__

#include <binfile.h>
#include "g_cachemap.h"

////////////////////////////////////////////////////////////////////////////////
///Стёк высот (от меньшей index = 0 к большой index = 64)
////////////////////////////////////////////////////////////////////////////////
struct heightstage_t {
  float         height [64];
};

////////////////////////////////////////////////////////////////////////////////
///Карта ланшафта
////////////////////////////////////////////////////////////////////////////////
class   LevelLoader;
class   LandMeshRender;
class   LandscapeCache;
class   LandscapeMap: public CacheMap
{
  protected:
       grtile_t*           map;                         //Тайлы

  public:
       int                 cacheWidth, cacheHeight;     //Размеры кэш-прямоугольника
       int                 vCount, fCount;              //Количество элементов
      
  public:
       int                 width, height;               //Размеры карты
       boundbox_t          bbox;                        //Ограничивающий пар-пед
       float               step;                        //Шаг дискретной сетки

  public:
                LandscapeMap   (Pool*,LevelLoader&,size_t land_cache_size);
       virtual  ~LandscapeMap  ();

////////////////////////////////////////////////////////////////////////////////
///Корректность
////////////////////////////////////////////////////////////////////////////////
       virtual bool valid      () const { return CacheMap::valid () && map && IsValid ((MemObject*)mCache); }

////////////////////////////////////////////////////////////////////////////////
///Кэширование карты
////////////////////////////////////////////////////////////////////////////////             
            void                SetPos         (const vector3d_t& p) { CacheMap::SetPos (p.x,p.z); }

            grtile_t&           tile  (int,int);
    inline  int                 index (int x,int y) { return (y-mBox.i.y1) * (mBox.i.x2-mBox.i.x1+1) + x - mBox.i.x1; }
    inline  grtile_t&           tile  (int index)   { return map [index]; }

            LandscapeCache*     cache ()  { return mCache; }

////////////////////////////////////////////////////////////////////////////////
///Определение высоты
////////////////////////////////////////////////////////////////////////////////
            uint                CalcHeight    (const vertex_t&,heightstage_t&);
            float               CalcMaxHeight (const vertex_t&);
            float               CalcMinHeight (const vertex_t&);

  protected:
    virtual void                reload        (const box_t&);
    virtual void                resize        (uint);

  private:
     BinFile*           mFile;
     LandscapeCache*    mCache;
};

#endif
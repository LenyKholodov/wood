#ifndef __RENDER_SPRITE_KEEPER__
#define __RENDER_SPRITE_KEEPER__

#include <math3d.h>
#include <misc.h>
#include "r_mdl3d.h"
#include <fixed\tables.h>

struct   sprite3d_t 
{
  int           texture;
  Angle         start, end;          //after
};

struct   spritemodeldata_t
{
  float         koef;   //Коэффициент домножения угла (опр. индекса спрайта) if (koef<0) then NO_KOEF

  struct lump_t
  {
    ulong       offs;   //Смещение данных (от data [])
    ulong       num;    //Количество элементов
    ulong       size;   //Размер данных
  }text, sprite;

  char     data [];  
};

struct  spritemodel_t: public m3dmodel_t
{
  Angle             angle;          //Угол "зрения" спрайта (относительно NORTH z = 0, x = 1)
  float             width, height;  //Размеры спрайта в мировых координатах

  ptroffs_t<int>    reindex;

  char              data [];
};

////////////////////////////////////////////////////////////////////////////////
///Хранилище 3D спрайтов
////////////////////////////////////////////////////////////////////////////////
class    Sprite3DModelKeeper: public M3DModelKeeper
{
  public:
                Sprite3DModelKeeper  (Pool*,ModelEnv*);
       virtual  ~Sprite3DModelKeeper ();

  protected:
     virtual    int        filetype      () const { return SPRITE_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///Работа с моделью
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///Рендеринг
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);  
    virtual void           DrawAlpha        (DWORD param,const Camera&);
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&);

  private:
        MATGROUP        mMatGroup;
};

#endif
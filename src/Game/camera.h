#ifndef __ENGINE_CAMERA__
#define __ENGINE_CAMERA__

#include <graph\grtypes.h>
#include <graph\utils.h>

/*

////////////////////////////////////////////////////////////////////////////////
///Камера
////////////////////////////////////////////////////////////////////////////////
class   Camera
{
  public:
                                Camera ();
                                Camera (const vector3d_t&,
                                        const vector3d_t&,
                                        const vector3d_t&);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
    const  vector3d_t&          pos () const { return mPos; }
    const  vector3d_t&          dir () const { return mDir; }
    const  vector3d_t&          up  () const { return mUp;  }

////////////////////////////////////////////////////////////////////////////////
///Установление камеры
////////////////////////////////////////////////////////////////////////////////
           void                 set (const vector3d_t& _Pos,
                                     const vector3d_t& _Dir,
                                     const vector3d_t& _Up
                                    );

  protected:
    vector3d_t          mPos;
    vector3d_t          mDir;
    vector3d_t          mUp;        
};      */

#ifndef __RENDER_CAMERA__
#define __RENDER_CAMERA__

/////////////////////////////////////////////////////////////////////////////////////////////
///Камера
/////////////////////////////////////////////////////////////////////////////////////////////
class   Camera
{
  public:
                        Camera  ();
                        Camera  (const vector3d_t& pos,
                                 const vector3d_t& dir,
                                 const vector3d_t& up
                                );

/////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на попадение в область видимости (box,sphere,facet,portal,base)
/////////////////////////////////////////////////////////////////////////////////////////////
     template <typename T>
     bool               visible (const T& el) const { return IsInBound (el,6,frustum.clip); }
     bool               visible (const vertex_t& center,float width) const { return IsInBound (center,width,6,frustum.clip); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Управление камерой
/////////////////////////////////////////////////////////////////////////////////////////////
     void               set     (const vector3d_t& pos,
                                 const vector3d_t& dir,
                                 const vector3d_t& up
                                );

     void               recalc  ();
  
  public:
    vector3d_t  pos, dir, up;
    matrix_t    view;
    frustum_t   frustum;
};

#endif
  
#endif
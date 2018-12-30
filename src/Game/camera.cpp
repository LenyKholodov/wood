#include <pch.h>
#include "camera.h"

void   Camera::set 
 ( const vector3d_t& _Pos,
   const vector3d_t& _Dir,
   const vector3d_t& _Up
 )
{
  pos = _Pos;
  up  = _Up;
  dir = _Dir;
}

void    Camera::recalc  ()
{
  //calc base

  CalcFrustum (view,frustum);
}

Camera::Camera ()
       : pos (vector3d_t (.0f)),
         dir (vector3d_t (0,0,-1)),
         up  (vector3d_t (0,1,0))
{
}

Camera::Camera (const vector3d_t& _Pos,const vector3d_t& _Dir,const vector3d_t& _Up)
       : pos (_Pos),
         dir (_Dir),
         up  (_Up) 
{
}
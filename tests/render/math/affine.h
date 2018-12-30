#pragma once

#include "matrix.h"

namespace math
{
////////////////////////////////////////////////////////////////////////////////////////////
///Параллельный перенос
////////////////////////////////////////////////////////////////////////////////////////////
  template <class type,class base,unsigned int size>
  matrix<type,size+1> translate (const vec<type,size,base>&);

  template <class type>
  matrix<type,4> translate (const type&,const type&,const type&);

////////////////////////////////////////////////////////////////////////////////////////////
///Масштабирование
////////////////////////////////////////////////////////////////////////////////////////////
  template <class type,class base,unsigned int size> 
  matrix<type,size+1> scale (const vec<type,size,base>&);

  template <class type>
  matrix<type,4>  scale (const type&,const type&,const type&);

////////////////////////////////////////////////////////////////////////////////////////////
///Поворот вокруг оси
////////////////////////////////////////////////////////////////////////////////////////////
  template <class type,class base> 
  matrix<type,4> rotate (const type& angle,const vec<type,3,base>& axis);  

  template <class type> 
  matrix<type,4> rotate (const type& angle,const type& axis_x,const type& axis_y,const type& axis_z);  

  #include "affine.inl"
}

using math::translate;
using math::scale;
using math::rotate;

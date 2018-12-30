#pragma once

#include "vec3.h"

namespace math
{
////////////////////////////////////////////////////////////////////////////////////////////
///Углы Эйлера
////////////////////////////////////////////////////////////////////////////////////////////
template <class type> struct EulerAngle
{
  type pitch, yaw, roll;

                      EulerAngle ();
                      EulerAngle (const type&,const type&,const type&);
  template <class t1> EulerAngle (const EulerAngle<t1>&);

  template <class t1> EulerAngle& operator =   (const EulerAngle<t1>&);
};

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования углов (градусы - радианы)
////////////////////////////////////////////////////////////////////////////////////////////
template <class type> type Deg2Rad (const type& angle);
template <class type> type Rad2Deg (const type& angle);

#include "angles.inl"

}

using math::Deg2Rad;
using math::Rad2Deg;

typedef math::EulerAngle<float>           EulerAnglef;
typedef math::EulerAngle<double>          EulerAngled;
typedef math::EulerAngle<int>             EulerAnglei;
typedef math::EulerAngle<unsigned int>    EulerAngleui;
typedef math::EulerAngle<short>           EulerAngles;
typedef math::EulerAngle<unsigned short>  EulerAngleus;
typedef math::EulerAngle<char>            EulerAngleb;
typedef math::EulerAngle<unsigned char>   EulerAngleub;

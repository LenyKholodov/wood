#include <pch.h>
#include <math\euler.h>

EulerAngle&  EulerAngle::operator = (const quatf& q)
{
  double w2  = q.w * q.w,
         x2  = q.x * q.x,
         y2  = q.y * q.y,
         z2  = q.z * q.z;

  pitch      = Rad2Deg (atan (2.0 * (q.y*q.z + q.x*q.w)/(-x2 - y2 + z2 + w2)));
  yaw        = Rad2Deg (asin (-2.0 * (q.x*q.z - q.y*q.w)));  
  bank       = Rad2Deg (atan (2.0 * (q.x*q.y + q.z*q.w)/(x2 - y2 - z2 + w2)));

  return *this;
}

EulerAngle::EulerAngle (quatf& q)
{
  double w2  = q.w * q.w,
         x2  = q.x * q.x,
         y2  = q.y * q.y,
         z2  = q.z * q.z;

  pitch      = Rad2Deg (atan (2.0 * (q.y*q.z + q.x*q.w)/(-x2 - y2 + z2 + w2)));
  yaw        = Rad2Deg (asin (-2.0 * (q.x*q.z - q.y*q.w)));  
  bank       = Rad2Deg (atan (2.0 * (q.x*q.y + q.z*q.w)/(x2 - y2 - z2 + w2)));
}



EulerAngle::EulerAngle () 
  {  }

EulerAngle::EulerAngle (float _pitch,float _yaw,float _bank)
{
  pitch = _pitch;
  yaw   = _yaw;
  bank  = _bank;
}


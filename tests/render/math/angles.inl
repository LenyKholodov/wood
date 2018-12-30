template <class type>
template <class t1> 
EulerAngle<type>& EulerAngle<type>::operator = (const EulerAngle<t1>& a)
{
  pitch = a.pitch;
  yaw   = a.yaw;
  roll  = a.roll;

  return *this;
}

template <class type>
template <class t1> 
EulerAngle<type>::EulerAngle (const EulerAngle<t1>& a)
  : pitch (a.pitch), yaw (a.yaw), roll (a.roll)
{  }

template <class type>
__forceinline EulerAngle<type>::EulerAngle () {}

template <class type>
__forceinline EulerAngle<type>::EulerAngle (const type& _pitch,const type& _yaw,const type& _roll)
  : pitch (_pitch), yaw (_yaw), roll (_roll)
{  }

template <class type> 
__forceinline type Deg2Rad (const type& angle) 
{ 
  return angle * type (PI) / type (180); 
}

template <class type> 
__forceinline type Rad2Deg (const type& angle) 
{ 
  return angle * type (360) / type (PI); 
}

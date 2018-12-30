// определения инлайнов к vector3d.h



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Прибамбасы к vec3
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Всяка чухня

template <class Type>vec3<Type>& vec3<Type>::operator+=(const vec3& v) 
{ 
  x += v.x; 
  y += v.y; 
  z += v.z; 

  return *this; 
}

template <class Type>vec3<Type>& vec3<Type>::operator-=(const vec3& v) 
{ 
  x -= v.x; 
  y -= v.y; 
  z -= v.z; 

  return *this; 
}

template <class Type>vec3<Type>& vec3<Type>::operator*=(const vec3& v) 
{ 
  x *= v.x; 
  y *= v.y; 
  z *= v.z; 

  return *this; 
}

template <class Type>vec3<Type>& vec3<Type>::operator/=(const vec3& v) 
{ 
  x /= v.x; 
  y /= v.y; 
  z /= v.z; 

  return *this; 
}
        
template <class Type>vec3<Type>& vec3<Type>::operator*=(Type d)         
{ 
  x *= d; 
  y *= d; 
  z *= d; 

  return *this; 
}

template <class Type>vec3<Type>& vec3<Type>::operator/=(Type d)
{
  return operator*=(Type(1)/d);
}

template <class Type> Type vec3<Type>::operator&(const vec3& v) const
{ 
  return x*v.x+y*v.y+z*v.z;
}

template <class Type>vec3<Type> vec3<Type>::operator^(const vec3<Type>& v) const
{
  return vec3<Type>(y*v.z-z*v.y,
                            z*v.x-x*v.z,
                            x*v.y-y*v.x
                           );
}

//-------------------------------------------------------------------------------------------------
// Сравнение векторов


template <class Type> bool vec3<Type>::operator==(const vec3<Type>& v) const
{
  return (x==v.x) && (y==v.y) && (z==v.z);
}

template <class Type>bool vec3<Type>::operator!=(const vec3<Type>& v) const
{
  return (x!=v.x) || (y!=v.y) || (z!=v.z);
}

//-------------------------------------------------------------------------------------------------
// Потоковый ввод-вывод

//template <class Type> ostream& operator<<(ostream& os,const vec3<Type>& v)
//{
//  os<<'('<<v.x<<';'<<v.y<<';'<<v.z<<')';
//  return os;
//}

//-------------------------------------------------------------------------------------------------
// "Вынесенные" операции над векторами

template <class Type>Type angle(const vec3<Type>& a,const vec3<Type>& b)
{
        Type angle=acos((a&b)/sqrt(a.qlen()*b.qlen()));
        return _isnan(angle)?Type(0):angle;
}

template <class Type> vec3<Type> abs(const vec3<Type>& v)
{ 
        return vec3<Type>(v.x<Type(0)?-v.x:v.x,
                                  v.y<Type(0)?-v.y:v.y,
                                  v.z<Type(0)?-v.z:v.z);
}

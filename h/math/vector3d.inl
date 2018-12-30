// определения инлайнов к vector3d.h

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Прибамбасы к Vector3DTmpl
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Всяка чухня

template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator+=(const Vector3DTmpl& v) 
{ 
  x += v.x; 
  y += v.y; 
  z += v.z; 

  return *this; 
}

template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator-=(const Vector3DTmpl& v) 
{ 
  x -= v.x; 
  y -= v.y; 
  z -= v.z; 

  return *this; 
}

template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator*=(const Vector3DTmpl& v) 
{ 
  x *= v.x; 
  y *= v.y; 
  z *= v.z; 

  return *this; 
}

template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator/=(const Vector3DTmpl& v) 
{ 
  x /= v.x; 
  y /= v.y; 
  z /= v.z; 

  return *this; 
}
        
template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator*=(Type d)         
{ 
  x *= d; 
  y *= d; 
  z *= d; 

  return *this; 
}

template <class Type>Vector3DTmpl<Type>& Vector3DTmpl<Type>::operator/=(Type d)
{
  return operator*=(Type(1)/d);
}

template <class Type> Type Vector3DTmpl<Type>::operator&(const Vector3DTmpl& v) const
{ 
  return x*v.x+y*v.y+z*v.z;
}

template <class Type>Vector3DTmpl<Type> Vector3DTmpl<Type>::operator^(const Vector3DTmpl<Type>& v) const
{
  return Vector3DTmpl<Type>(y*v.z-z*v.y,
                            z*v.x-x*v.z,
                            x*v.y-y*v.x
                           );
}

//-------------------------------------------------------------------------------------------------
// Сравнение векторов


template <class Type> bool Vector3DTmpl<Type>::operator==(const Vector3DTmpl<Type>& v) const
{
  return (x==v.x) && (y==v.y) && (z==v.z);
}

template <class Type>bool Vector3DTmpl<Type>::operator!=(const Vector3DTmpl<Type>& v) const
{
  return (x!=v.x) || (y!=v.y) || (z!=v.z);
}

//-------------------------------------------------------------------------------------------------
// Потоковый ввод-вывод

template <class Type> ostream& operator<<(ostream& os,const Vector3DTmpl<Type>& v)
{
  os<<'('<<v.x<<';'<<v.y<<';'<<v.z<<')';
  return os;
}

template <class Type> istream& operator>>(istream& is,Vector3DTmpl<Type>& v)
{
  is>>v.x>>v.y>>v.z;
  return is;
}

//-------------------------------------------------------------------------------------------------
// "Вынесенные" операции над векторами

template <class Type>Type angle(const Vector3DTmpl<Type>& a,const Vector3DTmpl<Type>& b)
{
        Type angle=acos((a&b)/sqrt(a.qlen()*b.qlen()));
        return _isnan(angle)?Type(0):angle;
}

template <class Type> Vector3DTmpl<Type> abs(const Vector3DTmpl<Type>& v)
{ 
        return Vector3DTmpl<Type>(v.x<Type(0)?-v.x:v.x,
                                  v.y<Type(0)?-v.y:v.y,
                                  v.z<Type(0)?-v.z:v.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Прибамбасы к Vector4DTmpl
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// Потоковый ввод-вывод

template <class Type> ostream& operator<<(ostream& os,const Vector4DTmpl<Type>& v)
{
        os<<'('<<v.x<<';'<<v.y<<';'<<v.z<<';'<<v.w<<')';
        return os;
}

template <class Type> istream& operator>>(istream& is,Vector4DTmpl<Type>& v)
{
        is>>v.x>>v.y>>v.z>>v.w;
        return is;
}

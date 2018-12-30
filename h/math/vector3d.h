#ifndef __MATH_VECTOR_3D_H__
#define __MATH_VECTOR_3D_H__

#pragma warning (disable : 4244) //double to float

#include <math.h>
#include <float.h>

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Vector3DTmpl
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  #pragma pack(push,1)

  template <class Type> class Vector3DTmpl
  {
    public:
          Type x,y,z;

  // инициализация
          Vector3DTmpl (void)                              {}
          Vector3DTmpl (Type d)                            : x(d) ,   y (d),    z(d)     {}
          Vector3DTmpl (const Type* d)                     : x(d[0]), y (d[1]), z(d[1])  {}
          Vector3DTmpl (Type _x,Type _y,Type _z=0)         : x(_x),   y (_y),   z(_z)    {}               

  // операторы
          Vector3DTmpl  operator + () const                 { return *this; }
          Vector3DTmpl  operator - () const                 { return Vector3DTmpl<Type>(-x,-y,-z); }
                                    
          Vector3DTmpl& operator += (const Vector3DTmpl& v);
          Vector3DTmpl& operator -= (const Vector3DTmpl& v);
          Vector3DTmpl& operator *= (const Vector3DTmpl& v);// покомпонентное умножение
          Vector3DTmpl& operator /= (const Vector3DTmpl& v);
          Vector3DTmpl& operator *= (Type d);
          Vector3DTmpl& operator /= (Type d);

          Type          operator &  (const Vector3DTmpl&) const;     // Скалярное умножение (шоб нихто не догадался)
          Vector3DTmpl  operator ^  (const Vector3DTmpl&) const;     // векторное произведение


          operator      Type*       () const                { return (Type*)this; }

          bool          operator == (const Vector3DTmpl& v) const;      
          bool          operator != (const Vector3DTmpl& v) const;

          Vector3DTmpl  operator +  (const Vector3DTmpl& v) const { return Vector3DTmpl(x+v.x,y+v.y,z+v.z); }
          Vector3DTmpl  operator -  (const Vector3DTmpl& v) const { return Vector3DTmpl(x-v.x,y-v.y,z-v.z); }
          Vector3DTmpl  operator *  (const Vector3DTmpl& v) const { return Vector3DTmpl(x*v.x,y*v.y,z*v.z); }
          Vector3DTmpl  operator /  (const Vector3DTmpl& v) const { return Vector3DTmpl(x/v.x,y/v.y,z/v.z); }
          
          static Vector3DTmpl  operator *  (const Vector3DTmpl& v,Type d) { return Vector3DTmpl(v) *= d; }
          static Vector3DTmpl  operator *  (Type d,const Vector3DTmpl& v) { return Vector3DTmpl(v) *= d; }
          static Vector3DTmpl  operator /  (const Vector3DTmpl& v,Type d) { return Vector3DTmpl(v) /= d; }

  // Вывод/Ввод информации
          static
          ostream&      operator << (ostream&,const Vector3DTmpl&);
          static
          istream&      operator >> (istream&,Vector3DTmpl&);

          Type            qlen      (void) const                  { return x*x+y*y+z*z;       }
          Type            length    (void) const                  { return sqrt(x*x+y*y+z*z); }

          Vector3DTmpl&   normalize (void)                        { return operator/=(length()); }
          Vector3DTmpl&   abs       (void);      // покомпонентный модуль
  };

  #pragma pack(pop)

  template <class Type> 
           Type  dot  (const Vector3DTmpl<Type>& a,const Vector3DTmpl<Type>& b) 
           { return a&b; }

  template <class Type> 
           Type  cross (const Vector3DTmpl<Type>& a,const Vector3DTmpl<Type>& b) 
           { return a^b; }

  template <class Type> 
           Type   length (const Vector3DTmpl<Type>& v)
           { return v.length (); }

  template <class Type> 
           Vector3DTmpl<Type>&  normalize (Vector3DTmpl<Type>& v)
           { return v.normalize (); }

  template <class Type> 
           Vector3DTmpl<Type>   GetNormal (const Vector3DTmpl<Type>& src)
           { return Vector3DTmpl<Type> (src).normalize (); }

  template <class Type> 
           void   GetNormal (const Vector3DTmpl<Type>& src,Vector3DTmpl<Type>& dest)
           { dest = src; dest.normalize (); }

  template <class Type> 
           Type   angle     (const Vector3DTmpl<Type>& a,const Vector3DTmpl<Type>& b);

  template <class Type>
           Vector3DTmpl<Type> abs   (const Vector3DTmpl<Type>& v);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Vector4DTmpl - очччень частный потомок от Vector3DTmpl
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  template <class Type> class Vector4DTmpl: public Vector3DTmpl<Type>
  {
    public:
          Type w;
  // инициализация
          Vector4DTmpl (void)                                        {}
          Vector4DTmpl (Type d):Vector3DTmpl(d), w (Type (0))        {}
          Vector4DTmpl (const Type* d):Vector3DTmpl(d), w (Type (0)) {}
          Vector4DTmpl (Type _x,Type _y,Type _z=0,Type _w=0)
                                  :Vector3DTmpl<Type>(_x,_y,_z), w (_w)    {}
          Vector4DTmpl (const Vector3DTmpl<Type>& v)
                                  :Vector3DTmpl<Type>(v),w (Type (0))      {}

          bool            operator == (const Vector4DTmpl& v) const    { return Vector3DTmpl<Type>::operator==(v) && w==v.w; }
          bool            operator != (const Vector4DTmpl& v) const    { return Vector3DTmpl<Type>::operator!=(v) || w!=v.w; }

  // Вывод/Ввод информации
          static ostream& operator << (ostream&,const Vector4DTmpl&);
          static istream& operator >> (istream&,Vector4DTmpl&);
  };

  // определения инлайнов
  #include <math\vector3d.inl>

typedef   Vector3DTmpl<float>   vector3d_t;
typedef   Vector3DTmpl<float>   vec3_t;

typedef   Vector3DTmpl<float>   float3;
typedef   Vector3DTmpl<double>  double3;
typedef   Vector3DTmpl<float>   vector3f;

typedef   Vector4DTmpl<float>   float4;
typedef   Vector4DTmpl<double>  double4;

#endif
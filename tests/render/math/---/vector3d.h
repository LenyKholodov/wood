#pragma once

#pragma pack(push,1)

//#include <iostream>

namespace vector
{

//using std::ostream;

///////////////////////////////////////////////////////////////////////////////////////////////////
// vec3
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class type> class vec3
{
  public:
        Type x,y,z;

        vec3 ()                                   {}
        vec3 (Type d)                            : x(d) ,   y (d),    z(d)     {}
        vec3 (Type _x,Type _y,Type _z)           : x(_x),   y (_y),   z(_z)    {}               

        vec3  operator + () const                 { return *this; }
        vec3  operator - () const                 { return vec3<Type>(-x,-y,-z); }
                                  
        vec3& operator += (const vec3& v);
        vec3& operator -= (const vec3& v);
        vec3& operator *= (const vec3& v);
        vec3& operator /= (const vec3& v);
        vec3& operator *= (Type d);
        vec3& operator /= (Type d);

        Type  operator &  (const vec3&) const;
        vec3  operator ^  (const vec3&) const;

        operator      Type*       () const                { return (Type*)this; }

        bool          operator == (const vec3& v) const;      
        bool          operator != (const vec3& v) const;

        vec3  operator +  (const vec3& v) const { return vec3(x+v.x,y+v.y,z+v.z); }
        vec3  operator -  (const vec3& v) const { return vec3(x-v.x,y-v.y,z-v.z); }
        vec3  operator *  (const vec3& v) const { return vec3 (x*v.x,y*v.y,z*v.z); }
        vec3  operator /  (const vec3& v) const { return vec3(x/v.x,y/v.y,z/v.z); }
        
        static vec3  operator *  (const vec3& v,Type d) { return vec3(v) *= d; }
        static vec3  operator *  (Type d,const vec3& v) { return vec3(v) *= d; }
        static vec3  operator /  (const vec3& v,Type d) { return vec3(v) /= d; }

//        static
//        ostream&      operator << (ostream&,const vec3&);

        Type            qlen      (void) const                  { return x*x+y*y+z*z;       }
        Type            length    (void) const                  { return sqrt(x*x+y*y+z*z); }

        vec3&   normalize (void)                        { return operator/=(length()); }
        vec3&   abs       (void);
};

template <class Type> 
         Type  dot  (const vec3<Type>& a,const vec3<Type>& b) 
         { return a&b; }

template <class Type> 
         Type  cross (const vec3<Type>& a,const vec3<Type>& b) 
         { return a^b; }

template <class Type> 
         Type   length (const vec3<Type>& v)
         { return v.length (); }

template <class Type> 
         vec3<Type>&  normalize (vec3<Type>& v)
         { return v.normalize (); }

template <class Type> 
         vec3<Type>   GetNormal (const vec3<Type>& src)
         { return vec3<Type> (src).normalize (); }

template <class Type> 
         void   GetNormal (const vec3<Type>& src,vec3<Type>& dest)
         { dest = src; dest.normalize (); }

template <class Type> 
         Type   angle     (const vec3<Type>& a,const vec3<Type>& b);

template <class Type>
         vec3<Type> abs   (const vec3<Type>& v);

#include "vector3d.inl"
};

#pragma pack(pop)

typedef vector::vec3<float>   float3;
typedef vector::vec3<double>  double3;
typedef vector::vec3<float>   vec3f;
typedef vector::vec3<float>   vec3d;
//typedef vector::vec3<float>   vec3;

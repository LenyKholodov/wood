#pragma once

#include "vector.h"

namespace math
{

////////////////////////////////////////////////////////////////////////////////////////////
///Vector4
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,class base=vec_base<type,4> >
class vec4: public vec<type,4,base>
{
  typedef vec<type,4,base> vector;
  public:
                                       vec4       () {}
                                       vec4       (type x1,type x2,type x3,type x4=type(0));
                                       vec4       (type a);
    template <class t1,class b1>       vec4       (const vec<t1,4,b1>& v): vector (v) {}

    template <class t1,class b1> vec4& operator = (const vec<t1,4,b1>& v) { return (vec4&)vector::operator = (v); }
};

#pragma pack(push,1)

template <class type> struct vec_xyzw
{
  type x,y,z,w;

             type& operator [] (int i) { return (&x)[i]; }
       const type& operator [] (int i) const { return (&x)[i]; }
};

#pragma pack(pop)

#include "vec4.inl"

};

typedef math::vec4<float,math::vec_xyzw<float> >                    vec4f;
typedef math::vec4<double,math::vec_xyzw<double> >                  vec4d;
typedef math::vec4<int,math::vec_xyzw<int> >                        vec4i;
typedef math::vec4<unsigned int,math::vec_xyzw<unsigned int> >      vec4ui;
typedef math::vec4<short,math::vec_xyzw<short> >                    vec4s;
typedef math::vec4<unsigned short,math::vec_xyzw<unsigned short> >  vec4us;
typedef math::vec4<char,math::vec_xyzw<char> >                      vec4b;
typedef math::vec4<unsigned char,math::vec_xyzw<unsigned char> >    vec4ub;

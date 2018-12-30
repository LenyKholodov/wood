#pragma once
#include "vector.h"

namespace math
{

#pragma pack(push,1)

////////////////////////////////////////////////////////////////////////////////////////////
///Vector3
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,class base=vec_base<type,3> >
class vec3: public vec<type,3,base>
{
  typedef vec<type,3,base> vector;
  public:
                                       vec3       () {}
                                       vec3       (type x1,type x2,type x3);
                                       vec3       (type a);
    template <class t1,class b1>       vec3       (const vec<t1,3,b1>& v): vector (v) {}

    template <class t1,class b1> vec3& operator = (const vec<t1,3,b1>& v) { return (vec3&)vector::operator = (v); }
};

template <class type> struct vec_xyz
{
  type x,y,z;

             type& operator [] (int i) { return (&x)[i]; }
       const type& operator [] (int i) const { return (&x)[i]; }
};

#pragma pack(pop)

#include "vec3.inl"

};

typedef math::vec3<float,math::vec_xyz<float> >                    vec3f;
typedef math::vec3<double,math::vec_xyz<double> >                  vec3d;
typedef math::vec3<int,math::vec_xyz<int> >                        vec3i;
typedef math::vec3<unsigned int,math::vec_xyz<unsigned int> >      vec3ui;
typedef math::vec3<short,math::vec_xyz<short> >                    vec3s;
typedef math::vec3<unsigned short,math::vec_xyz<unsigned short> >  vec3us;
typedef math::vec3<char,math::vec_xyz<char> >                      vec3b;
typedef math::vec3<unsigned char,math::vec_xyz<unsigned char> >    vec3ub;



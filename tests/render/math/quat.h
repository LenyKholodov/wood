#pragma once

#include "matrix.h"
#include "vec3.h"
#include "angles.h"

namespace math
{

#pragma pack (push,1)

/////////////////////////////////////////////////////////////////////////////////////////////
///Кватернион
/////////////////////////////////////////////////////////////////////////////////////////////
template <typename type> class quat
{
  public:
                type   x,y,z,w;

////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
////////////////////////////////////////////////////////////////////////////////////////////
                                 quat           ();
                                 quat           (type x,type y,type z,type w = 1);
     template <class t1,
               class t2,
               class b2>         quat           (t1 angle,const vec3<t2,b2>&);
     template <class t1>         quat           (const quat<t1>&);
     template <class t1>         quat           (const matrix<t1,4>&);
     template <class t1>         quat           (const EulerAngle<t1>&);

////////////////////////////////////////////////////////////////////////////////////////////
///Индексирование
////////////////////////////////////////////////////////////////////////////////////////////
                          type&  operator []    (int i)       { return (&x) [i]; }
                    const type&  operator []    (int i) const { return (&x) [i]; }

////////////////////////////////////////////////////////////////////////////////////////////
///Унарные операции
////////////////////////////////////////////////////////////////////////////////////////////
                          quat   operator -     () const    { return quat (-x,-y,-z,w); }
                          quat&  operator +     () const    { return *this; }
                          type   operator !     () const    { return length (); }

////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
     template <class t1> quat&   operator   =   (const quat<t1>&);
     template <class t1> quat&   operator   =   (const matrix<t1,4>&);
     template <class t1> quat&   operator   =   (const EulerAngle<t1>&);

////////////////////////////////////////////////////////////////////////////////////////////
///Основные арифметические операции
////////////////////////////////////////////////////////////////////////////////////////////
     template <class t1> quat&   operator  +=   (const quat<t1>&);
     template <class t1> quat&   operator  -=   (const quat<t1>&);
     template <class t1> quat&   operator  *=   (const quat<t1>&);
                         quat&   operator  *=   (const type&);
                         quat&   operator  /=   (const type&);
     
     template <class t1> quat    operator   +   (const quat<t1>& q) const      { return quat (*this) += q; }
     template <class t1> quat    operator   -   (const quat<t1>& q) const      { return quat (*this) -= q; }
     template <class t1> quat    operator   *   (const quat<t1>& q) const      { return quat (*this) *= q; }
                         quat    operator   *   (const type& a)     const      { return quat (*this) *= a; }
     friend              quat    operator   *   (const type& a,const quat& q)  { return quat (q) *= a; }
                         quat    operator   /   (const type& a)     const      { return quat (*this) /= a; }

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между кватернионами
////////////////////////////////////////////////////////////////////////////////////////////
     template <class t1> bool    operator  ==   (const quat<t1>&) const;
     template <class t1> bool    operator  !=   (const quat<t1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные функции
////////////////////////////////////////////////////////////////////////////////////////////
                         type    length         () const      { return sqrt (x*x + y*y + z*z + w*w); }
                         void    normalize      ()            { *this /= length (); }

    friend               type    length         (const quat& q) { return q.norm (); }
    friend               quat    normal         (const quat& q) { quat res = q; return res.normalize (), res; }

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования
////////////////////////////////////////////////////////////////////////////////////////////
                         void    toMatrix       (matrix<type,4>&) const;
                         void    toEuler        (EulerAngle<type>&) const;

    friend               matrix<type,4>   toMatrix (const quat& q) { matrix<type,4> m; return q.toMatrix (m), m; }
    friend               EulerAngle<type> toEuler  (const quat& q) { EulerAngle<type> a; return q.toEuler (a), a; }

////////////////////////////////////////////////////////////////////////////////////////////
///Поворот вектора
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1>
                         vec3<t1,b1> operator * (const vec3<t1,b1>&) const;
                         
////////////////////////////////////////////////////////////////////////////////////////////
///Вывод в поток
////////////////////////////////////////////////////////////////////////////////////////////
                         ostream& print         (ostream&) const;
     friend              ostream& operator <<   (ostream& os,const quat& q) { return q.print (os); }
};

#pragma pack (pop)

template <class t1,class t2,class t3> 
bool equal (const quat<t1>&,const quat<t2>&,const t3& eps);

template <class t1,class t2> 
bool equal (const quat<t1>& a,const quat<t2>& b)
{
  return equal (a,b,EPS);
}

#include "quat.inl"

}

typedef math::quat<float>          quatf;
typedef math::quat<double>         quatd;
typedef math::quat<int>            quati;
typedef math::quat<unsigned int>   quatui;
typedef math::quat<short>          quats;
typedef math::quat<unsigned short> quatus;
typedef math::quat<char>           quatb;
typedef math::quat<unsigned char>  quatub;

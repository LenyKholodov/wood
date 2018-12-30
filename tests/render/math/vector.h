#pragma once

#include <iostream>
#include <math.h>

#include "defs.h"

#pragma inline_depth (255)
#pragma inline_recursion (on)
#pragma auto_inline (on)

namespace math
{

using std::ostream;

#pragma pack (push,1)

template <class type,unsigned int size> struct vec_base
{
  public:
              type& operator [] (int i)       { return x [i]; }
        const type& operator [] (int i) const { return x [i]; }

  private:
    type x [size];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//vec
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class type,const unsigned int size,class base=vec_base<type,size> >
class vec: public base
{
  public:
////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
////////////////////////////////////////////////////////////////////////////////////////////
                                        vec         () {}
    template <class t1,class b1>        vec         (const vec<t1,size,b1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Унарный +,-, длина (!)
////////////////////////////////////////////////////////////////////////////////////////////
                                 vec    operator +  () const        { return *this; }
                                 vec    operator -  () const;
                                 type   operator !  () const        { return length (); }

////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> vec&   operator =  (const vec<t1,size,b1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Бинарные основные операции
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> vec&   operator += (const vec<t1,size,b1>&);
    template <class t1,class b1> vec&   operator -= (const vec<t1,size,b1>&);
    template <class t1,class b1> vec&   operator *= (const vec<t1,size,b1>&);
    template <class t1,class b1> vec&   operator /= (const vec<t1,size,b1>&);

                                 vec&   operator *= (const type&);
                                 vec&   operator /= (const type&);

    template <class t1,class b1> vec    operator +  (const vec<t1,size,b1>& v) const  { return vec (*this) += v; }
    template <class t1,class b1> vec    operator -  (const vec<t1,size,b1>& v) const  { return vec (*this) -= v; }
    template <class t1,class b1> vec    operator *  (const vec<t1,size,b1>& v) const  { return vec (*this) *= v; }
    template <class t1,class b1> vec    operator /  (const vec<t1,size,b1>& v) const  { return vec (*this) /= v; }
                                 vec    operator *  (const type& b)            const  { return vec (*this) *= b; }
    friend                       vec    operator *  (const type& b,const vec& a)      { return vec (a) *= b; }
                                 vec    operator /  (const type& b)            const  { return vec (*this) /= b; }

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между векторами           
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> bool   operator == (const vec<t1,size,b1>&) const;      
    template <class t1,class b1> bool   operator != (const vec<t1,size,b1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Векторное, скалярное произведение
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> type   operator &  (const vec<t1,size,b1>&) const; //dot product
    template <class t1,class b1> vec    operator ^  (const vec<t1,size,b1>&) const; //cross product

////////////////////////////////////////////////////////////////////////////////////////////
///Операторы индексирования - должны присутствовать в классе base
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов
////////////////////////////////////////////////////////////////////////////////////////////
    template <class b1>                 operator vec<type,size,b1>& ()    { return *(vec<type,b1,size>*)this; }
    template <class b1>                 operator const vec<type,size,b1>& () const { return *(vec<type,b1,size>*)this; }
             
////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные функции
////////////////////////////////////////////////////////////////////////////////////////////
                                 type   length      () const              { return sqrt (qlen ()); }
                                 type   qlen        () const;
                                 vec    abs         ();
                                 void   normalize   ()                    { operator /= (length ()); }
                                 ostream& print     (ostream&) const;

    friend                       type   length      (const vec& v)        { return v.length (); } 
    friend                       vec    normal      (const vec& v)        { vec res = v; return res.normalize (), res ; }
    friend                       vec    abs         (const vec& v)        { return v.abs (); }

////////////////////////////////////////////////////////////////////////////////////////////
///Вывод вектора в поток
////////////////////////////////////////////////////////////////////////////////////////////
    friend                       ostream& operator << (ostream& os,const vec& v) { return v.print (os); }
};

#pragma pack(pop)

template <class t1,class b1,class t2,class b2,unsigned int size> 
t1 dot (const vec<t1,size,b1>& a,const vec<t2,size,b2>& b) 
{
  return a & b;
}

template <class t1,class b1,class t2,class b2,unsigned int size> 
vec<t1,size,b1> cross (const vec<t1,size,b1>& a,const vec<t2,size,b2>& b)
{
  return a ^ b;
}

template <class t1,class b1,class t2,class b2,unsigned int size,class t3> 
bool equal (const vec<t1,size,b1>&,const vec<t2,size,b2>&,const t3& eps);

template <class t1,class b1,class t2,class b2,unsigned int size> 
bool equal (const vec<t1,size,b1>& a,const vec<t2,size,b2>& b)
{
  return equal (a,b,EPS);
}

#include "vector.inl"

};


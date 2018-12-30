#ifndef __QUATERNION__
#define __QUATERNION__

#include <math\matrix.h>
#include <math\euler.h>

const  float DELTA = 0.001f;
struct  EulerAngle;

/////////////////////////////////////////////////////////////////////////////////////////////
///Кватернион
/////////////////////////////////////////////////////////////////////////////////////////////
template <typename Type> class   QuaternionTmpl
{
  public:
                Type   x,y,z,w;

                QuaternionTmpl      ();
                QuaternionTmpl      (const MatrixTmpl<Type>&);
                QuaternionTmpl      (const Vector3DTmpl<Type>& v,Type angle);
                QuaternionTmpl      (const EulerAngle&);
                QuaternionTmpl      (Type x,Type y,Type z,Type w = 1);

     MatrixTmpl<Type> toMatrix      () const;
     void             toMatrix      (MatrixTmpl<Type>&) const;

     EulerAngle       toAngle       () const;
     void             toAngle       (EulerAngle&) const;

     QuaternionTmpl& operator  =    (const MatrixTmpl<Type>&);
     QuaternionTmpl& operator  =    (const EulerAngle&);
     QuaternionTmpl  operator  -    () const    { return QuaternionTmpl (-x,-y,-z,w); } //not -w!

     QuaternionTmpl& operator  +=   (const QuaternionTmpl&);
     QuaternionTmpl& operator  -=   (const QuaternionTmpl&);
     QuaternionTmpl& operator  *=   (const QuaternionTmpl&);
     QuaternionTmpl& operator  *=   (Type);
     QuaternionTmpl& operator  /=   (Type);

     bool            operator  ==   (const QuaternionTmpl&) const;
     bool            operator  !=   (const QuaternionTmpl&) const;

     Type            length         () const;    //не совсем верно - это норма кват. в 4D пространстве
     Type            len            () const    { return length (); }
     void            normalize      ();
     void            norm           ()          { normalize ();     }
     QuaternionTmpl  invert         () const;

     static  Type           norm        (const QuaternionTmpl&);
     static  QuaternionTmpl normalize   (const QuaternionTmpl&);

     QuaternionTmpl         operator  + (const QuaternionTmpl&) const;
     QuaternionTmpl         operator  - (const QuaternionTmpl&) const;
     QuaternionTmpl         operator  * (const QuaternionTmpl&) const;

     static QuaternionTmpl  operator  * (const QuaternionTmpl&,Type);
     static QuaternionTmpl  operator  / (const QuaternionTmpl&,Type);

/////////////////////////////////////////////////////////////////////////////////
///Скалярное произведение кватернионов
////////////////////////////////////////////////////////////////////////////////
     Type                   operator  &    (const QuaternionTmpl&) const;

////////////////////////////////////////////////////////////////////////////////
///Вывод информации
////////////////////////////////////////////////////////////////////////////////
     static ostream&        operator  <<   (ostream&,const QuaternionTmpl&);
};

template <typename Type>
         void    slerp (QuaternionTmpl<Type>&,
                        const QuaternionTmpl<Type>&,
                        const QuaternionTmpl<Type>&,
                        float
                       );
template <class Type>
         void    lerp  (QuaternionTmpl<Type>&,
                        const QuaternionTmpl<Type>&,
                        const QuaternionTmpl<Type>&,
                        float
                       );

#include <math\quat.inl>

typedef QuaternionTmpl<float>  quat_t;

typedef QuaternionTmpl<float>  quatf;
typedef QuaternionTmpl<double> quatd;

#endif
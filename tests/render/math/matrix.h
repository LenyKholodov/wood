#pragma once

#include "vector.h"

namespace math
{

#pragma pack(push,1)

template <class type,unsigned int stride> struct column
{
  private:
    type a [1];

  public:
        type& operator [] (int i) { return a [i*stride]; }
  const type& operator [] (int i) const { return a [i*stride]; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Матрица (базис)
/////////////////////////////////////////////////////////////////////////////////////////////
template <class type,unsigned int size> class matrix
{
  typedef vec_base<type,size>     row_t;
  typedef math::column<type,size> column_t;

  public:
    typedef vec<type,size,row_t>     vec_row;  //вектор строка
    typedef vec<type,size,column_t>  vec_column; //вектор столбец

////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
////////////////////////////////////////////////////////////////////////////////////////////
                                      matrix      ();
                                      matrix      (const type& a);  //a будет записано на главной диагонали
                                      matrix      (const type*);
    template <class t1>               matrix      (const matrix<t1,size>&);

////////////////////////////////////////////////////////////////////////////////////////////
///Унарные операции
////////////////////////////////////////////////////////////////////////////////////////////
                         matrix&      operator +  () const           { return *this; }
                         matrix       operator -  () const;

////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1>  matrix&      operator =  (const matrix<t1,size>&);

////////////////////////////////////////////////////////////////////////////////////////////
///Индексирование элементов    
////////////////////////////////////////////////////////////////////////////////////////////
                         vec_row&     row         (int i)            { return m [i]; }
                   const vec_row&     row         (int i) const      { return m [i]; }
                         vec_column&  column      (int i)            { return *(vec_column*)((type*)&m + i); }
                   const vec_column&  column      (int i) const      { return *(vec_column*)((type*)&m + i); }
                         vec_row&     operator [] (int i)            { return row (i); }
                   const vec_row&     operator [] (int i) const      { return row (i); }

////////////////////////////////////////////////////////////////////////////////////////////
///Основные арифметические операции
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1>  matrix&      operator += (const matrix<t1,size>&);
    template <class t1>  matrix&      operator -= (const matrix<t1,size>&);
    template <class t1>  matrix&      operator *= (const matrix<t1,size>&);
                         matrix&      operator *= (const type&);
                         matrix&      operator /= (const type&);

    template <class t1>  matrix       operator +  (const matrix<t1,size>& a) const { return matrix(*this)+=a; }
    template <class t1>  matrix       operator -  (const matrix<t1,size>& a) const { return matrix(*this)-=a; }
    template <class t1>  matrix       operator *  (const matrix<t1,size>&)   const; 
                         matrix       operator *  (const type& a)            const { return matrix(*this)*=a; }
                         matrix       operator /  (const type& a)            const { return matrix(*this)/=a; } 

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между матрицами
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1>  bool         operator == (const matrix<t1,size>&) const;
    template <class t1>  bool         operator != (const matrix<t1,size>&) const;
               
////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные функции
////////////////////////////////////////////////////////////////////////////////////////////
                         void         transpose   ();
                         type         det         () const;
                         void         normalize   ();
                         void         invert      ();

    friend               matrix       transpose   (matrix& m)       { matriox res = m; return res.transpose (), res; }
    friend               type         det         (const matrix& m) { return m.det (); }
    friend               matrix       normal      (const matrix& m) { matrix res = m; return res.normalize (), res; }
    friend               matrix       inv         (const matrix& m) { matrix res = m; return res.invert (), res; }

////////////////////////////////////////////////////////////////////////////////////////////
///Умножение матрицы-строки на вектор-столбец
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> vec<t1,size,b1>  operator * (const vec<t1,size,b1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования в однородных координатах
////////////////////////////////////////////////////////////////////////////////////////////
    template <class t1,class b1> vec<t1,size-1,b1> TransformPoint  (const vec<t1,size-1,b1>&) const;
    template <class t1,class b1> vec<t1,size-1,b1> TransformVector (const vec<t1,size-1,b1>&) const;
    template <class t1,class b1> vec<t1,size-1,b1> operator *      (const vec<t1,size-1,b1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Вывод в поток
////////////////////////////////////////////////////////////////////////////////////////////
           ostream&     print       (ostream&) const;
    friend ostream&     operator << (ostream& os,const matrix& m) { return m.print (os); }

  private:
    template <unsigned int det_size> struct Det 
    {    
      static type det   (const matrix<type,det_size>&);
      static type minor (const matrix<type,det_size>&,int,int);
    };
    template <> struct Det<1>
    {    
      __forceinline static type det (const matrix<type,1>& m) { return m [0][0]; }
    };

  private:
    vec_row m [size];
};

#pragma pack(pop)

template <class t1,class t2,unsigned int size,class t3> 
bool equal (const matrix<t1,size>&,const matrix<t2,size>&,const t3& eps);

template <class t1,class t2,unsigned int size> 
bool equal (const matrix<t1,size>& a,const matrix<t2,size>& b)
{
  return equal (a,b,EPS);
}

#include "matrix.inl"

}

typedef math::matrix<float,4>           matrix4f;
typedef math::matrix<float,3>           matrix3f;
typedef math::matrix<float,2>           matrix2f;

typedef math::matrix<double,4>          matrix4d;
typedef math::matrix<double,3>          matrix3d;
typedef math::matrix<double,2>          matrix2d;

typedef math::matrix<int,4>             matrix4i;
typedef math::matrix<int,3>             matrix3i;
typedef math::matrix<int,2>             matrix2i;

typedef math::matrix<unsigned int,4>    matrix4ui;
typedef math::matrix<unsigned int,3>    matrix3ui;
typedef math::matrix<unsigned int,2>    matrix2ui;

typedef math::matrix<short,4>           matrix4s;
typedef math::matrix<short,3>           matrix3s;
typedef math::matrix<short,2>           matrix2s;

typedef math::matrix<unsigned short,4>  matrix4us;
typedef math::matrix<unsigned short,3>  matrix3us;
typedef math::matrix<unsigned short,2>  matrix2us;

typedef math::matrix<char,4>            matrix4b;
typedef math::matrix<char,3>            matrix3b;
typedef math::matrix<char,2>            matrix2b;

typedef math::matrix<unsigned char,4>   matrix4ub;
typedef math::matrix<unsigned char,3>   matrix3ub;
typedef math::matrix<unsigned char,2>   matrix2ub;

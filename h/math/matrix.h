#ifndef __MATRIX__
#define __MATRIX__

#include <math\vector3d.h>

#ifndef __BOUND_BASE__
#define __BOUND_BASE__

struct   boundbase_t
{
  vector3d_t    pos;
  vector3d_t    axis [3];
};

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
///Матрица (базис)
/////////////////////////////////////////////////////////////////////////////////////////////
template <class Type>  class MatrixTmpl
{
  public:
                        MatrixTmpl ();
                        MatrixTmpl (Type);

          inline MatrixTmpl&   operator +=       (const MatrixTmpl&);
          inline MatrixTmpl&   operator -=       (const MatrixTmpl&);
          inline MatrixTmpl&   operator *=       (const MatrixTmpl&);
          inline MatrixTmpl&   operator *=       ( Type );
          inline MatrixTmpl&   operator /=       ( Type );

          inline Vector4DTmpl<Type>& operator [] (int i) const { return *(Vector4DTmpl<Type>*)x [i]; }
          inline operator            Type *      ()      const { return (Type*)x; }

                 void          invert            (); //invert this

                 void          transpose         ();
                 void          identity          ();

          inline MatrixTmpl         operator +       (const MatrixTmpl&);
          inline MatrixTmpl         operator -       (const MatrixTmpl&);
          inline MatrixTmpl         operator *       (const MatrixTmpl&);
          inline MatrixTmpl         operator *       (Type);
          inline Vector3DTmpl<Type> operator *       (const Vector3DTmpl<Type>&) const;
          inline boundbase_t        operator *       (const boundbase_t&) const;

                 Vector3DTmpl<Type> TransformPoint   (const Vector3DTmpl<Type>&) const;
                 Vector3DTmpl<Type> TransformVector  (const Vector3DTmpl<Type>&) const;
                                                   
          static MatrixTmpl     translate        (const Vector3DTmpl<Type>&);
          static MatrixTmpl     scale            (const Vector3DTmpl<Type>&);
          static MatrixTmpl     rotate           (const Vector3DTmpl<Type>&,Type);

          static MatrixTmpl     translate        (Type,Type,Type);
          static MatrixTmpl     scale            (Type,Type,Type);
          static MatrixTmpl     rotate           (Type,Type,Type,Type angle);

////////////////////////////////////////////////////////////////////////////////
///Вывод информации
////////////////////////////////////////////////////////////////////////////////
          friend ostream&      operator  << (ostream&,const MatrixTmpl&);

  private:
                 Type x[4][4];
};

#include <math\matrix.inl>

typedef MatrixTmpl<float>       matrix_t;

typedef MatrixTmpl<float>       matrixf;
typedef MatrixTmpl<double>      matrixd;

typedef MatrixTmpl<float>       Matrix;

#endif
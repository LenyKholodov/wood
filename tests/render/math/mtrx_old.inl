#include <iomanip.h>

template <class Type>
inline ostream& operator << (ostream& os,const MatrixTmpl<Type>& m)
{
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
      os<<setprecision (4)<<m [i][j]<<"\t";
    os<<endl;
  }

  return os;
}

template <class Type>
inline MatrixTmpl<Type>& MatrixTmpl<Type>::
                  operator += (const MatrixTmpl<Type>& a)
{
  x [0] += a.x [0];
  x [1] += a.x [1];
  x [2] += a.x [2];
  x [3] += a.x [3];

  return *this;
}

template <class Type>
inline MatrixTmpl<Type>& MatrixTmpl<Type>::
                         operator -= (const MatrixTmpl<Type>& a)
{
  x [0] -= a.x [0];
  x [1] -= a.x [1];
  x [2] -= a.x [2];
  x [3] -= a.x [3];

  return *this;
}

template <class Type>
inline MatrixTmpl<Type>& MatrixTmpl<Type>::
                         operator *= (const MatrixTmpl<Type>& a)
{
  MatrixTmpl<Type> res( *this );
  for (int i=0;i<4;i++)
      for (int j=0;j<4;j++)
      {
        Type   sum=0;
        for (int k=0;k<4;k++)
            sum+=res.x[i][k]*a.x[k][j];
        x[i][j]=sum;
      }
  return *this;
/*  MatrixTmpl<Type> srcMatrix ( *this );
  Type*  d = (Type*)x,
      *  m = (Type*)srcMatrix.x,
      *  s = (Type*)a.x;

  d[ 0] = m[ 0] * s[ 0] + m[ 1] * s[ 4] + m[ 2] * s[ 8] + m[ 3] * s[12];
  d[ 1] = m[ 0] * s[ 1] + m[ 1] * s[ 5] + m[ 2] * s[ 9] + m[ 3] * s[13];
  d[ 2] = m[ 0] * s[ 2] + m[ 1] * s[ 6] + m[ 2] * s[10] + m[ 3] * s[14];
  d[ 3] = m[ 0] * s[ 3] + m[ 1] * s[ 7] + m[ 2] * s[11] + m[ 3] * s[15];

  d[ 4] = m[ 4] * s[ 0] + m[ 5] * s[ 4] + m[ 6] * s[ 8] + m[ 7] * s[12];
  d[ 5] = m[ 4] * s[ 1] + m[ 5] * s[ 5] + m[ 6] * s[ 9] + m[ 7] * s[13];
  d[ 6] = m[ 4] * s[ 2] + m[ 5] * s[ 6] + m[ 6] * s[10] + m[ 7] * s[14];
  d[ 7] = m[ 4] * s[ 3] + m[ 5] * s[ 7] + m[ 6] * s[11] + m[ 7] * s[15];

  d[ 8] = m[ 8] * s[ 0] + m[ 9] * s[ 4] + m[10] * s[ 8] + m[11] * s[12];
  d[ 9] = m[ 8] * s[ 1] + m[ 9] * s[ 5] + m[10] * s[ 9] + m[11] * s[13];
  d[10] = m[ 8] * s[ 2] + m[ 9] * s[ 6] + m[10] * s[10] + m[11] * s[14];
  d[11] = m[ 8] * s[ 3] + m[ 9] * s[ 7] + m[10] * s[11] + m[11] * s[15];

  d[12] = m[12] * s[ 0] + m[13] * s[ 4] + m[14] * s[ 8] + m[15] * s[12];
  d[13] = m[12] * s[ 1] + m[13] * s[ 5] + m[14] * s[ 9] + m[15] * s[13];
  d[14] = m[12] * s[ 2] + m[13] * s[ 6] + m[14] * s[10] + m[15] * s[14];
  d[15] = m[12] * s[ 3] + m[13] * s[ 7] + m[14] * s[11] + m[15] * s[15];

  return *this;  */
}

template <class Type>
inline MatrixTmpl<Type>& MatrixTmpl<Type>::
                         operator *= ( Type a )
{
  x [0] *= a;
  x [1] *= a;
  x [2] *= a;
  x [3] *= a;

  return *this;
}

template <class Type>
inline MatrixTmpl<Type>& MatrixTmpl<Type>::
                         operator /= ( Type a )
{
  float b = Type (1) / a;

  x [0] *= b;
  x [1] *= b;
  x [2] *= b;
  x [3] *= b;

  return *this;
}

template <class Type>
void MatrixTmpl<Type>::
            invert()
{
  MatrixTmpl<Type> out(1);
  for (int i=0;i<4;i++)
  {
    Type       d=x[i][i];
    if (d!=1.0)
      for (int j=0;j<4;j++)
      {
        out.x[i][j] /= d;
        x[i][j] /= d;
      }
    for (int j=0;j<4;j++)
        if (j!=i)
           if (x[j][i]!=0.0)
           {
             Type mulBy = x[j][i];
             for (int k=0;k<4;k++)
             {
               x[j][k] -= mulBy*x[i][k];
               out.x[j][k] -= mulBy*out[i][k];
             }
           }
  }
  *this=out;
}

template <class Type>
void MatrixTmpl<Type>::
     transpose ()
{
  Type t;
  for (register int i=0;i<4;i++)
      for (register int j=i;j<4;j++)
          if (i!=j)
          {
            t=x[i][j];
            x[i][j]=x[j][i];
            x[j][i]=t;
          }
}

template <class Type>
void   MatrixTmpl<Type>::
       identity ()
{
  (*this) [0] = Vector4DTmpl<Type> (1,0,0,0);
  (*this) [1] = Vector4DTmpl<Type> (0,1,0,0);
  (*this) [2] = Vector4DTmpl<Type> (0,0,1,0);
  (*this) [3] = Vector4DTmpl<Type> (0,0,0,1);
}       

template <class Type>
inline MatrixTmpl<Type> MatrixTmpl<Type>::
                        operator + (const MatrixTmpl<Type>& b)
{
  return MatrixTmpl<Type> (*this) += b;
}

template <class Type>
inline MatrixTmpl<Type> MatrixTmpl<Type>::
                        operator - (const MatrixTmpl<Type>& b)
{
  return MatrixTmpl<Type> (*this) -= b;
}

template <class Type>
inline MatrixTmpl<Type> MatrixTmpl<Type>::
                        operator * (const MatrixTmpl<Type>& b)
{
  return MatrixTmpl<Type> (*this) *= b;
}

template <class Type>
inline MatrixTmpl<Type> MatrixTmpl<Type>::
                        operator * (Type b)
{
  return MatrixTmpl<Type> (*this) *= b;
}

template <class Type>
inline Vector3DTmpl<Type> MatrixTmpl<Type>::
                      operator * (const Vector3DTmpl<Type>& v) const
{
  Vector3DTmpl<Type>      res;

  res.x       = x[0][0]*v.x + x[0][1]*v.y + x[0][2]*v.z + x[0][3];
  res.y       = x[1][0]*v.x + x[1][1]*v.y + x[1][2]*v.z + x[1][3];
  res.z       = x[2][0]*v.x + x[2][1]*v.y + x[2][2]*v.z + x[2][3];
  Type denom  = x[3][0]*v.x + x[3][1]*v.y + x[3][2]*v.z + x[3][3];

  return denom != Type (1) ? res/=denom : res;
}

template <class Type>
inline Vector3DTmpl<Type> MatrixTmpl<Type>::
                      TransformPoint (const Vector3DTmpl<Type>& v) const
{
  Vector3DTmpl<Type>      res;

  res.x       = x[0][0]*v.x + x[0][1]*v.y + x[0][2]*v.z + x[0][3];
  res.y       = x[1][0]*v.x + x[1][1]*v.y + x[1][2]*v.z + x[1][3];
  res.z       = x[2][0]*v.x + x[2][1]*v.y + x[2][2]*v.z + x[2][3];
  Type denom  = x[3][0]*v.x + x[3][1]*v.y + x[3][2]*v.z + x[3][3];

  return denom != Type (1) ? res/=denom : res;
}

template <class Type>
Vector3DTmpl<Type> MatrixTmpl<Type>::TransformVector (const Vector3DTmpl<Type>& v) const
{
  Vector3DTmpl<Type>      res;

  res.x       = x[0][0]*v.x + x[0][1]*v.y + x[0][2]*v.z;
  res.y       = x[1][0]*v.x + x[1][1]*v.y + x[1][2]*v.z;
  res.z       = x[2][0]*v.x + x[2][1]*v.y + x[2][2]*v.z;

  return res;
}

template <class Type>
inline boundbase_t MatrixTmpl<Type>::
                   operator * (const boundbase_t& b) const
{
  boundbase_t base;

  for (int i=0;i<3;i++)
  {
    vector3d_t&       res  = base.axis [i];
    const vector3d_t& v    = b.axis [i];

    res.x       = x[0][0]*v.x + x[0][1]*v.y + x[0][2]*v.z;
    res.y       = x[1][0]*v.x + x[1][1]*v.y + x[1][2]*v.z;
    res.z       = x[2][0]*v.x + x[2][1]*v.y + x[2][2]*v.z;
  }

  const vector3d_t& v      = b.pos;

  Type              denom  = x[3][0]*v.x + x[3][1]*v.y + x[3][2]*v.z + x[3][3];
  base.pos.x               = x[0][0]*v.x + x[0][1]*v.y + x[0][2]*v.z + x[0][3];
  base.pos.y               = x[1][0]*v.x + x[1][1]*v.y + x[1][2]*v.z + x[1][3];
  base.pos.z               = x[2][0]*v.x + x[2][1]*v.y + x[2][2]*v.z + x[2][3];
  base.pos                /= denom;

  return base;
}

template <class Type>
MatrixTmpl<Type>  MatrixTmpl<Type>::
                  translate        (Type x,Type y,Type z)
{
  return translate (Vector3DTmpl<Type> (x,y,z));
}

template <class Type>
MatrixTmpl<Type>  MatrixTmpl<Type>::
                  scale            (Type x,Type y,Type z)
{
  return scale (Vector3DTmpl<Type> (x,y,z));
}

template <class Type>
MatrixTmpl<Type>  MatrixTmpl<Type>::
                  rotate           (Type x,Type y,Type z,Type a)
{
  return rotate (Vector3DTmpl<Type> (x,y,z),a);
}

template <class Type>
MatrixTmpl<Type> MatrixTmpl<Type>::
                        translate (const Vector3DTmpl<Type>& loc)
{
  MatrixTmpl<Type> res (1);
  res.x[0][3]=loc.x;
  res.x[1][3]=loc.y;
  res.x[2][3]=loc.z;
  return res;
}

template <class Type>
MatrixTmpl<Type> MatrixTmpl<Type>::
                        scale ( const Vector3DTmpl<Type>& v)
{
  MatrixTmpl<Type> res (1);
  res.x[0][0] = v.x;
  res.x[1][1] = v.y;
  res.x[2][2] = v.z;
  return res;
}

template <class Type>
MatrixTmpl<Type> MatrixTmpl<Type>::
                        rotate (const Vector3DTmpl<Type>& v,Type angle)
{
  MatrixTmpl<Type> a(1);
  Type cosine = cos (angle);
  Type sine   = sin (angle);

  a.x[0][0]=v.x*v.x+(1-v.x*v.x)*cosine;
  a.x[1][0]=v.x*v.y*(1-cosine)+v.z*sine;
  a.x[2][0]=v.x*v.z*(1-cosine)-v.y*sine;
  a.x[3][0]=0;

  a.x[0][1]=v.x*v.y*(1-cosine)-v.z*sine;
  a.x[1][1]=v.y*v.y+(1-v.y*v.y)*cosine;
  a.x[2][1]=v.y*v.z*(1-cosine)+v.x*sine;
  a.x[3][1]=0;

  a.x[0][2]=v.x*v.z*(1-cosine)+v.y*sine;
  a.x[1][2]=v.y*v.z*(1-cosine)-v.x*sine;
  a.x[2][2]=v.z*v.z+(1-v.z*v.z)*cosine;
  a.x[3][2]=0;

  a.x[0][3]=0;
  a.x[1][3]=0;
  a.x[2][3]=0;
  a.x[3][3]=1;

  return a;
}

template <class Type>
MatrixTmpl<Type>::MatrixTmpl (Type v)
{
  (*this) [0] = Vector4DTmpl<Type> (v,0,0,0);
  (*this) [1] = Vector4DTmpl<Type> (0,v,0,0);
  (*this) [2] = Vector4DTmpl<Type> (0,0,v,0);
  (*this) [3] = Vector4DTmpl<Type> (0,0,0,v);  
}

template <class Type>
MatrixTmpl<Type>::MatrixTmpl ()
{    }


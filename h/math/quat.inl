#pragma warning (disable : 4244)

template <typename Type>
ostream&  operator  << (ostream& os,const QuaternionTmpl<Type>& q)
{
  os<<q.w<<" ("<<q.x<<','<<q.y<<','<<q.z<<')';

  return os;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator  =  (const MatrixTmpl<Type>& mt)
{
  Type     tr, s, q[4];
  int      i, j, k;
  MatrixTmpl<Type> m = mt;

  m.transpose (); //!!!

  int nxt[3] = {1, 2, 0};

  tr = m[0][0] + m[1][1] + m[2][2];

  if (tr > 0.0)
  {
    s = sqrt (tr + 1.0);
    w = s / 2.0;
    s = 0.5 / s;
    x = (m[1][2] - m[2][1]) * s;
    y = (m[2][0] - m[0][2]) * s;
    z = (m[0][1] - m[1][0]) * s;
  }
  else
  {
    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];

    s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);

    q[i] = s * 0.5;

    if (s != 0.0) s = 0.5 / s;

    q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator = (const EulerAngle& a)
{
  Type  ex = Deg2Rad (a.pitch) / (Type)2.0;
  Type  ey = Deg2Rad (a.yaw  ) / (Type)2.0;
  Type  ez = Deg2Rad (a.bank ) / (Type)2.0;

  Type  cr = cos(ex);
  Type  cp = cos(ey);
  Type  cy = cos(ez);

  Type  sr = sin(ex);
  Type  sp = sin(ey);
  Type  sy = sin(ez);

  Type  cpcy = cp * cy;
  Type  spsy = sp * sy;

  w = cr * cpcy + sr * spsy;

  x = sr * cpcy - cr * spsy;
  y = cr * sp * cy + sr * cp * sy;
  z = cr * cp * sy - sr * sp * cy;

  normalize();  

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator  += (const QuaternionTmpl<Type>& q)
{
  x += q.x;
  y += q.y;
  z += q.z;
  w += q.w;

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator  -= (const QuaternionTmpl<Type>& q)
{
  x -= q.x;
  y -= q.y;
  z -= q.z;
  w -= q.w;

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>&  QuaternionTmpl<Type>::operator  *=  (const QuaternionTmpl<Type>& q)
{
  Vector3DTmpl<Type> v1 (x,y,z);
  Vector3DTmpl<Type> v2 (q.x,q.y,q.z);
  Vector3DTmpl<Type> res = v2*w + v1*q.w + (v1^v2);

  w = w*q.w - (x*q.x+y*q.y+z*q.z);
  x = res.x;
  y = res.y;
  z = res.z;

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>   QuaternionTmpl<Type>::operator  +  (const QuaternionTmpl<Type>& q) const
{
  return QuaternionTmpl<Type> (x+q.x,
                               y+q.y,
                               z+q.z,
                               w+q.w
                              );
}

template <typename Type>
QuaternionTmpl<Type>   QuaternionTmpl<Type>::operator  -  (const QuaternionTmpl<Type>& q)  const
{
  return QuaternionTmpl<Type> (x-q.x,
                               y-q.y,
                               z-q.z,
                               w-q.w
                              );
}

template <typename Type>
QuaternionTmpl<Type>   QuaternionTmpl<Type>::operator  *  (const QuaternionTmpl<Type>& q) const
{
  return QuaternionTmpl<Type> (*this) *= q;
}

template <typename Type>
QuaternionTmpl<Type>  operator  *  (const QuaternionTmpl<Type>& q,Type v)
{
  return QuaternionTmpl<Type> (q) *= v;
}

template <typename Type>
QuaternionTmpl<Type>  QuaternionTmpl<Type>::operator  /  (const QuaternionTmpl<Type>& q,Type _x)
{
  Type  v = 1.0f / _x;        

  return QuaternionTmpl<Type> (q) *= v;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator  *= (Type _x)
{
  x *= _x;
  y *= _x;
  z *= _x;
  w *= _x;

  return *this;
}

template <typename Type>
QuaternionTmpl<Type>& QuaternionTmpl<Type>::operator  /= (Type v)
{
  Type  _x = 1.0f / v;  

  x *= _x;
  y *= _x;
  z *= _x;
  w *= _x;

  return *this;
}

template <typename Type>
Type         QuaternionTmpl<Type>::operator  &  (const QuaternionTmpl<Type>& q) const
{
  return x*q.x + y*q.y + z*q.z + w*q.w;
}

template <typename Type>
bool          QuaternionTmpl<Type>::operator  == (const QuaternionTmpl<Type>& q) const
{
  return q.x == x && q.y == y && q.z == z && q.w == w;
}

template <typename Type>
bool          QuaternionTmpl<Type>::operator  != (const QuaternionTmpl<Type>& q) const
{
  return q.x != x || q.y != y || q.z != z || q.w != w;
}

template <typename Type>
Type          QuaternionTmpl<Type>::length  () const
{
  return sqrt (x*x + y*y + z*z + w*w);
}

template <typename Type>
void          QuaternionTmpl<Type>::normalize ()
{
  Type n = sqrt (x*x + y*y + z*z + w*w);

  if (n > DELTA)
  {
    Type v = 1.0f / n;

    x      *= v;
    y      *= v;
    z      *= v;
    w      *= v;
  }
}

template <typename Type>
Type          norm         (const QuaternionTmpl<Type>& q)
{
  return sqrt (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

template <typename Type>
QuaternionTmpl<Type>    normalize    (const QuaternionTmpl<Type>& q)
{
  Type n = sqrt (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);

  if (n > DELTA)
  {
    Type v = 1.0f / n;

    return QuaternionTmpl<Type> (q.x*v,q.y*v,q.z*v,q.w*v);
  }

  return q;
}

template <typename Type>
MatrixTmpl<Type>    QuaternionTmpl<Type>::toMatrix     () const
{
  MatrixTmpl<Type> m;

  Type wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  x2 = x + x;
  y2 = y + y;
  z2 = z + z;
  xx = x * x2;   xy = x * y2;   xz = x * z2;
  yy = y * y2;   yz = y * z2;   zz = z * z2;
  wx = w * x2;   wy = w * y2;   wz = w * z2;

  m[0][0]=1.0f-(yy+zz); m[0][1]=xy-wz;        m[0][2]=xz+wy;
  m[1][0]=xy+wz;        m[1][1]=1.0f-(xx+zz); m[1][2]=yz-wx;
  m[2][0]=xz-wy;        m[2][1]=yz+wx;        m[2][2]=1.0f-(xx+yy);

  m[0][3] = m[1][3] = m[2][3] = 0;
  m[3][0] = m[3][1] = m[3][2] = 0;
  m[3][3] = 1;

  return m;
}

template <typename Type>
void  QuaternionTmpl<Type>::toMatrix (MatrixTmpl<Type>& m) const
{
  Type wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  x2 = x + x;
  y2 = y + y;
  z2 = z + z;
  xx = x * x2;   xy = x * y2;   xz = x * z2;
  yy = y * y2;   yz = y * z2;   zz = z * z2;
  wx = w * x2;   wy = w * y2;   wz = w * z2;

  m[0][0]=1.0f-(yy+zz); m[0][1]=xy-wz;        m[0][2]=xz+wy;
  m[1][0]=xy+wz;        m[1][1]=1.0f-(xx+zz); m[1][2]=yz-wx;
  m[2][0]=xz-wy;        m[2][1]=yz+wx;        m[2][2]=1.0f-(xx+yy);

  m[0][3] = m[1][3] = m[2][3] = 0;
  m[3][0] = m[3][1] = m[3][2] = 0;
  m[3][3] = 1;
}

template <typename Type>
EulerAngle QuaternionTmpl<Type>::toAngle () const
{
  return EulerAngle (*this);
}

template <typename Type>
void  QuaternionTmpl<Type>::toAngle (EulerAngle& a) const
{
  Type   w2  = w * w,
         x2  = x * x,
         y2  = y * y,
         z2  = z * z;

  a.pitch     = Rad2Deg (atan (2.0 * (y*z + x*w)/(-x2 - y2 + z2 + w2)));
  a.yaw       = Rad2Deg (asin (-2.0 * (x*z - y*w)));  
  a.bank      = Rad2Deg (atan (2.0 * (x*y + z*w)/(x2 - y2 - z2 + w2)));
}

template <typename Type>
QuaternionTmpl<Type>    QuaternionTmpl<Type>::invert       () const
{
  return -*this / norm ();
}

template <typename Type>
//QuaternionTmpl::QuaternionTmpl      (): x (0), y(0), z(0), w(1) {}
QuaternionTmpl<Type>::QuaternionTmpl      () {}

template <typename Type>
QuaternionTmpl<Type>::QuaternionTmpl      (const Vector3DTmpl<Type>& v,Type angle)
{
  Type  _sin = sin(angle * 0.5f);
  Type  len  = v.length ();
  _sin       = len > DELTA ? _sin / len : _sin; 
  
  x = _sin * v.x;
  y = _sin * v.y;
  z = _sin * v.z;
  w = cos (angle * Type (0.5));
}

template <typename Type>
QuaternionTmpl<Type>::QuaternionTmpl      (const MatrixTmpl<Type>& mt)
{
  Type     tr, s, q[4];
  int      i, j, k;
  MatrixTmpl<Type> m = mt;
  m.transpose ();

  int nxt[3] = {1, 2, 0};

  tr = m[0][0] + m[1][1] + m[2][2];

  if (tr > 0.0)
  {
    s = sqrt (tr + 1.0);
    w = s / 2.0;
    s = 0.5 / s;
    x = (m[1][2] - m[2][1]) * s;
    y = (m[2][0] - m[0][2]) * s;
    z = (m[0][1] - m[1][0]) * s;
  }
  else
  {
    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];

    s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);

    q[i] = s * 0.5;

    if (s != 0.0) s = 0.5 / s;

    q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }
}

template <typename Type>
QuaternionTmpl<Type>::QuaternionTmpl      (Type _x,Type _y,Type _z,Type _w)
           : x(_x), y (_y), z(_z), w(_w) {} 

template <typename Type>
QuaternionTmpl<Type>::QuaternionTmpl (const EulerAngle& a)
{
  Type  ex = Deg2Rad (a.pitch) / (Type)2.0;
  Type  ey = Deg2Rad (a.yaw  ) / (Type)2.0;
  Type  ez = Deg2Rad (a.bank ) / (Type)2.0;

  Type  cr = cos(ex);
  Type  cp = cos(ey);
  Type  cy = cos(ez);

  Type  sr = sin(ex);
  Type  sp = sin(ey);
  Type  sy = sin(ez);

  Type  cpcy = cp * cy;
  Type  spsy = sp * sy;

  w = cr * cpcy + sr * spsy;

  x = sr * cpcy - cr * spsy;
  y = cr * sp * cy + sr * cp * sy;
  z = cr * cp * sy - sr * sp * cy;

  normalize();
}

template <typename Type>
void    slerp (QuaternionTmpl<Type>& res,const QuaternionTmpl<Type>& q,const QuaternionTmpl<Type>& p,float t)
{

  Type p1[4];
  Type omega, cosom, sinom, scale0, scale1; //double

  cosom = q & p;

  if ( cosom < 0.0 )
  { 
    cosom = -cosom;
    p1[0] = - p.x;  p1[1] = - p.y;
    p1[2] = - p.z;  p1[3] = - p.w;
  }
  else
  {
    p1[0] = p.x;    p1[1] = p.y;
    p1[2] = p.z;    p1[3] = p.w;
  }

  if ( (1.0 - cosom) > DELTA)
  {
    omega = acos(cosom);
    sinom = sin(omega);
    scale0 = sin((1.0 - t) * omega) / sinom;
    scale1 = sin(t * omega) / sinom;
  }
  else
  {        
    scale0 = 1.0 - t;
    scale1 = t;
  }

  res.x = scale0 * q.x + scale1 * p1[0];
  res.y = scale0 * q.y + scale1 * p1[1];
  res.z = scale0 * q.z + scale1 * p1[2];
  res.w = scale0 * q.w + scale1 * p1[3];
}

template <typename Type>
void    lerp  (QuaternionTmpl<Type>& res,const QuaternionTmpl<Type>& a,const QuaternionTmpl<Type>& b,float t)
{                           
  res = a * (1.0f - t) + b * t;
}


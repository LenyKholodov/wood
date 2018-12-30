template <class type> template <class type1>
__forceinline quat<type>& quat<type>::operator += (const quat<type1>& q)
{
  for (int i=0;i<4;i++)
    (*this)[i] += q [i];

  return *this;
}

template <class type> template <class type1>
__forceinline quat<type>& quat<type>::operator -= (const quat<type1>& q)
{
  for (int i=0;i<4;i++)
    (*this)[i] -= q [i];

  return *this;
}

template <class type>
__forceinline quat<type>& quat<type>::operator *= (const type& a)
{
  for (int i=0;i<4;i++)
    (*this)[i] *= a;

  return *this;
}

template <class type>
__forceinline quat<type>& quat<type>::operator /= (const type& a)
{
  return operator *= (type (1) / a);
}

template <class type> template <class t1> 
quat<type>& quat<type>::operator *= (const quat<t1>& q)
{
  vec3<type> v1 (x,y,z);
  vec3<type> v2 (q.x,q.y,q.z);
  vec3<type> res = v2*w + v1*q.w + (v1^v2);

  x = res [0];
  y = res [1];
  z = res [2];
  w = w*q.w - (x*q.x+y*q.y+z*q.z);

  return *this;
}

template <class type>
template <class t1> 
bool quat<type>::operator == (const quat<t1>& q) const
{
  for (int i=0;i<4;i++)
    if ((*this)[i] != q [i])
      return false;

  return true;  
}

template <class type>
template <class t1> 
bool quat<type>::operator != (const quat<t1>& q) const
{
  for (int i=0;i<4;i++)
    if ((*this)[i] != q [i])
      return true;

  return false; 
}

template <class t1,class t2,class t3> 
bool equal (const quat<t1>& a,const quat<t2>& b,const t3& eps)
{
  for (int i=0;i<4;i++)
  {
    t3 d = a [i] - b [i];

    if (d > eps || d < -eps)
      return false;
  }    

  return true;  
}

template <class type>
template <class t1> 
quat<type>& quat<type>::operator = (const quat<t1>& q)
{
  for (int i=0;i<4;i++)
    (*this)[i] = q [i];

  return *this;
}

template <class type>
template <class t1> 
quat<type>& quat<type>::operator = (const EulerAngle<t1>& angle)
{
  type  ex = Deg2Rad (angle.pitch) * type (0.5);
  type  ey = Deg2Rad (angle.yaw  ) * type (0.5);
  type  ez = Deg2Rad (angle.roll ) * type (0.5);

  type  cr = cos (ex);
  type  cp = cos (ey);
  type  cy = cos (ez);

  type  sr = sin (ex);
  type  sp = sin (ey);
  type  sy = sin (ez);

  type  cpcy = cp * cy;
  type  spsy = sp * sy;

  w = cr * cpcy + sr * spsy;

  x = sr * cpcy - cr * spsy;
  y = cr * sp * cy + sr * cp * sy;
  z = cr * cp * sy - sr * sp * cy;

  normalize();    

  return *this;
}

template <class type>
template <class type1>
quat<type>& quat<type>::operator = (const matrix<type1,4>& m)
{
  type tr = m [0][0] + m [1][1] + m [2][2];

  if (tr > type (0))
  {
    type  s = sqrt (tr + type (1));
    w       = s * type (0.5);
    s       = type (0.5) / s;
    x       = (m [2][1] - m [1][2]) * s;
    y       = (m [0][2] - m [2][0]) * s;
    z       = (m [1][0] - m [0][1]) * s;
  }
  else
  {
    const int nxt [3] = {1, 2, 0};

    int i = 0;

    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;

    int j = nxt[i];
    int k = nxt[j];

    type s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + type (1));

    (*this) [i] = s * type (0.5);

    if (s != type (0)) s = type (0.5) / s;

    (*this) [3] = (m [k][j] - m [j][k]) * s;
    (*this) [j] = (m [j][i] + m [i][j]) * s;
    (*this) [k] = (m [k][i] + m [i][k]) * s;
  }

  return *this;
}

template <class type> 
template <class t1,class b1>
vec3<t1,b1> quat<type>::operator * (const vec3<t1,b1>& v) const
{
  quat res = *this * quat (v [0],v [1],v [2],0) * -(*this) / length ();

  return vec3<t1,b1> (res [0],res [1],res [2]);
}

template <class type>
void quat<type>::toMatrix (matrix<type,4>& m) const
{  
  type wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
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

template <class type>
void quat<type>::toEuler (EulerAngle<type>& angle) const
{
  type   w2  = w * w,
         x2  = x * x,
         y2  = y * y,
         z2  = z * z;

  a.pitch = Rad2Deg (atan (2.0 * (y*z + x*w)/(-x2 - y2 + z2 + w2)));
  a.yaw   = Rad2Deg (asin (-2.0 * (x*z - y*w)));  
  a.roll  = Rad2Deg (atan (2.0 * (x*y + z*w)/(x2 - y2 - z2 + w2))); 
}

template <class type> 
ostream& quat<type>::print (ostream& os) const
{
  return os<<'('<<x<<','<<y<<','<<z<<','<<w<<')';
}

template <class type> 
__forceinline quat<type>::quat (type _x,type _y,type _z,type _w)
  : x(_x), y (_y), z (_z), w (_w)
{  }

template <class type> 
template <class t1> 
__forceinline quat<type>::quat (const quat<t1>& q)
  : x (q.x), y (q.y), z (q.z), w (q.w)
{  }

template <class type> 
__forceinline quat<type>::quat ()
{  }

template <class type>
template <class type1>
quat<type>::quat (const matrix<type1,4>& m)
{
  *this = m;
}

template <class type>
template <class type1>
quat<type>::quat (const EulerAngle<type1>& angle)
{
  *this = angle;
}

template <class type>
template <class t1,class t2,class b2> 
quat<type>::quat (t1 a,const vec3<t2,b2>& v)
{
  const type EPS = 0.00001;

  type  _sin = sin (a * t1 (0.5));
  type  len  = v.length ();

  _sin       = len > EPS ? _sin / len : _sin; 
  
  x = _sin * v [0];
  y = _sin * v [1];
  z = _sin * v [2];
  w = cos (a * t1 (0.5));  
}

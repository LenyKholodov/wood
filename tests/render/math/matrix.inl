template <class type,unsigned int size>
matrix<type,size> matrix<type,size>::operator - () const
{
  matrix res;

  for (int i=0;i<size;i++)
    res [i] = -(*this)[i];

  return res;
}

template <class type,unsigned int size>
template <class t1>  
__forceinline matrix<type,size>& matrix<type,size>::operator += (const matrix<t1,size>& a)
{
  for (int i=0;i<size;i++)
    (*this)[i] += a [i];

  return *this;
}

template <class type,unsigned int size>
template <class t1>  
__forceinline matrix<type,size>& matrix<type,size>::operator -= (const matrix<t1,size>& a)
{
  for (int i=0;i<size;i++)
    (*this)[i] -= a [i];

  return *this;
}

template <class type,unsigned int size>
template <class t1>  
__forceinline matrix<type,size>& matrix<type,size>::operator *= (const matrix<t1,size>& a)
{
  matrix res;

  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++)
      res [i][j] = dot (row (i),a.column (j));

  return *this = res;
}

template <class type,unsigned int size>
__forceinline matrix<type,size>& matrix<type,size>::operator *= (const type& a)
{
  for (int i=0;i<size;i++)
    (*this)[i] *= a;  

  return *this;
}

template <class type,unsigned int size>
__forceinline matrix<type,size>& matrix<type,size>::operator /= (const type& a)
{
  return operator *= (type (1)/a);
}

template <class type,unsigned int size>
template <class t1>  
__forceinline matrix<type,size> matrix<type,size>::operator * (const matrix<t1,size>& a) const
{
  matrix res;

  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++)
      res [i][j] = dot (row (i),a.column (j));

  return res;
}

template <class type,unsigned int size>
template <class t1>
bool matrix<type,size>::operator == (const matrix<t1,size>& a) const
{
  for (int i=0;i<size;i++)
    if (a [i] != (*this)[i])
      return false;

  return true;
}

template <class type,unsigned int size>
template <class t1>
bool matrix<type,size>::operator != (const matrix<t1,size>& a) const
{
  for (int i=0;i<size;i++)
    if (a [i] != (*this)[i])
      return true;

  return false;
}

template <class t1,class t2,unsigned int size,class t3> 
bool equal (const matrix<t1,size>& a,const matrix<t2,size>& b,const t3& eps)
{
  for (int i=0;i<size;i++)
    if (!equal (a [i],b [i],eps))
      return false;

  return true;  
} 

template <class type,unsigned int size>
__forceinline void matrix<type,size>::transpose ()
{
  for (int i=1;i<size;i++)
    for (int j=i;j<size;j++)
    {
      type t     = m [i-1][j];
      m [i-1][j] = m [j][i-1];
      m [j][i-1] = t;
    }
}

template <class type,unsigned int size>
template <unsigned int det_size>
__forceinline type matrix<type,size>::Det<det_size>::det (const matrix<type,det_size>& m)
{
  type res  = type (0);

  for (int i=0;i<det_size/2;i+=2)  res += m [0][i] * minor (m,0,i);
  for (i=1;i<det_size/2;i+=2)      res -= m [0][i] * minor (m,0,i);

  return res;
}

template <class type,unsigned int size> 
template <unsigned int det_size>
__forceinline type matrix<type,size>::Det<det_size>::minor (const matrix<type,det_size>& src,int m,int n) 
{
  matrix<type,det_size-1> tmp;

  for (int i=0;i<m;i++)
  {
    for (int j=0;j<n;j++) tmp [i][j]   = src [i][j];
    j++;
    for (;j<det_size;j++) tmp [i][j-1] = src [i][j];
  }      

  i++;

  for (;i<det_size;i++)
  {
    for (int j=0;j<n;j++) tmp [i-1][j]   = src [i][j];
    j++;
    for (;j<det_size;j++) tmp [i-1][j-1] = src [i][j];
  }      

  return Det<det_size-1>::det (tmp);
}

template <class type,unsigned int size>
type matrix<type,size>::det () const
{
  return Det<size>::det (*this);
}

template <class type,unsigned int size>
void matrix<type,size>::invert ()
{
  matrix out = 1;

  const type one  = 1,
             zero = 0;

  for (int i=0;i<size;i++)
  {
    type d = m [i][i];

    if (d != one)
    {
      out.row (i) /= d;
      row (i)     /= d;
    }

    for (int j=0;j<size;j++)
      if (j != i && m [j][i] != zero)
       {
         const type a = m [j][i];

         row (j)     -= row (i) * a;
         out.row (j) -= out.row (i) * a;
       }
  }

  *this = out;
}

template <class type,unsigned int size>
template <class t1,class b1>
__forceinline vec<t1,size,b1> matrix<type,size>::operator * (const vec<t1,size,b1>& v) const
{
  vec<t1,size,b1> res;

  for (int i=0;i<size;i++)
    res [i] = dot (row (i),v);

  return res;
}

template <class type,unsigned int size>
void matrix<type,size>::normalize ()
{
  *this /= sqrt (det ());
}

template <class type,unsigned int size>
template <class t1,class b1> 
__forceinline vec<t1,size-1,b1> matrix<type,size>::TransformPoint  (const vec<t1,size-1,b1>& p) const
{
  vec<t1,size-1,b1> res;

  for (int i=0;i<size-1;i++)
    res [i] = dot ((vec<t1,size-1,b1>&)row (i),p) + row (i)[size-1];

  return res;
}

template <class type,unsigned int size>
template <class t1,class b1> 
__forceinline vec<t1,size-1,b1> matrix<type,size>::TransformVector (const vec<t1,size-1,b1>& v) const
{
  vec<t1,size-1,b1> res;

  for (int i=0;i<size-1;i++)
    res [i] = dot ((vec<t1,size-1,b1>&)row (i),v);

  return res;
}

template <class type,unsigned int size>
template <class t1,class b1> 
__forceinline vec<t1,size-1,b1> matrix<type,size>::operator * (const vec<t1,size-1,b1>& p) const
{
  return TransformPoint (p);
}

template <class type,unsigned int size>
ostream& matrix<type,size>::print (ostream& os) const
{
  for (int i=0;i<size;i++)
    os<<(*this)[i]<<endl;

  return os;
}

template <class type,unsigned int size>
__forceinline matrix<type,size>::matrix (const type& a)
{
  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++)
      if (i==j) (*this)[i][j] = a;
      else      (*this)[i][j] = 0;
}

template <class type,unsigned int size>
__forceinline matrix<type,size>::matrix (const type* array)
{
  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++,array++)
      (*this)[i][j] = *array;
}

template <class type,unsigned int size>
template <class t1>               
__forceinline matrix<type,size>& matrix<type,size>::operator = (const matrix<t1,size>& src)
{
  for (int i=0;i<size;i++)
    m [i] = src [i];

  return *this;
}

template <class type,unsigned int size>
template <class t1>               
__forceinline matrix<type,size>::matrix (const matrix<t1,size>& src)
{
  for (int i=0;i<size;i++)
    m [i] = src [i];
}

template <class type,unsigned int size>
__forceinline matrix<type,size>::matrix ()
{ }

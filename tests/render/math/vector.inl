template <class type,const unsigned int size,class base>
vec<type,size,base> vec<type,size,base>::operator - () const
{ 
  vec res;

  for (int i=0;i<size;i++)
    res [i] = -(*this)[i];

  return res;
}

template <class type,const unsigned int size,class base>
template <class type1,class base1>
vec<type,size,base>& vec<type,size,base>::operator += (const vec<type1,size,base1>& v) 
{ 
  for (int i=0;i<size;i++)
    (*this)[i] += v [i];

  return *this; 
}

template <class type,const unsigned int size,class base>
template <class type1,class base1>
vec<type,size,base>& vec<type,size,base>::operator -= (const vec<type1,size,base1>& v) 
{ 
  for (int i=0;i<size;i++)
    (*this)[i] -= v [i];

  return *this; 
}

template <class type,const unsigned int size,class base>
template <class type1,class base1>
vec<type,size,base>& vec<type,size,base>::operator *= (const vec<type1,size,base1>& v) 
{ 
  for (int i=0;i<size;i++)
    (*this)[i] *= v [i];

  return *this; 
}

template <class type,const unsigned int size,class base>
template <class type1,class base1>
vec<type,size,base>& vec<type,size,base>::operator /= (const vec<type1,size,base1>& v) 
{ 
  for (int i=0;i<size;i++)
    (*this)[i] /= v [i];

  return *this; 
}
        
template <class type,const unsigned int size,class base>
vec<type,size,base>& vec<type,size,base>::operator *= (const type& d)         
{ 
  for (int i=0;i<size;i++)
    (*this)[i] *= d;

  return *this; 
}

template <class type,const unsigned int size,class base>
vec<type,size,base>& vec<type,size,base>::operator /= (const type& d)
{
  return operator *= (type(1)/d);
}

template <class type,const unsigned int size,class base> 
template <class type1,class base1>
bool vec<type,size,base>::operator == (const vec<type1,size,base1>& v) const
{
  for (int i=0;i<size;i++)
    if ((*this)[i] != v [i])
      return false;

  return true;
}

template <class type,const unsigned int size,class base>
template <class type1,class base1>
bool vec<type,size,base>::operator != (const vec<type1,size,base1>& v) const
{
  for (int i=0;i<size;i++)
    if ((*this)[i] != v [i])
      return true;

  return false;
}

template <class t1,class b1,class t2,class b2,unsigned int size,class t3> 
bool equal (const vec<t1,size,b1>& a,const vec<t2,size,b2>& b,const t3& eps)
{
  for (int i=0;i<size;i++)
  {
    t3 d = a [i] - b [i];

    if (d > eps || d < -eps)
      return false;
  }    

  return true;  
} 

template <class type,const unsigned int size,class base> 
template <class type1,class base1> 
type vec<type,size,base>::operator & (const vec<type1,size,base1>& v) const
{ 
  type result = 0;

  for (int i=0;i<size;i++)
    result += (*this)[i] * v [i];

  return result;
}

template <class type,const unsigned int size,class base> 
template <class type1,class base1> 
vec<type,size,base> vec<type,size,base>::operator ^ (const vec<type1,size,base1>& v) const
{ 
  vec res;

  for (int i=0;i<size;i++)
    res [i] = (*this) [(i+size-2)%size] * 
              v       [(i+size-1)%size] - 
              (*this) [(i+size-1)%size] * 
              v       [(i+size-2)%size];

  return res;
}

template <class type,const unsigned int size,class base> 
type vec<type,size,base>::qlen () const
{
  type result = type (0);

  for (int i=0;i<size;i++)
    result += (*this)[i] * (*this)[i];

  return result;
}

template <class type,const unsigned int size,class base> 
vec<type,size,base>  vec<type,size,base>::abs ()
{ 
  vec res;

  for (int i=0;i<size;i++)
    res [i] = (*this)[i] < type (0) ? -(*this)[i] : (*this)[i];

  return res;
}

template <class type,const unsigned int size,class base> 
ostream& vec<type,size,base>::print (ostream& os) const
{
  os<<'(';

  for (int i=0;i<size;i++)
    if (i != size - 1) os<<(*this) [i]<<',';
    else               os<<(*this) [i];

  return os<<')';
}

template <class type,const unsigned int size,class base> 
template <class type1,class base1>
vec<type,size,base>& vec<type,size,base>::operator = (const vec<type1,size,base1>& v)
{
  for (int i=0;i<size;i++)
    (*this)[i] = v [i];

  return *this;
}

template <class type,const unsigned int size,class base> 
template <class type1,class base1>
vec<type,size,base>::vec (const vec<type1,size,base1>& v)
{
  for (int i=0;i<size;i++)
    (*this)[i] = v [i];
}

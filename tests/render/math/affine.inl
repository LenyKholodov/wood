template <class type,class base,unsigned int size> 
matrix<type,size+1> translate (const vec<type,size,base>& offset)
{
  matrix<type,size+1> res = 1;

  for (int i=0;i<size;i++)
    res [i][size] = offset [i];

  return res;  
}

template <class type>
matrix<type,4> translate (const type& x,const type& y,const type& z)
{
  return translate (vec3<type> (x,y,z));
}

template <class type,class base,unsigned int size> 
matrix<type,size+1> scale (const vec<type,size,base>& s)
{
  matrix<type,size+1> res = 1;

  for (int i=0;i<size;i++)
    res [i][i] = s [i];    

  return res;
}

template <class type>
matrix<type,4> scale (const type& x,const type& y,const type& z)
{
  return scale (vec3<type> (x,y,z));
}

template <class type,class base> 
matrix<type,4> rotate (const type& angle,const vec<type,3,base>& axis)
{
  matrix<type,4> a;

  type cosine = cos (angle);
  type sine   = sin (angle);

  a [0][0] = axis [0]*axis [0]+(1-axis [0]*axis [0])*cosine;
  a [1][0] = axis [0]*axis [1]*(1-cosine)+axis [2]*sine;
  a [2][0] = axis [0]*axis [2]*(1-cosine)-axis [1]*sine;
  a [3][0] = 0;

  a [0][1] = axis [0]*axis [1]*(1-cosine)-axis [2]*sine;
  a [1][1] = axis [1]*axis [1]+(1-axis [1]*axis [1])*cosine;
  a [2][1] = axis [1]*axis [2]*(1-cosine)+axis [0]*sine;
  a [3][1] = 0;

  a [0][2] = axis [0]*axis [2]*(1-cosine)+axis [1]*sine;
  a [1][2] = axis [1]*axis [2]*(1-cosine)-axis [0]*sine;
  a [2][2] = axis [2]*axis [2]+(1-axis [2]*axis [2])*cosine;
  a [3][2] = 0;

  a [0][3] = 0;
  a [1][3] = 0;
  a [2][3] = 0;
  a [3][3] = 1;

  return a;  
}

template <class type> 
matrix<type,4> rotate (const type& angle,const type& axis_x,const type& axis_y,const type& axis_z)
{
  return rotate (angle,vec3<type> (axis_x,axis_y,axis_z));
}

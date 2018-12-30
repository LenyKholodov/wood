template <class type,class base> 
vec3<type,base>::vec3 (type x1,type x2,type x3)
{
  (*this)[0] = x1;
  (*this)[1] = x2;
  (*this)[2] = x3;
}

template <class type,class base> 
vec3<type,base>::vec3 (type a)
{
  (*this)[0] = a;
  (*this)[1] = a;
  (*this)[2] = a;
}

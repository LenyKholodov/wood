template <class type,class base> 
vec4<type,base>::vec4 (type x1,type x2,type x3,type x4)
{
  (*this)[0] = x1;
  (*this)[1] = x2;
  (*this)[2] = x3;
  (*this)[3] = x4;
}

template <class type,class base> 
vec4<type,base>::vec4 (type a)
{
  (*this)[0] = a;
  (*this)[1] = a;
  (*this)[2] = a;
  (*this)[3] = a;
}

#pragma once

#include "defs.h"

class Vertex
{
  public:  
    template <class type,uint size,class base=math::vec_base<type,size> > 
             struct vertex { math::vec<type,size,base> v; };
    template <class type,class base>           
             struct normal { math::vec3<type,base> n; };
    template <class 

    template <class vtype> 
             static void SetVertexArray (const vtype*);

  private:
    static void SetVertexArray (const void*,size_t);
    static void SetNormalArray (const void*,size_t);

    template <class type,uint size,class base> 
             static void SetVertexArray (const vertex<type,size,base>*,size_t);   

    template <class type,class base> 
             static void SetNormalArray (const normal<type,base>*,size_t);
};

#include "vb.inl"


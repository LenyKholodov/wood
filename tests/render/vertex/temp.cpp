template <class type> 
inline void BindNormalArray   (const Vertex::normal<type>* ptr,size_t size) 
{
  glEnableClientState (GL_NORMAL_ARRAY);
  glNormalPointer     (GetGLType ((type*)(NULL)),size,ptr);
}

template <class type> 
inline void BindColorArray    (const Vertex::color<type>* ptr,size_t size) 
{
  glEnableClientState (GL_COLOR_ARRAY);
  glColorPointer      (3,GetGLType ((type*)(NULL)),size,ptr);  
}

template <class type> 
inline void BindTexCoordArray (const Vertex::texcoord<type>*,size_t) 
{
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer   (2,GetGLType ((type*)(NULL)),size,ptr);
}

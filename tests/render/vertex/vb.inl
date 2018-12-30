__forceinline void Vertex::SetVertexArray (const void*)
{
  glDisableClientState (GL_VERTEX_ARRAY); 
}

__forceinline void Vertex::SetNormalArray (const void*)
{
  glDisableClientState (GL_NORMAL_ARRAY); 
}

template <class type,uint size,class base> 
__forceinline void Vertex::SetVertexArray (const vertex* array,size_t stride)
{
  glEnableClientState (GL_VERTEX_ARRAY);
  glVertexPointer     (size,GLType<type>::type,stride,array);
}

template <class type,class base> 
__forceinline void Vertex::SetNormalArray (const normal* array,size_t stride)
{
  glEnableClientState (GL_NORMAL_ARRAY);
  glNormalPointer     (GLType<type>::type,stride,array);
}

template <class vtype>
void Vertex::SetVertexArray (const vtype* vertexes)
{
  SetVertexArray   (vertexes,sizeof (vtype));
  SetNormalArray   (vertexes,sizeof (vtype));
}

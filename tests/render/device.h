namespace Render
{



/////////////////////////////////////////////////////////////////////////////////////////////
///Изображение
/////////////////////////////////////////////////////////////////////////////////////////////
template <class type,uint size,class base> struct Image
{
  uint        width, height, depth; //размеры
  datatype_t* data;             //данные
};



class Texture1D
{
  public:
    void  SetData    (uint width,uint iFormat,const Image&);
    void  SetSubData ();
};



void glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glBufferDataARBPROC (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
void glBufferSubDataARBPROC (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);

GLboolean glBindMem(GLenum target,
                    GLmem mem,
                    GLenum preserve)

GLmem glAllocMem1D(GLenum format, GLsizei width,
                   GLsizei n, const GLenum *properties,
                   const GLint *values)
 
GLmem glAllocMem2D(GLenum format, GLsizei width,
                   GLsizei height,
                   GLsizei n, const GLenum *properties,
                   const GLint *values)
 
GLmem glAllocMem3D(GLenum format, GLsizei width,
                   GLsizei height, GLsizei depth,
                   GLsizei n, const GLenum *properties,
                   const GLint *values)


GL_COLOR_BUFFER
GL_DEPTH_BUFFER
GL_STENCIL_BUFFER
GL_TEXTURE_1D
GL_TEXTURE_2D
GL_TEXTURE_3D
GL_TEXTURE_CUBE_MAP
GL_BUFFER_OBJECT

GL_MIPMAP
GL_SAMPLES
GL_USAGE

}

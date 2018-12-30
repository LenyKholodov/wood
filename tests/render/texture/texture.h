#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��������
////////////////////////////////////////////////////////////////////////////////////////////
class Texture
{
  public:  
              Texture  (GLenum target);
              ~Texture ();

    matrix4f  tm;

  private:
    GLuint    texture;
    GLenum    target;
};

////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
enum TextureFilters {
  T2D_NEAREST = 0, 
  T2D_LINEAR, 
  T2D_NEAREST_MIPMAP_NEAREST,
  T2D_LINEAR_MIPMAP_NEAREST,
  T2D_NEAREST_MIPMAP_LINEAR,
  T2D_LINEAR_MIPMAP_LINEAR,

  T2D_BILINEAR  = T2D_LINEAR,
  T2D_TRILINEAR = T2D_LINEAR_MIPMAP_LINEAR
};

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��� ������ �� �������
////////////////////////////////////////////////////////////////////////////////////////////
enum TextureWraps {
  T2D_CLAMP  = 0,
  T2D_REPEAT = 1
};

enum T2DUpdateMode {
  T2D_UPDATE_READ = 0,
  T2D_UPDATE_WRITE,
  T2D_UPDATE_READ_WRITE
};

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D: public Texture
{
  public:
                Texture2D        ();
                ~Texture2D       ();

        uchar   mipmaps     : 1; //������������ mipmap'�
        uchar   border      : 1; //������������ �� ������ (������������ ������ ��� �������� ��������)

////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������
////////////////////////////////////////////////////////////////////////////////////////////      
        uint    width            () const; //������ ��������
        uint    height           () const; //������ ��������
        size_t  size             () const; //������ � �����������

        uint    GetMipmapsNum    () const; //���������� mipmap'��

////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ��������� ���������� ����� �������������
////////////////////////////////////////////////////////////////////////////////////////////
        uchar   min_filter  : 3; //������ �� ����������
        uchar   mag_filter  : 3; //������ �� ����������
        uchar   wrap_s      : 1; //��������� ��� �������� �� ������� s
        uchar   wrap_t      : 1; //��������� ��� �������� �� ������� t

        float   max_anisotropy;  //������������ ���� ���������� �������� (� ��������)        

        col3f   borderColor;     //���� �������

////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
////////////////////////////////////////////////////////////////////////////////////////////
        void    Update           (Enum mode);

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
        void    SetPrior         (float prior);
        float   GetPrior         () const;

        bool    IsResident       () const; //��������� �� � �����������

////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �������� 
////////////////////////////////////////////////////////////////////////////////////////////
        struct  info_t {
          uint    width, height;        //������, ������
          uint    format;               //���������� ������ ��������
          float   prior;                //��������� ��������
          bool    compressed;           //����� �� ��������
          size_t  compress_size;        //������ ������ �������� (��� �������� = 0)
          uchar   redBits,  greenBits,  //���������� ��� �� ����������� ����������
                  blueBits, alphaBits,
                  luminanceBits,
                  intensityBits;
        };       

        bool    GetInfo          (info_t* buffer,uint level = 0,uint count = 1);

////////////////////////////////////////////////////////////////////////////////////////////
///��������� � �������� / ��������
////////////////////////////////////////////////////////////////////////////////////////////
        void    BindTexture      (); 
        void    UnloadTexture    ();

        bool    IsValid          () const { return mTexture != 0; } 

////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� � �����������
////////////////////////////////////////////////////////////////////////////////////////////
        bool    SetTexImage      (const Image&);
        bool    SetTexImage      (const UncompressedImage&);
        bool    SetTexImage      (const CompressedImage&);
        bool    GetTexImage      (UncompressedImage&,uint level = 0);
        bool    GetTexImage      (CompressedImage&,Pool&,uint level = 0);

////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� � ������ (�������� ������������� ���������� �������)
////////////////////////////////////////////////////////////////////////////////////////////
        void    CopyTexImage     (uint iformat,int x,int y,uint width,uint height); 
        void    CopyTexImage     (uint iformat,int x,int y,uint width,uint height,uint level);

        void    CopyTexSubImage  (int tex_x,int tex_y,int x,int y,uint width,uint height,uint level = 0);

  protected:
        void    SetTextureParams ();

  private:
        size_t  GetSize          () const;
        void    CacheData        ();

  private:
        uchar   mChange;
        uint    mTexture;
        uint    mWidth, mHeight, mSize;
        float   mPrior;
};

inline uint Texture2D::width () const
{
  return mWidth;
}

inline uint Texture2D::height () const
{
  return mHeight;
}

inline size_t Texture2D::size () const
{
  return mSize;
}

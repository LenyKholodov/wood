#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс текстуры
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
///Фильтры текстуры
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
///Поведение при выходе за границу
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
///Двумерная текстура
////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D: public Texture
{
  public:
                Texture2D        ();
                ~Texture2D       ();

        uchar   mipmaps     : 1; //генерировать mipmap'ы
        uchar   border      : 1; //присутствует ли бордюр (используется только при загрузке текстуры)

////////////////////////////////////////////////////////////////////////////////////////////
///Закэшированыые данные
////////////////////////////////////////////////////////////////////////////////////////////      
        uint    width            () const; //ширина текстуры
        uint    height           () const; //высота текстуры
        size_t  size             () const; //размер в видеопамяти

        uint    GetMipmapsNum    () const; //количество mipmap'ов

////////////////////////////////////////////////////////////////////////////////////////////
///После изменения следующих параметров нужно подтверждение
////////////////////////////////////////////////////////////////////////////////////////////
        uchar   min_filter  : 3; //фильтр на уменьшение
        uchar   mag_filter  : 3; //фильтр на увеличение
        uchar   wrap_s      : 1; //поведение при перевале за границу s
        uchar   wrap_t      : 1; //поведение при веревале за границу t

        float   max_anisotropy;  //максимальный угол отколнения текстуры (в градусах)        

        col3f   borderColor;     //цвет бордюра

////////////////////////////////////////////////////////////////////////////////////////////
///Обновить состояние
////////////////////////////////////////////////////////////////////////////////////////////
        void    Update           (Enum mode);

////////////////////////////////////////////////////////////////////////////////////////////
///Приоритет текстуры
////////////////////////////////////////////////////////////////////////////////////////////
        void    SetPrior         (float prior);
        float   GetPrior         () const;

        bool    IsResident       () const; //находится ли в видеопамяти

////////////////////////////////////////////////////////////////////////////////////////////
///Внутренние параметры текстуры 
////////////////////////////////////////////////////////////////////////////////////////////
        struct  info_t {
          uint    width, height;        //ширина, высота
          uint    format;               //внутренний формат текстуры
          float   prior;                //приоритет текстуры
          bool    compressed;           //сжата ли текстура
          size_t  compress_size;        //размер сжатой текстуры (для несжатой = 0)
          uchar   redBits,  greenBits,  //количество бит на определённые компоненты
                  blueBits, alphaBits,
                  luminanceBits,
                  intensityBits;
        };       

        bool    GetInfo          (info_t* buffer,uint level = 0,uint count = 1);

////////////////////////////////////////////////////////////////////////////////////////////
///Установка в контекст / удаление
////////////////////////////////////////////////////////////////////////////////////////////
        void    BindTexture      (); 
        void    UnloadTexture    ();

        bool    IsValid          () const { return mTexture != 0; } 

////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка текстуры в видеопамять
////////////////////////////////////////////////////////////////////////////////////////////
        bool    SetTexImage      (const Image&);
        bool    SetTexImage      (const UncompressedImage&);
        bool    SetTexImage      (const CompressedImage&);
        bool    GetTexImage      (UncompressedImage&,uint level = 0);
        bool    GetTexImage      (CompressedImage&,Pool&,uint level = 0);

////////////////////////////////////////////////////////////////////////////////////////////
///Копирование текстуры с экрана (тексутра автоматически становится текущей)
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

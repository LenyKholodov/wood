#pragma once

struct Image
{
  uint   width, height, depth;
  uint   bpp;
  void*  data;
};

struct ImageList
{
  Image img [];
};

class TextureArchive
{
 //?????
};

class TextureManager
{
  public:
        Texture2D*      CreateTexture2D (const char* name);  
        Texture3D*      CreateTexture3D (const char* name);
        Texture1D*      CreateTexture1D (const char* name);
        CubeMap*        CreateCubeMap   (const char** tex);

        void            RegisterBuffer  (const char* name,const Image&);
        void            RegisterBuffer  (const char* name,const ImageList&);

        void            SetMinFilter    (uint level);
        void            SetMagFilter    (uint level);
        void            SetInternalBPP  (uint bpp);

  private:
};

        
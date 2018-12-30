#include <pch.h>
#include <graph\textenv.h>
#include <binfile.h>
#include <graph\x3f.h>
#include <graph\bmp.h>
#include <graph\jpeg.h>
#include <graph\tga.h>
#include <graph\alp.h>

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")

extern uint TEXTURE_MAG_FILTER = GL_LINEAR_MIPMAP_LINEAR;
extern uint TEXTURE_MIN_FILTER  = GL_LINEAR_MIPMAP_LINEAR;

enum {
  NO_TEXTURE = -1,
  WRONG_TEXTURE = -2
};

struct   matnode_t
{
  textmat_t     mat;
  int           text;  
};

struct   textnode_t
{
  textnode_t*   next;
  textnode_t*   prev;
  matnode_t*    mat;
};

void         TextEnvironment::unbind ()
{
  glBindTexture (GL_TEXTURE_2D,mErrorText);
}

BOOL         TextEnvironment::bind              (int materialIndex)
{
  if (materialIndex < 0  || materialIndex > mTextCount)
    return FALSE;

  if (mMat [materialIndex].text > 0)
  {
    glBindTexture (GL_TEXTURE_2D,mHWTexture [mMat [materialIndex].text]);
    reset         (&mText [mMat [materialIndex].text]);
  }  
  else if (mMat [materialIndex].text != WRONG_TEXTURE)
  {
    if (!LoadTexture (materialIndex))
    {
      #ifdef DEBUG
//        dout<<"TextEnvironment: Texture not load #"<<materialIndex<<" "<<endl;
      #endif
      mMat [materialIndex].text = WRONG_TEXTURE;
      return FALSE;
    }
  }
  else
  {
    unbind ();
    return FALSE;
  }  

  return TRUE;
}

void         TextEnvironment::SetMaterial       (int materialIndex)
{
  D_Try
  {
    if (materialIndex < 0  || materialIndex > mTextCount)
      return;

    material_t& mat = mMat [materialIndex].mat.m;

    glMaterialfv  (GL_FRONT_AND_BACK,GL_AMBIENT,(GLfloat*)&mat.ambient);
    glMaterialfv  (GL_FRONT_AND_BACK,GL_DIFFUSE,(GLfloat*)&mat.diffuse);
    glMaterialfv  (GL_FRONT_AND_BACK,GL_SPECULAR,(GLfloat*)&mat.specular);
    glMaterialfv  (GL_FRONT_AND_BACK,GL_EMISSION,(GLfloat*)&mat.emission);
    glMaterialf   (GL_FRONT_AND_BACK,GL_SHININESS,mat.shiness);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at SetMaterial (int) index: "
          <<materialIndex<<" "<<this<<endl;
    #endif
  }
}

void         TextEnvironment::SetTextmat        (int materialIndex,const textmat_t& m)
{
  D_Try
  {
    if (materialIndex < 0 || materialIndex >= mTextCount)
      return ;

    mMat [materialIndex].mat = m;
  
    if (mMat [materialIndex].text > 0)
    {
      end (&mText [mMat [materialIndex].text]);
      mMat [materialIndex].text = -1;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG      
      dout<<"TextEnvironment: Error at SetTextmat () "<<this<<endl;
    #endif
  }
}

void         TextEnvironment::SetMaterial       (int materialIndex,const material_t& mat)
{
  D_Try
  {
    if (materialIndex < 0 || materialIndex >= mTextCount)
      return ;

    mMat [materialIndex].mat.m = mat;
  
    if (mMat [materialIndex].text > 0)
    {
      end (&mText [mMat [materialIndex].text]);
      mMat [materialIndex].text = -1;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG      
      dout<<"TextEnvironment: Error at SetMaterial (int,const material_t&) "<<this<<endl;
    #endif
  }
}

BOOL         TextEnvironment::SetTextureName    (int materialIndex,const char* name)
{
  D_Try
  {
    if (materialIndex < 0 || materialIndex >= mTextCount)
      return FALSE;

    if (strlen (name) >= TEXT_NAME_LEN)
      return FALSE;
    else
    {
      strcpy (mMat [materialIndex].mat.text,name);

      if (mMat [materialIndex].text > 0)
        end    (&mText [mMat [materialIndex].text]);
      mMat [materialIndex].text = -1;

      return TRUE;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at SetTextureName () "<<this<<endl;
    #endif
    return FALSE;
  }
}

material_t&  TextEnvironment::GetMaterial       (int materialIndex)
{
  D_Try
  {
    if (materialIndex < 0 || materialIndex > mTextCount)
      return; //throw
//      D_Error (EXCEPTION_ERROR);

    return mMat [materialIndex].mat.m;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at GetMaterial (int) "<<materialIndex<<" "<<this<<endl;
    #endif
    static material_t error;
    return error;
  }
}

const  char* TextEnvironment::GetTextureName    (int materialIndex)
{
  D_Try
  {
    if (materialIndex < 0 || materialIndex > mTextCount)
      return;  //throw
//      D_Error (EXCEPTION_ERROR);

    return mMat [materialIndex].mat.text;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at GetTextureName (int) "<<materialIndex<<" "<<this<<endl;
    #endif
    static char buf [1];
    buf [0] = 0; 
    return buf;
  }
}

bitmap_t*   TextEnvironment::LoadTexture       (const char* name,Pool* pool)
{
  char buf [128];

  locase (name,buf);

  char*         ext     = strchr (buf, '.'); 
  BinFile*      file    = F_OpenFile ("textures",name,BF_OPEN_RO);
  bitmap_t*     bmp     = NULL;

  if (IsValid (file))
  {  
    for (char* _ext=strchr(ext,'.');_ext;ext=_ext,_ext++,_ext=strchr(_ext,'.'));

    if (!ext)
    {
      if (!(bmp = LoadJPEG (file,pool,dout)))
        if (!(bmp = LoadBMP (file,pool,dout)))
          if (!(bmp = LoadTGA (file,pool,dout)))
            bmp = LoadALP (file,pool,dout);
    }
    else
    {
      if      (!strcmp (ext,".bmp"))  
      {
//        cacheBinFile cbf (pool,file,IMAGE_LOADER_CACHE_SIZE);     
//        bmp =  LoadBMP  (&cbf,pool,dout);
        bmp =  LoadBMP  (file,pool,dout);
      }
      else if (!strcmp (ext,".jpg"))  
        bmp =  LoadJPEG (file,pool,dout);
      else if (!strcmp (ext,".tga"))  
        bmp =  LoadTGA (file,pool,dout);
      else if (!strcmp (ext,".alp"))
        bmp =  LoadALP (file,pool,dout);
    }    

    delete file;
  }
  else
    return NULL;

  return bmp;
}

BOOL        TextEnvironment::LoadTexture       (int materialIndex)
{
  if (strlen (mMat [materialIndex].mat.text) <= 1)
    return FALSE;  

  bitmap_t*  bmp     = LoadTexture (mMat [materialIndex].mat.text,GetPool()); 

  if (!bmp)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at load texture ";
      dout<<"'"<<mMat [materialIndex].mat.text<<"' "<<this<<endl;
    #endif
    return FALSE;
  }

  static int fmt [5] = {0,GL_LUMINANCE,0,GL_RGB,GL_RGBA};
  int        format  = fmt [bmp->bpp>>3];

  if (!format)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Wrong texture bpp: "<<bmp->bpp<<endl;
    #endif
    D_Try
    {
      free    (bmp);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }
    return FALSE;
  }

  textnode_t* node           = mLast->prev;
  int         index          = long((char*)node-(char*)mText)/sizeof (textnode_t);
  mMat [materialIndex].text  = index;    
  node->mat                  = &mMat [materialIndex];

  glBindTexture    (GL_TEXTURE_2D,mHWTexture [index]);
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,TEXTURE_MAG_FILTER);
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,TEXTURE_MIN_FILTER);

  if (TEXTURE_MIN_FILTER < GL_NEAREST_MIPMAP_NEAREST &&
      TEXTURE_MAG_FILTER < GL_NEAREST_MIPMAP_NEAREST)            
        glTexImage2D     (GL_TEXTURE_2D,0,bmp->bpp>>3,bmp->width,bmp->height,
                          0,format,GL_UNSIGNED_BYTE,bmp->data);
  else
    gluBuild2DMipmaps    (GL_TEXTURE_2D,bmp->bpp>>3,bmp->width,bmp->height,
                          format,GL_UNSIGNED_BYTE,bmp->data);

  free    (bmp);

  reset (mLast->prev);
  
  return TRUE;
}

void         TextEnvironment::SetFirst          (int index)
{
  if (index < 0 || index >= mTextCount)
    return;

  if (mMat [index].text >= 0)  
    reset (mText + mMat [index].text); 
}

void         TextEnvironment::SetLast           (int index)
{
  if (index < 0 || index >= mTextCount)
    return;

  if (mMat [index].text >= 0)
    end (mText + mMat [index].text); 
}


void        TextEnvironment::end               (textnode_t* node)
{
  node->prev->next    = node->next;
  node->next->prev    = node->prev;

  node->next          = mLast;
  node->prev          = mLast->prev;
  mLast->prev->next   = node;
  mLast->prev         = node;
}

void        TextEnvironment::reset             (textnode_t* node)
{
        //check
  node->prev->next    = node->next;
  node->next->prev    = node->prev;

  node->next          = mFirst->next;
  node->prev          = mFirst;
  mFirst->next->prev  = node;
  mFirst->next        = node;
}

void        TextEnvironment::reset             ()
{
  D_Try
  {
    textnode_t*  mat   = mText;
    textnode_t*  prev  = NULL;
    textnode_t*  next  = &mText [1];
                      
    for (int i=0;i<mHWTextCount;i++,prev=mat,mat++,next++)
    {
      mat->prev = prev;
      mat->next = next;
      mat->mat  = NULL;
    }  

    for (i=0;i<mTextCount;i++)
      mMat [i].text = -1;

    mText [mHWTextCount-1].next = mLast; 
    mText [0].prev              = mFirst;
    mLast->prev                 = &mText [mHWTextCount-1];
    mLast->next                 = NULL;
    mFirst->next                = &mText [0];
    mFirst->prev                = NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at reset () "<<this<<endl;
    #endif
  }
}

BOOL         TextEnvironment::Load              (BinFile* file)
{  
  D_Try
  {
    file->read (&mTextCount,sizeof (mTextCount));

    if (mMat) free (mMat);

    mMat  = (matnode_t*)alloc (mTextCount * sizeof (matnode_t));

    file->read (mMat,mTextCount * sizeof (matnode_t));

    reset ();

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at Load (BinFile*) "<<this<<endl;
    #endif

    D_Try
    {
      if (mMat) free (mMat);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }        

    mMat        = NULL;
    mTextCount  = 0;

    return FALSE;
  }
}

void         TextEnvironment::Save              (BinFile* file) const
{
  D_Try
  {
    file->write  (&mTextCount,sizeof (mTextCount));
    file->write  (mMat,mTextCount * sizeof (matnode_t));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at Save (BinFile*) "<<this<<endl;
    #endif
  }
}

BOOL     TextEnvironment::CreateTextures (int num,uint* texture,textmat_t* text,Pool* pool)
{
  D_Try
  {
    glGenTextures    (num,texture);

    for (int i=0;i<num;i++)
    {
      bitmap_t*  bmp  = LoadTexture (text->text,pool);

      if (!bmp)
      {
        D_Message ("TextEnvironment: Error at load texture "<<
                   "'"<<text->text<<"'"
                  );
        glDeleteTextures (num,texture);
        return FALSE;
      }

      static int fmt [5] = {0,GL_LUMINANCE,0,GL_RGB,GL_RGBA};
      int        format  = fmt [bmp->bpp>>3];

      if (!format)
      {
        D_Message ("TextEnvironment: Wrong texture bpp: "<<bmp->bpp);
        D_Try
        {
          pool->free    (bmp);
        }
        D_Exception (EXCEPTION_EXECUTE_HANDLER)
        {
        }
        glDeleteTextures (num,texture);
        return FALSE;
      }

      glBindTexture    (GL_TEXTURE_2D,texture [i]);

      glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,TEXTURE_MAG_FILTER);
      glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,TEXTURE_MIN_FILTER);

      if (TEXTURE_MIN_FILTER < GL_NEAREST_MIPMAP_NEAREST &&
          TEXTURE_MAG_FILTER < GL_NEAREST_MIPMAP_NEAREST)            
            glTexImage2D     (GL_TEXTURE_2D,0,bmp->bpp>>3,bmp->width,bmp->height,
                              0,format,GL_UNSIGNED_BYTE,bmp->data);
      else
        gluBuild2DMipmaps    (GL_TEXTURE_2D,bmp->bpp>>3,bmp->width,bmp->height,
                              format,GL_UNSIGNED_BYTE,bmp->data);

      pool->free    (bmp);
    }

    return TRUE;
  }
  D_ExcRet ("TextEnvironment: Error at CreateTextures",FALSE);
}

int      TextEnvironment::CreateTexture  (const char* name,Pool* pool)
{
  D_Try
  {
    bitmap_t*  bmp  = LoadTexture (name,pool);

    if (!bmp)
    {
      D_Message ("TextEnvironment: Error at load texture "<<
                 "'"<<name<<"'"
                );
      return -1;
    }

    static int fmt [5] = {0,GL_LUMINANCE,0,GL_RGB,GL_RGBA};
    int        format  = fmt [bmp->bpp>>3];

    if (!format)
    {
      D_Message ("TextEnvironment: Wrong texture bpp: "<<bmp->bpp);
      D_Try
      {
        pool->free    (bmp);
      }
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      {
      }
      return -1;
    }

    uint texture;

    glGenTextures    (1,&texture);

    glBindTexture    (GL_TEXTURE_2D,texture);

    if (TEXTURE_MIN_FILTER < GL_NEAREST_MIPMAP_NEAREST &&
        TEXTURE_MAG_FILTER < GL_NEAREST_MIPMAP_NEAREST)            
          glTexImage2D     (GL_TEXTURE_2D,0,bmp->bpp>>3,bmp->width,bmp->height,
                            0,format,GL_UNSIGNED_BYTE,bmp->data);
    else
      gluBuild2DMipmaps    (GL_TEXTURE_2D,bmp->bpp>>3,bmp->width,bmp->height,
                            format,GL_UNSIGNED_BYTE,bmp->data);

    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,TEXTURE_MAG_FILTER);
    glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,TEXTURE_MIN_FILTER);

    pool->free    (bmp);

    return texture;
  }
  D_ExcRet ("TextEnvironment: Error at CreateTexture",-1);
}

TextEnvironment::TextEnvironment  (Pool* _Pool,int _TexturesNum,int _HWTextureAlloc)
                : MemObject       (_Pool),
                  mHWTextCount    (_HWTextureAlloc),
                  mTextCount      (_TexturesNum),
                  mMat            ((matnode_t*)_Pool->alloc (_TexturesNum*sizeof (matnode_t))),
                  mFirst          ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mLast           ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mText           ((textnode_t*)_Pool->alloc (sizeof (textnode_t)*_HWTextureAlloc)),
                  mHWTexture      ((uint*)_Pool->alloc (_HWTextureAlloc*sizeof (int)))
{
  if (!mMat || !mFirst || !mLast || !mText || !mHWTexture)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at construct. No enough memory "<<this<<endl;
    #endif
//    D_Try
//    {
      if (GetPool ())
      {
        if (mMat)       GetPool ()->free (mMat);
        if (mFirst)     GetPool ()->free (mFirst);
        if (mLast)      GetPool ()->free (mLast);
        if (mText)      GetPool ()->free (mText);
        if (mHWTexture) GetPool ()->free (mHWTexture);
      }  
//    }
//    D_Exception (EXCEPTION_EXECUTE_HANDLER)
//    {
//    }

    mMat         = NULL;
    mFirst       = NULL;
    mLast        = NULL;
    mText        = NULL;
    mHWTexture   = NULL;            
    mTextCount   = 0;
    mHWTextCount = 0;
  }
  else
  {
    reset         ();
    glGenTextures (_HWTextureAlloc,mHWTexture);
  }
}

TextEnvironment::TextEnvironment  (Pool* _Pool,int _TexturesNum,textmat_t* _TextmatArray,int _HWTextureAlloc)
                : MemObject       (_Pool),
                  mHWTextCount    (_HWTextureAlloc),
                  mTextCount      (_TexturesNum),
                  mMat            ((matnode_t*)_Pool->alloc (_TexturesNum*sizeof (matnode_t))),
                  mFirst          ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mLast           ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mText           ((textnode_t*)_Pool->alloc (sizeof (textnode_t)*_HWTextureAlloc)),
                  mHWTexture      ((uint*)_Pool->alloc (_HWTextureAlloc*sizeof (int)))
{
  if (!mMat || !mFirst || !mLast || !mText || !mHWTexture)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at construct. No enough memory "<<this<<endl;
    #endif
//    D_Try
//    {
      if (GetPool ())
      {
        if (mMat)       GetPool ()->free (mMat);
        if (mFirst)     GetPool ()->free (mFirst);
        if (mLast)      GetPool ()->free (mLast);
        if (mText)      GetPool ()->free (mText);
        if (mHWTexture) GetPool ()->free (mHWTexture);
      }  
//    }
//    D_Exception (EXCEPTION_EXECUTE_HANDLER)
//    {
//    }

    mMat         = NULL;
    mFirst       = NULL;
    mLast        = NULL;
    mText        = NULL;
    mHWTexture   = NULL;            
    mTextCount   = 0;
    mHWTextCount = 0;
  }
  else
  {
    reset         ();
    glGenTextures (_HWTextureAlloc,mHWTexture);
    memcpy        (mMat,_TextmatArray,sizeof (textmat_t)*_TexturesNum);
  }
}

TextEnvironment::TextEnvironment  (Pool* _Pool,BinFile* file,int _HWTextureAlloc)
                : MemObject       (_Pool),
                  mHWTextCount    (_HWTextureAlloc),
                  mTextCount      (0),
                  mMat            (NULL),
                  mFirst          ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mLast           ((textnode_t*)_Pool->alloc (sizeof (textnode_t))),
                  mText           ((textnode_t*)_Pool->alloc (sizeof (textnode_t)*_HWTextureAlloc)),
                  mHWTexture      ((uint*)_Pool->alloc (_HWTextureAlloc*sizeof (int))),
                  mErrorText      (0)
{
  if (!mFirst || !mLast || !mText || !mHWTexture)
  {
    #ifdef DEBUG
      dout<<"TextEnvironment: Error at construct. No enough memory "<<this<<endl;
    #endif
//    D_Try
//    {
      if (GetPool ())
      {
        if (mFirst)     GetPool ()->free (mFirst);
        if (mLast)      GetPool ()->free (mLast);
        if (mText)      GetPool ()->free (mText);
        if (mHWTexture) GetPool ()->free (mHWTexture);
      }  
//    }
//    D_Exception (EXCEPTION_EXECUTE_HANDLER)
//    {
//    }

    mFirst       = NULL;
    mLast        = NULL;
    mText        = NULL;
    mHWTexture   = NULL;            
    mTextCount   = 0;
    mHWTextCount = 0;
  }
  else
  {
    glGenTextures (_HWTextureAlloc,mHWTexture);
    glGenTextures (1,&mErrorText);
    Load          (file);
  }
}

TextEnvironment::~TextEnvironment ()
{
//  D_Try
//  {
    if (mHWTexture) glDeleteTextures (mHWTextCount,mHWTexture);  
    glDeleteTextures (1,&mErrorText);  

    if (mMat)       GetPool ()->free (mMat);
    if (mFirst)     GetPool ()->free (mFirst);
    if (mLast)      GetPool ()->free (mLast);
    if (mText)      GetPool ()->free (mText);
    if (mHWTexture) GetPool ()->free (mHWTexture);    
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
// {
//    #ifdef DEBUG
//      dout<<"TextEnvironment: Error at destruct "<<this<<endl;
//    #endif
//  }
}

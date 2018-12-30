#ifndef __TEXTURE_ENVIRONMENT__
#define __TEXTURE_ENVIRONMENT__

#include <memory\memory.h>
#include <binfile.h>
#include <graph\grtypes.h>

const    int    MAX_TEXTURES = 16;

struct   matnode_t;
struct   textnode_t;
struct   textmat_t;

////////////////////////////////////////////////////////////////////////////////
///Кэш текстур (with OpenGL support)
////////////////////////////////////////////////////////////////////////////////
class TextEnvironment: public MemObject
{
  public:
                        TextEnvironment  (Pool*,
                                          int    _TexturesNum, 
                                          int    _HWTextureAlloc = MAX_TEXTURES);
                        TextEnvironment  (Pool*,
                                          BinFile*,
                                          int    _HWTextureAlloc = MAX_TEXTURES);
                        TextEnvironment  (Pool*,
                                          int        _TexturesNum, 
                                          textmat_t* _TextmatArray,
                                          int        _HWTextureAlloc = MAX_TEXTURES); 
           virtual      ~TextEnvironment ();

////////////////////////////////////////////////////////////////////////////////
///Присоединение текстуры 
////////////////////////////////////////////////////////////////////////////////
           BOOL         bind              (int materialIndex);
           void         unbind            ();
           void         SetMaterial       (int materialIndex);

           void         SetFirst          (int materialIndex);
           void         SetLast           (int materialIndex);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
           void         SetTextmat        (int materialIndex,const textmat_t&);
           void         SetMaterial       (int materialIndex,const material_t&);
           material_t&  GetMaterial       (int materialIndex);
    const  char*        GetTextureName    (int materialIndex);
           BOOL         SetTextureName    (int materialIndex,const char*);

           int          count             () const { return mTextCount;   }
           int          hwcount           () const { return mHWTextCount; }

////////////////////////////////////////////////////////////////////////////////
///Сохранение - востановление
////////////////////////////////////////////////////////////////////////////////
           BOOL         Load              (BinFile*);
           void         Save              (BinFile*) const;

   virtual bool         valid             () const  { return mHWTexture &&
                                                      mMat && mText;
                                                    }

////////////////////////////////////////////////////////////////////////////////
///Просто создание текстур (ы)
////////////////////////////////////////////////////////////////////////////////
   static BOOL          CreateTextures    (int num,uint*,textmat_t*,Pool*);
   static int           CreateTexture     (const char*,Pool*);

  protected:
   static   bitmap_t*   LoadTexture       (const char*,Pool*);
            BOOL        LoadTexture       (int);

            void        reset             ();
            void        reset             (textnode_t*);
            void        end               (textnode_t*);

  protected:
           int          mTextCount;
           int          mHWTextCount;
           uint         mErrorText;
           uint*        mHWTexture;
           matnode_t*   mMat;
           textnode_t*  mText;
           textnode_t*  mFirst;
           textnode_t*  mLast;
};

extern uint TEXTURE_MAG_FILTER;
extern uint TEXTURE_MIN_FILTER;

#endif
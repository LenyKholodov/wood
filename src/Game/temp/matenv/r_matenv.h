#ifndef __RENDER_MATERIAL_ENVIRONMENT__
#define __RENDER_MATERIAL_ENVIRONMENT__

#include <graph\textenv.h>

#include "g_level.h"
#include "r_matdict.h"

////////////////////////////////////////////////////////////////////////////////
///Окружение материалов
////////////////////////////////////////////////////////////////////////////////
class    MatEnv: public MemObject
{
  public:
                 MatEnv  (Pool*,
                          GameLevel*,
                          int    _CacheTexturesNum,
                          int    _HWStaticTexturesNum
                         );
                 MatEnv  (Pool*,
                          TextEnvironment*,
                          int    _CacheTexturesNum
                         );
       virtual   ~MatEnv ();

////////////////////////////////////////////////////////////////////////////////
///Присоединение текстуры 
////////////////////////////////////////////////////////////////////////////////
           BOOL         bind              (int index);
           void         unbind            ();
           void         SetMaterial       (int materialIndex);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
           void         SetTextmat        (int materialIndex,const textmat_t&);
           void         SetMaterial       (int materialIndex,const material_t&);
           material_t&  GetMaterial       (int materialIndex);
    const  char*        GetTextureName    (int materialIndex);
           BOOL         SetTextureName    (int materialIndex,const char*);

           int          StaticCount       () const { return mStaticText->count (); }
           int          DinamicCount      () const { return mCacheText.count (); } 

   virtual bool         valid             () const;

////////////////////////////////////////////////////////////////////////////////
///Динамическое распределение текстур (не материалов)
////////////////////////////////////////////////////////////////////////////////
           int          AllocTexture      (const char*,ulong& CRC);
           void         FreeTexture       (int);
           uint         UseCount          (int);
           ulong        GetCRC            (int);

////////////////////////////////////////////////////////////////////////////////
///Распределение текстурных отрезков
////////////////////////////////////////////////////////////////////////////////
           int          AllocTextureRange (uint count,uint cacheNum = 1);
           void         FreeTextureRange  (int); //without return alloc

  private:
       struct   cachenode_t
       {        
         cachenode_t *next, *prev; 
         int         index;
         uint        count;
         ulong       crc;
       };

       struct   textrange_t
       {
         TextEnvironment*       env;
         int                    start, end;
       };

           void                 start             (cachenode_t*);
           void                 end               (cachenode_t*);
           TextEnvironment*     GetEnv            (int& index);

  private:
           typedef Dict<int>  CRCDict;
           typedef RingTmpl<textrange_t> TextEnvRing;

           int               mCacheCount, mStaticCount;
           cachenode_t*      mCacheNode, mFirst, mLast;
           CRCDict           mCRCDict;
           BOOL              mCreateStaticText;
           
           TextEnvironment   *mStaticText, mCacheText;
           TextEnvRing       mEnvRing;
           int               mTextAllocPos;
};

#endif
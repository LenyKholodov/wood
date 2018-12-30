#ifndef __WOOD_BOX_CACHE__
#define __WOOD_BOX_CACHE__

#include "g_map.h"
#include "defs.h"
#include <binfile.h>
#include <memory\flcache.h>

////////////////////////////////////////////////////////////////////////////////
///Абстрактная система кэширования на карте
////////////////////////////////////////////////////////////////////////////////
class    BoxCache: public FloatCache
{
  public:
                BoxCache (Pool*                 _Pool,
                          BinFile*              _File,
                          size_t                _CacheSize,
                          int                   _Width,
                          int                   _Height
                         );
     virtual    ~BoxCache();

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
     void*      lock            (int x,int y); //inc lock
     void       unlock          (int x,int y); //dec lock
     void       reset           ();

     void*      get             (int x,int y);

////////////////////////////////////////////////////////////////////////////////
///Проверка на корректность
////////////////////////////////////////////////////////////////////////////////
     virtual 
     bool       valid           () const { return  FloatCache::valid () &&
                                                   IsValid (mFile) &&
                                                   mInfo;
                                         }

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
     int        width           () const { return mWidth;  }
     int        height          () const { return mHeight; } 
     int        CacheWidth      () const { return mCacheWidth; }
     int        CacheHeight     () const { return mCacheHeight; }

  protected:
     BinFile*   file            () const { return mFile; }   

  protected:
         struct boxdesc_t         {
//           int16        x,y,width,height;
           int          x,y,width,height;
           char*        first;
           tileinfo_t*  info;
           char         data [];
         };

  protected:
   virtual BOOL            LoadData     (boxdesc_t*);
   virtual void            LoadInfo     (int,int,int,int) = 0;
   virtual void            Flush        () { reset (); }

           void            SetCacheBox  (int width,int height);

  private:
           boxdesc_t*      AllocBox     (int x,int y,BOOL lock = FALSE);
           dcachenode_t*   FindNode     (int x,int y);
           dcachenode_t*   AllocBox     (int x,int y,int width,int height);
   virtual BOOL            MoveData     (DWORD,void* from,void* to);
     
  private:
        BinFile*                mFile;
        int                     mWidth, mHeight, mCacheWidth, mCacheHeight;
        uint                    mLockData;

  protected:
        int                     mXs,mYs,mLoadWidth,mLoadHeight;
        tileinfo_t*             mInfo;
}; 

#endif
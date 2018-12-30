#ifndef __RENDER_MODEL_DATA_CACHE__
#define __RENDER_MODEL_DATA_CACHE__

#include "r_mdlkeeper.h"

////////////////////////////////////////////////////////////////////////////////
///Кэширование данных модели
////////////////////////////////////////////////////////////////////////////////
class    ModelDataCache: public MemObject
{
  public:
                ModelDataCache  (Pool*,
                                 size_t _CacheSize,
                                 int _ModelDataNum = 2048
                                );
        virtual ~ModelDataCache ();

////////////////////////////////////////////////////////////////////////////////
///Создание и удаление дескриптора
////////////////////////////////////////////////////////////////////////////////
        int                     CreateData (ModelKeeper*,BOOL _AutoLoad,va_list);
        void                    DeleteData (int);
        
////////////////////////////////////////////////////////////////////////////////
///Lock/Unlock данных
////////////////////////////////////////////////////////////////////////////////
        void*                   LockData   (int);
        void                    UnlockData (int);

////////////////////////////////////////////////////////////////////////////////
///Свойтсва (RO)
////////////////////////////////////////////////////////////////////////////////
        BOOL                    GetProperties (int,ModelKeeper*&,size_t&);

        virtual bool            valid         () const { return mCacheData != NULL &&
                                                         mDefragPool.valid ();
                                                       }

  protected:
         struct         cachenode_t;
         struct         cachedata_t
         {
           BOOL                 use;
           size_t               size;
           cachenode_t*         node;
           ModelKeeper*         owner;
           char                 param [128];
           int                  next; //free
         };

         struct         cachenode_t
         {
           cachenode_t  *next, *prev;
           cachedata_t* cd;
           BOOL         isLock;

           char         data [1];
         };

    static BOOL                 MoveData   (void* _From,void* _To,DWORD);           
           cachenode_t*         AllocData  (size_t);

  protected:
         cachedata_t*   mCacheData;
         int            mFirstFree;

         cachenode_t    mFirst, mLast;
         DefragPool     mDefragPool;
         int            mModelDataNum;
};

#endif
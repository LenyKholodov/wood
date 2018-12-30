////////////////////implementation of class LandscapeNoCache////////////////////

/*void*          LandscapeNoCache::lock            (int _GrNode)
{
  return mData + mRender->mInfo [_GrNode].offs - mRender->mInfo [0].offs;
}

void           LandscapeNoCache::reset           ()
{   }

LandscapeNoCache::LandscapeNoCache  (Pool* _Pool,LandMeshRender* _Render,size_t _PoolSize,int,int)
                 : MemObject (_Pool),
                   mRender   (_Render),
                   mData     (NULL)
{
  mData = (char*)pool()->alloc (_PoolSize);

  if (!mData)
  {
    #ifdef DEBUG
      dout<<"LandscapeNoCache: No enough memory "<<this<<endl;
    #endif
  }  

  mRender->mFile->seek (mRender->mInfo [0].offs);
  mRender->mFile->read (mData,mRender->mInfo [mRender->mMapSize-1].offs - 
                        mRender->mInfo [0].offs + 
                        mRender->mInfo [mRender->mMapSize-1].size
                       );
}
                   
LandscapeNoCache::~LandscapeNoCache ()
{
  if (mData) pool()->free (mData);
}
  */

////////////////////////////////////////////////////////////////////////////////
///Аналогия без кэширования
////////////////////////////////////////////////////////////////////////////////
class  LandscapeNoCache: public MemObject
{
  public:
                        LandscapeNoCache  (Pool*           _Pool,
                                           LandMeshRender* _Render,
                                           size_t          _PoolSize,
                                           int             _Width = 0,
                                           int             _Height = 0
                                          );
         virtual        ~LandscapeNoCache ();
         
////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
         void*          lock            (int _GrNode);
         void           reset           ();

         nodeinfo_t*    GetNodeInfo     (int i) const { return mInfo [i]; }
         meshinfo_t*    GetMeshInfo     (int i) const { return mMeshInfo [i]; }

  protected:
    char*               mData;
    LandMeshRender*     mRender;
    nodeinfo_t*         mInfo;
    meshinfo_t*         mMeshInfo;
};

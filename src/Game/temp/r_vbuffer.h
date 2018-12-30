////////////////////////////////////////////////////////////////////////////////
///Вершинный буфер разбитый на куски
////////////////////////////////////////////////////////////////////////////////
class   Cache;
class   VertexLump: public MemObject
{
  public:
                        VertexLump      (Pool*,
                                         VertexBuffer*,
                                         int count,
                                         int cacheNum  = 0,
                                         int cacheSize = 1
                                        );
                        VertexLump      (Pool*,
                                         VertexBuffer*
                                        );
       virtual          ~VertexLump     ();

       BOOL             set             (int index);
       int              base            () const { return mBaseIndex; }

   virtual bool         valid           () const { return mBaseIndex != -1; }

  protected:
   virtual    void      LoadData        (int index,vertexbuffer_t*) = 0;
              void      InitCache       (int cacheNum,int cacheSize);
              void      InitCache       (int clSize,int cacheNum,int cacheSize);

  private: 
   static     void      _LoadData       (DWORD,int,void*,size_t);
              int       curbuf          () const { return mCurBuf; }

  private:
        VertexBuffer*   mVBuffer;
        Cache*          mCache;
        vertex_t*       mVertex;
        vector3d_t*     mNormal;
        textcoord_t*    mTextCoord;
        int             mCurBuf;
        int             mBaseIndex;
        int             mCount, mVBNum;
};
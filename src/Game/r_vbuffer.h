#ifndef __RENDER_VERTEX_BUFFER__
#define __RENDER_VERTEX_BUFFER__

#include "g_level.h"

////////////////////////////////////////////////////////////////////////////////
///���設�� ����, �ਢ易�� � ���ᨢ� ���設
////////////////////////////////////////////////////////////////////////////////
class    VertexBuffer: public MemObject
{
  friend class VertexLump;
  friend class VertexPool;
  public:
                        VertexBuffer  (Pool*,uint count);
          virtual       ~VertexBuffer ();

          virtual bool  valid         () const { return mVertex != NULL &&
                                                 mNormal != NULL &&
                                                 mTextCoord != NULL;
                                               }

////////////////////////////////////////////////////////////////////////////////
///�㭪樨, �⭮��騥�� � CVA
////////////////////////////////////////////////////////////////////////////////
          void          CVALock          (int,int);
          void          CVAUnlock        ();
          void          CVARefresh       ();

////////////////////////////////////////////////////////////////////////////////
///�㭪樨, �⭮��騥�� � ���樨 VAO
////////////////////////////////////////////////////////////////////////////////
          void          ChangeBase       (int new_base);
          void          RestoreBase      ();

          vertex_t*     vertex           (int base) const  { return mVertex + base; }
                  
   protected:
          int           AllocChunk    (uint num);          
          BOOL          get           (uint base,vertex_t**,vector3d_t**,textcoord_t**);

   protected:
          vertex_t*     mVertex;
          vector3d_t*   mNormal;
          textcoord_t*  mTextCoord;
          uint          mCount, mPos;
};

////////////////////////////////////////////////////////////////////////////////
///���設�� ��
////////////////////////////////////////////////////////////////////////////////

struct vbuffernode_t
{
  uint          count, base;    //������⢮ ����⮢ � ������ ������

  vertex_t*     vertex;         //�����⥫� �� ���ᨢ ���設
  vector3d_t*   normal;         //�����⥫� �� ���ᨢ ��ଠ���
  textcoord_t*  tcoord;         //�����⥫� �� ���ᨢ ⥪������ ���न���
};   

typedef   BOOL (*MoveVBFn)(vbuffernode_t& vb,int offs,DWORD);

class     VertexPool: public MemObject
{
  public:
                        VertexPool  (Pool*,
                                     VertexBuffer*,
                                     uint count
                                    );
       virtual          ~VertexPool ();

       virtual bool     valid       () const { return mVBuffer!=NULL && mVertex && mNormal && mTextCoord; }

               void             SetMoveFn (MoveVBFn fn,DWORD par) { mMoveFn = fn; mParam = par; }

////////////////////////////////////////////////////////////////////////////////
///����� � �㫥
////////////////////////////////////////////////////////////////////////////////
               uint             VertexCount    () const { return mCount; }
               uint             VertexAvail    () const { return mFree;  }
               uint             VertexMaxAvail () const;

////////////////////////////////////////////////////////////////////////////////
///���।������ ��᪮� �����
////////////////////////////////////////////////////////////////////////////////
               vbuffernode_t*   AllocChunk  (uint count);
               void             FreeChunk   (vbuffernode_t*);

               void             defrag      ();

  private:
        struct vpnode_t
        {
          vbuffernode_t    vb;
          vpnode_t         *next, *prev;
          vpnode_t         *prevFree, *nextFree;
        };

        vpnode_t*               concat (vpnode_t* left,vpnode_t* right);
        BOOL                    move   (vpnode_t* left,vpnode_t* right);
               
  private:
    vertex_t*           mVertex;
    vector3d_t*         mNormal;
    textcoord_t*        mTextCoord;    
    uint                mCount, mFree;
    vpnode_t*           mFirstFree;
    MoveVBFn            mMoveFn;
    DWORD               mParam;
    VertexBuffer*       mVBuffer;
};

////////////////////////////////////////////////////////////////////////////////
///��� ���設
////////////////////////////////////////////////////////////////////////////////
class   VertexCache: public MemObject
{
  public:
                        VertexCache     (Pool*,
                                         VertexBuffer*,
                                         int count,
                                         int vbCount = 0
                                        );
       virtual          ~VertexCache    ();  

////////////////////////////////////////////////////////////////////////////////
///��⠭�������� ���設�� ���� � ���⥪�� � �����頥� ������ ������
////////////////////////////////////////////////////////////////////////////////
       int              bind            (int index,int* count = NULL);
       int              set             (int index,int* count = NULL); //�����. ������ ������

////////////////////////////////////////////////////////////////////////////////
///�����
////////////////////////////////////////////////////////////////////////////////
       int              count           () const { return mVertexPool.VertexCount (); } 
       virtual bool     valid           () const { return mIndex && mVBuffer && IsValid (&mVertexPool); }                

       vertex_t*        vertex          (int first) const  { return mVBuffer->vertex (first); }

  protected:
        struct  vcnode_t
        {
          vcnode_t        *prev, *next;
          vbuffernode_t*  vb;
          int             index;
        };        

    virtual vcnode_t*   LoadData        (int index) = 0;

        vcnode_t*       AllocChunk      (uint count);
        void            FreeChunk       (vcnode_t*);

  private:
    inline  vcnode_t*   find            (int);

  private:
        int               mVBCount;
        int               mCurBuf, mCurBase;
        vcnode_t**        mIndex;
        vcnode_t          mFirst, mLast;
        VertexPool        mVertexPool;
        VertexBuffer*     mVBuffer;
};

#endif
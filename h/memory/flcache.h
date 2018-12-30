#ifndef __MEMORY_FLOAT_CACHE__
#define __MEMORY_FLOAT_CACHE__

#include <memory\dpool.h>

struct cachenode_t 
{
  void*         buf;
  int           index;
  cachenode_t*  prev;
  cachenode_t*  next;
};

//������ ��� ��������� ������ ������

////////////////////////////////////////////////////////////////////////////////
///��� � ���������������
////////////////////////////////////////////////////////////////////////////////

struct  dcachenode_t: public cachenode_t
{
  size_t        size;   //������ �����
  BOOL          lock;   //���� ����
  DWORD         param;  //�������� ������������
};

class   FloatCache: public MemObject
{
  public:
                        FloatCache (Pool*      _Pool,
                                    size_t     _CacheSize,
                                    uint       _Flags = FLAGS_DEFAULT
                                   );
            virtual     ~FloatCache ();

        enum {
             READ_ONLY          = 1,
             NO_LOCK            = 2,
             MOVE_DATA_EVENT    = 4,
             NO_MOVE            = 8,
             FLUSH_EVENT        = 16,

             FLAGS_DEFAULT      = READ_ONLY
        };

  protected:
////////////////////////////////////////////////////////////////////////////////
///������ � �������� �����������
////////////////////////////////////////////////////////////////////////////////
        void            start        (dcachenode_t*);
        void            end          (dcachenode_t*);                

////////////////////////////////////////////////////////////////////////////////
///��������� / �����
////////////////////////////////////////////////////////////////////////////////
        void            lock         (dcachenode_t*);        
        void            unlock       (dcachenode_t*);

  public:  
        void            reset        ();

////////////////////////////////////////////////////////////////////////////////
///������
////////////////////////////////////////////////////////////////////////////////
        size_t          size         () const { return mDefragPool.size (); }
        uint            flags        () const { return mFlags;              }

   virtual bool         valid        () const { return mDefragPool.valid (); }

  protected:
////////////////////////////////////////////////////////////////////////////////
///��������� ����� ��������� �������
////////////////////////////////////////////////////////////////////////////////
           dcachenode_t*      AllocChunk   (size_t);
   virtual void               FreeChunk    (dcachenode_t*);

////////////////////////////////////////////////////////////////////////////////
///������������� ������������� ������
////////////////////////////////////////////////////////////////////////////////
   virtual void            Flush        () {}
   virtual BOOL            MoveData     (DWORD,void* from,void* to) { return TRUE; };
   virtual void            UnloadData   (dcachenode_t*) {}

  private:
   static  BOOL            Move         (void* from,void* to,DWORD);

  protected:
        DefragPool      mDefragPool;
        uint            mFlags;
        dcachenode_t    mFirst, mLast;        
};

#endif
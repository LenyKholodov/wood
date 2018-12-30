#ifndef __RESOURCE_CACHE__
#define __RESOURCE_CACHE__

#include <resource\resource.h>
#include <memory\pool.h>
#include <misc\ring.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///���� �ਭ������騩 ������
/////////////////////////////////////////////////////////////////////////////////////////////
struct cachebuffer_t
{
  struct bufnode_t
  {
    bufnode_t *prev,*next;
    cachebuffer_t *buf;
  };         

  Resource*    res;
  cachebuffer_t    *next,*prev;
  bufnode_t    bufnode;
  size_t       size;
  char         buf [];
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���
/////////////////////////////////////////////////////////////////////////////////////////////
class    Cache: public Pool, public ResourceGroup
{
  friend class  CacheResourcePrivateData;
  friend struct cachebuffer_t;
  public:
                        Cache  (Pool*);
          virtual       ~Cache ();

/////////////////////////////////////////////////////////////////////////////////////////////
///�㡫�஢���� ����䥩� �㫠
/////////////////////////////////////////////////////////////////////////////////////////////
          virtual       void    reset   () {}
          virtual       bool    belong  (void*) const = 0;

          virtual       size_t  size     () const = 0;
          virtual       size_t  memavail () const = 0;
          virtual       size_t  maxavail () const = 0;

  protected:
          virtual void*   _alloc        (size_t)  = 0;
          virtual void    _free         (void*)   = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
///��८�।������ free ��� ��⮬��� (� �ਢ�� ����� ���⠢����� dirty)
/////////////////////////////////////////////////////////////////////////////////////////////
                 void     free          (cachebuffer_t*); 

/////////////////////////////////////////////////////////////////////////////////////////////
///��⠭���� ���� ���� ���� ��᫥����
/////////////////////////////////////////////////////////////////////////////////////////////
               cachebuffer_t* GetFirstBuffer () const { return mBufFirst.next->buf; }
               cachebuffer_t* GetLastBuffer  () const { return mBufLast.prev->buf;  }
               void           SetFirstBuffer (cachebuffer_t*); //��� �஢�ન ���४⭮��
               void           SetLastBuffer  (cachebuffer_t*);

               void           InsertBuffer   (cachebuffer_t*);
               void           EraseBuffer    (cachebuffer_t*);
               
/////////////////////////////////////////////////////////////////////////////////////////////
///����� ���஢
/////////////////////////////////////////////////////////////////////////////////////////////
   static      cachebuffer_t* NextBuffer     (cachebuffer_t* buf);
   static      cachebuffer_t* PrevBuffer     (cachebuffer_t* buf);

/////////////////////////////////////////////////////////////////////////////////////////////
///����᫨�� ���� �易��� � ����ᮬ
/////////////////////////////////////////////////////////////////////////////////////////////
               bool           EnumBuffers    (const Resource&,Ring<cachebuffer_t*>&) const;

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��뫮� �� ���� �� ��� ��६�饭��
/////////////////////////////////////////////////////////////////////////////////////////////
               void           UpdateBuffer  (cachebuffer_t* from,cachebuffer_t* to); 

  private:
       virtual void   insert (Resource&);
       virtual bool   erase  (Resource&);

       virtual bool   lock   (Resource&);

       virtual bool   first  (Resource&); 
       virtual bool   last   (Resource&); 

       virtual Pool*  GetPool (const Resource&) const;

                      Pool::alloc;
  
  private:
        cachebuffer_t::bufnode_t     mBufFirst,mBufLast;
};


#endif
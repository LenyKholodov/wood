#ifndef __RESOURCE_CACHE__
#define __RESOURCE_CACHE__

#include <resource\resource.h>
#include <memory\pool.h>
#include <misc\ring.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Буфер принадлежащий ресурсу
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
///Кэш
/////////////////////////////////////////////////////////////////////////////////////////////
class    Cache: public Pool, public ResourceGroup
{
  friend class  CacheResourcePrivateData;
  friend struct cachebuffer_t;
  public:
                        Cache  (Pool*);
          virtual       ~Cache ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Дублирование интерфейса пула
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
///Переопределение free для потомков (в привате ресурса выставляется dirty)
/////////////////////////////////////////////////////////////////////////////////////////////
                 void     free          (cachebuffer_t*); 

/////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера первым иили последним
/////////////////////////////////////////////////////////////////////////////////////////////
               cachebuffer_t* GetFirstBuffer () const { return mBufFirst.next->buf; }
               cachebuffer_t* GetLastBuffer  () const { return mBufLast.prev->buf;  }
               void           SetFirstBuffer (cachebuffer_t*); //без проверки корректности
               void           SetLastBuffer  (cachebuffer_t*);

               void           InsertBuffer   (cachebuffer_t*);
               void           EraseBuffer    (cachebuffer_t*);
               
/////////////////////////////////////////////////////////////////////////////////////////////
///Итерация буферов
/////////////////////////////////////////////////////////////////////////////////////////////
   static      cachebuffer_t* NextBuffer     (cachebuffer_t* buf);
   static      cachebuffer_t* PrevBuffer     (cachebuffer_t* buf);

/////////////////////////////////////////////////////////////////////////////////////////////
///Перечислить буфера связанные с ресурсом
/////////////////////////////////////////////////////////////////////////////////////////////
               bool           EnumBuffers    (const Resource&,Ring<cachebuffer_t*>&) const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Обновление ссылок на буфер при его перемещении
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
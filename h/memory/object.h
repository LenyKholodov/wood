#ifndef __MEMORY_OBJECT__
#define __MEMORY_OBJECT__

#include <types.h>
#include <memory\memory.h>
#include <misc\excpt.h>
#include <debug\defs.h>

class    Pool;
class    Cache;

class    MemManager
{
  public:
/////////////////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
/////////////////////////////////////////////////////////////////////////////////////////////
    void*       operator           new     (size_t,
                                            Pool* dest_pool = ::M_GetPool ()
                                           ) throw (ExcBadAlloc&);
    void*       operator           new     (size_t,void*); //size += 4 bytes!!
    void        operator           delete  (void*);  
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Описание абстрактного класса управления памятью
///(Является базовым классом почти для всей иерархии объектов)
/////////////////////////////////////////////////////////////////////////////////////////////
class    MemObject: public MemManager
{
  friend class UsePool;
  public:
                        MemObject  ();
                        MemObject  (Pool* pool);
                virtual ~MemObject () {}


/////////////////////////////////////////////////////////////////////////////////////////////
///Проверка объекта на корректность
/////////////////////////////////////////////////////////////////////////////////////////////
    friend      bool               IsValid (const MemObject*);
    friend      bool               IsValid (const MemObject&);
    virtual     bool               valid   () const { return TRUE; }

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Выделение памяти
/////////////////////////////////////////////////////////////////////////////////////////////
                void*              alloc   (size_t size);
                void               free    (void* x);

/////////////////////////////////////////////////////////////////////////////////////////////
///Работа с пулом объекта
/////////////////////////////////////////////////////////////////////////////////////////////
    inline      Pool*              GetPool   () const     { return mPool; }
    inline      void               SetPool   (Pool* pool) { mPool = pool; }

    inline      Pool*              M_GetPool () const     { return mPool; }

  private:
               Pool*               mPool;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Управление памятью с явным указанием пула
/////////////////////////////////////////////////////////////////////////////////////////////
class           UsePool 
{
  public:
                UsePool (MemObject* obj,Pool* pool) { obj->SetPool (pool); }
};

#endif
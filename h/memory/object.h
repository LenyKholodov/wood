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
///������������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
    void*       operator           new     (size_t,
                                            Pool* dest_pool = ::M_GetPool ()
                                           ) throw (ExcBadAlloc&);
    void*       operator           new     (size_t,void*); //size += 4 bytes!!
    void        operator           delete  (void*);  
};

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ������ ���������� �������
///(�������� ������� ������� ����� ��� ���� �������� ��������)
/////////////////////////////////////////////////////////////////////////////////////////////
class    MemObject: public MemManager
{
  friend class UsePool;
  public:
                        MemObject  ();
                        MemObject  (Pool* pool);
                virtual ~MemObject () {}


/////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �� ������������
/////////////////////////////////////////////////////////////////////////////////////////////
    friend      bool               IsValid (const MemObject*);
    friend      bool               IsValid (const MemObject&);
    virtual     bool               valid   () const { return TRUE; }

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
                void*              alloc   (size_t size);
                void               free    (void* x);

/////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����� �������
/////////////////////////////////////////////////////////////////////////////////////////////
    inline      Pool*              GetPool   () const     { return mPool; }
    inline      void               SetPool   (Pool* pool) { mPool = pool; }

    inline      Pool*              M_GetPool () const     { return mPool; }

  private:
               Pool*               mPool;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� � ����� ��������� ����
/////////////////////////////////////////////////////////////////////////////////////////////
class           UsePool 
{
  public:
                UsePool (MemObject* obj,Pool* pool) { obj->SetPool (pool); }
};

#endif
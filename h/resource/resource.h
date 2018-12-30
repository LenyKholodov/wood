#ifndef __MEMORY_RESOURCE__
#define __MEMORY_RESOURCE__

#include <memory\pool.h>

enum ResourceStates {
  RES_WRONG      = 1,   //неверный ресурс
  RES_LOAD       = 2,   //загружаемый ресурс
  RES_DIRTY      = 4,   //ресурс, частично загруженный
  RES_UPLOAD     = 8,   //"догрузка"
  RES_SWAP       = 16   //умеет свапится
};

class    ResourceGroup;
class    ResourcePrivatePool;

////////////////////////////////////////////////////////////////////////////////
///Ресурс
////////////////////////////////////////////////////////////////////////////////
class    Resource
{
  friend class ResourceService;
  friend class ResourceGroup;

  public:
                Resource  (ResourceGroup* = NULL);
    virtual     ~Resource ();

////////////////////////////////////////////////////////////////////////////////
///Запирание / отпирание
////////////////////////////////////////////////////////////////////////////////
    virtual     uint      lock       (); //ret:lock count
    virtual     void      unlock     ();

////////////////////////////////////////////////////////////////////////////////
///Владелец
////////////////////////////////////////////////////////////////////////////////
    virtual     void           SetOwner   (ResourceGroup* own = NULL);
                ResourceGroup* GetOwner () const { return mOwner; }

////////////////////////////////////////////////////////////////////////////////
///Распределение памяти
////////////////////////////////////////////////////////////////////////////////
                void*     operator new    (size_t,Pool* = mResourcePool);
                void      operator delete (void*);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
    inline      bool      IsWrong    () const { return mFlags & RES_WRONG;   }
    inline      bool      IsDirty    () const { return mFlags & RES_DIRTY; }
    inline      bool      IsUpload   () const { return mFlags & RES_UPLOAD; }
    inline      bool      IsLoad     () const { return mFlags & RES_LOAD;    }
    inline      bool      IsSwap     () const { return mFlags & RES_LOAD;    }
    inline      bool      IsLock     () const { return mLock != 0;       }

    inline      uint16    LockCount  () const { return mLock;            }

    inline      uint16    flags      ()       const { return mFlags;        }

    inline      uint16    enable     (uint x)       { return (mFlags |= x); }
    inline      uint16    disable    (uint x)       { return (mFlags &= ~0 ^ x); }

  protected:
////////////////////////////////////////////////////////////////////////////////
///Для ext_new и проч.
////////////////////////////////////////////////////////////////////////////////
    inline      ResourcePrivatePool*  M_GetPool  () const; 
    inline      ResourcePrivatePool*  GetPool    () const { return M_GetPool (); }

////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка ресурса из памяти
////////////////////////////////////////////////////////////////////////////////
    virtual     bool      load       () { if (IsLoad () || IsDirty ()) unload (); return true; }
    virtual     void      unload     ();

////////////////////////////////////////////////////////////////////////////////
///Какой-либо из членов ресурса ХОЧЕТ обновить своё расположение
///Вызывается только в unlock-state
///В случае вызова free возможен вызов update без учёта возвращаемого значения!
////////////////////////////////////////////////////////////////////////////////
    virtual     bool      update     (void* old_position, 
                                      void* new_position
                                     ) { return false; }

  private:
    ResourceGroup* mOwner;
    Resource       *mPrev, *mNext;
    uint16         mLock, mFlags;
    void*          mOwnerData;

    static Pool*   mResourcePool; 
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Группа ресурсов
/////////////////////////////////////////////////////////////////////////////////////////////
class  ResourceGroup
{
  friend class Resource;
  public:
                ResourceGroup  ();
       virtual  ~ResourceGroup ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Перемещение ресурса по очереди приоритетов
/////////////////////////////////////////////////////////////////////////////////////////////
      virtual  bool     first  (Resource&); 
      virtual  bool     last   (Resource&); 

/////////////////////////////////////////////////////////////////////////////////////////////
///Итератор
/////////////////////////////////////////////////////////////////////////////////////////////
       class iterator
       {         
         friend class ResourceGroup;
         public:
                         iterator (): mCurrent (NULL) {}
                         iterator (Resource& r):mCurrent (&r) {}
                         iterator (const ResourceGroup& group) {*this=group.GetFirst();} 
                         iterator (const iterator& iter):mCurrent (iter.mCurrent) {}

               iterator& operator =  (const iterator& iter) { mCurrent = iter.mCurrent; return *this;}
               iterator& operator =  (const ResourceGroup& group) { return *this = group.GetFirst ();;}
               iterator& operator =  (Resource& r) { mCurrent = &r; return *this;}
                         
               Resource* operator -> () const { return data(); }
                         operator Resource* () const { return data(); }                         

               iterator&             operator  ++ ()    { return ResourceGroup::IterNext(*this); }
               const iterator&       operator  ++ (int) { return ResourceGroup::IterNext(*this); }
               iterator&             operator  -- ()    { return ResourceGroup::IterPrev(*this); }
               const iterator&       operator  -- (int) { return ResourceGroup::IterPrev(*this); }

                     bool            more () { return mCurrent && ResourceGroup::ResNext (*this) && ResourceGroup::ResPrev (*this); }
                     Resource*       data () const  { return mCurrent; }

          protected:
                 Resource* mCurrent;
       };

       friend class  ResourceGroup::iterator;

       typedef iterator iter;

/////////////////////////////////////////////////////////////////////////////////////////////
///Для итераторов
/////////////////////////////////////////////////////////////////////////////////////////////
       iterator         GetFirst        () const { return iterator (*mFirst.mNext); } 
       iterator         GetLast         () const { return iterator (*mLast.mPrev);  } 

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Запирание / отпирание ресурса
/////////////////////////////////////////////////////////////////////////////////////////////
       virtual bool     lock           (Resource& r) { first (r); return true; } 
       virtual void     unlock         (Resource&) {}

/////////////////////////////////////////////////////////////////////////////////////////////
///Добавление ресурса
/////////////////////////////////////////////////////////////////////////////////////////////
       virtual void     insert (Resource& r) { r.mOwner = this; first (r); } //сразу становится первым 
       virtual bool     erase  (Resource&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв связей с ресурсами
/////////////////////////////////////////////////////////////////////////////////////////////
               void     UnlinkAll      ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к приватным данным ресурса
/////////////////////////////////////////////////////////////////////////////////////////////
       static  void*&   data    (const Resource& r) { return (void*)r.mOwnerData;  }
       static  void     unload  (Resource& r)       { r.unload (); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Обновление ресурса с учётом использования operator new
/////////////////////////////////////////////////////////////////////////////////////////////
       static  bool     update  (Resource& r,void* from,void* to) { 
           return r.update (from?(char*)from+4:NULL,to?(char*)to+4:NULL); 
       }

/////////////////////////////////////////////////////////////////////////////////////////////
///Трюк для распределения памяти в resource посредством ext_new
/////////////////////////////////////////////////////////////////////////////////////////////
       virtual Pool*    GetPool (const Resource&) const { return ::M_GetPool (); } 


  private:
       static inline    iterator& IterNext   (iterator& i)  { i.mCurrent = i.mCurrent ? i.mCurrent->mNext : NULL; return i; }
       static inline    iterator& IterPrev   (iterator& i)  { i.mCurrent = i.mCurrent ? i.mCurrent->mPrev : NULL; return i; }
       static inline    Resource* ResNext    (iterator& i)  { return i.mCurrent ? i.mCurrent->mNext : NULL; }
       static inline    Resource* ResPrev    (iterator& i)  { return i.mCurrent ? i.mCurrent->mPrev : NULL; }

  private:
        Resource  mFirst, mLast; 
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Пользователь должен вызывать operator new (ext_new) / operator delete
///Иначе возможны ошибки с адресами update!
/////////////////////////////////////////////////////////////////////////////////////////////
class    ResourcePrivatePool: public Pool
{
  protected:
                Pool::alloc;
                Pool::free;
                Pool::belong;
};

inline  ResourcePrivatePool* Resource::M_GetPool  () const 
{ 
  return mOwner ? (ResourcePrivatePool*)mOwner->GetPool (*this) : (ResourcePrivatePool*)::M_GetPool(); 
}

#ifdef DEBUG
  #pragma comment(lib,"resourced.lib")
#else
  #pragma comment(lib,"resource.lib")
#endif

#endif
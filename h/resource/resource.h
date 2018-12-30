#ifndef __MEMORY_RESOURCE__
#define __MEMORY_RESOURCE__

#include <memory\pool.h>

enum ResourceStates {
  RES_WRONG      = 1,   //������ �����
  RES_LOAD       = 2,   //����㦠��� �����
  RES_DIRTY      = 4,   //�����, ���筮 ����㦥���
  RES_UPLOAD     = 8,   //"����㧪�"
  RES_SWAP       = 16   //㬥�� ᢠ�����
};

class    ResourceGroup;
class    ResourcePrivatePool;

////////////////////////////////////////////////////////////////////////////////
///������
////////////////////////////////////////////////////////////////////////////////
class    Resource
{
  friend class ResourceService;
  friend class ResourceGroup;

  public:
                Resource  (ResourceGroup* = NULL);
    virtual     ~Resource ();

////////////////////////////////////////////////////////////////////////////////
///����࠭�� / �⯨࠭��
////////////////////////////////////////////////////////////////////////////////
    virtual     uint      lock       (); //ret:lock count
    virtual     void      unlock     ();

////////////////////////////////////////////////////////////////////////////////
///��������
////////////////////////////////////////////////////////////////////////////////
    virtual     void           SetOwner   (ResourceGroup* own = NULL);
                ResourceGroup* GetOwner () const { return mOwner; }

////////////////////////////////////////////////////////////////////////////////
///���।������ �����
////////////////////////////////////////////////////////////////////////////////
                void*     operator new    (size_t,Pool* = mResourcePool);
                void      operator delete (void*);

////////////////////////////////////////////////////////////////////////////////
///�����
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
///��� ext_new � ���.
////////////////////////////////////////////////////////////////////////////////
    inline      ResourcePrivatePool*  M_GetPool  () const; 
    inline      ResourcePrivatePool*  GetPool    () const { return M_GetPool (); }

////////////////////////////////////////////////////////////////////////////////
///����㧪� / ���㧪� ����� �� �����
////////////////////////////////////////////////////////////////////////////////
    virtual     bool      load       () { if (IsLoad () || IsDirty ()) unload (); return true; }
    virtual     void      unload     ();

////////////////////////////////////////////////////////////////////////////////
///�����-���� �� 童��� ����� ����� �������� ᢮� �ᯮ�������
///��뢠���� ⮫쪮 � unlock-state
///� ��砥 �맮�� free �������� �맮� update ��� ���� �����頥���� ���祭��!
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
///��㯯� ����ᮢ
/////////////////////////////////////////////////////////////////////////////////////////////
class  ResourceGroup
{
  friend class Resource;
  public:
                ResourceGroup  ();
       virtual  ~ResourceGroup ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��६�饭�� ����� �� ��।� �ਮ��⮢
/////////////////////////////////////////////////////////////////////////////////////////////
      virtual  bool     first  (Resource&); 
      virtual  bool     last   (Resource&); 

/////////////////////////////////////////////////////////////////////////////////////////////
///�����
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
///��� ����஢
/////////////////////////////////////////////////////////////////////////////////////////////
       iterator         GetFirst        () const { return iterator (*mFirst.mNext); } 
       iterator         GetLast         () const { return iterator (*mLast.mPrev);  } 

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///����࠭�� / �⯨࠭�� �����
/////////////////////////////////////////////////////////////////////////////////////////////
       virtual bool     lock           (Resource& r) { first (r); return true; } 
       virtual void     unlock         (Resource&) {}

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
/////////////////////////////////////////////////////////////////////////////////////////////
       virtual void     insert (Resource& r) { r.mOwner = this; first (r); } //�ࠧ� �⠭������ ���� 
       virtual bool     erase  (Resource&);

/////////////////////////////////////////////////////////////////////////////////////////////
///����� �痢� � ����ᠬ�
/////////////////////////////////////////////////////////////////////////////////////////////
               void     UnlinkAll      ();

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � �ਢ��� ����� �����
/////////////////////////////////////////////////////////////////////////////////////////////
       static  void*&   data    (const Resource& r) { return (void*)r.mOwnerData;  }
       static  void     unload  (Resource& r)       { r.unload (); }

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ���⮬ �ᯮ�짮����� operator new
/////////////////////////////////////////////////////////////////////////////////////////////
       static  bool     update  (Resource& r,void* from,void* to) { 
           return r.update (from?(char*)from+4:NULL,to?(char*)to+4:NULL); 
       }

/////////////////////////////////////////////////////////////////////////////////////////////
///��� ��� ��।������ ����� � resource ���।�⢮� ext_new
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
///���짮��⥫� ������ ��뢠�� operator new (ext_new) / operator delete
///���� �������� �訡�� � ���ᠬ� update!
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
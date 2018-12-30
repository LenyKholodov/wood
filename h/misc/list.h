#ifndef __MISC_BASE_LIST__
#define __MISC_BASE_LIST__

#include <misc\collection.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
class  BaseList: public BaseCollection
{
  public:
                        BaseList (); 

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ��������
/////////////////////////////////////////////////////////////////////////////////////////////
        void            InsertFirst  (listnode_t&); 
        void            InsertLast   (listnode_t&); 
        void            Insert       (listnode_t& node) { InsertLast (node); } //�� ��������� � �����

/////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ������������ ������
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void    reset        ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��������
/////////////////////////////////////////////////////////////////////////////////////////////
        bool            IsLast   (const listnode_t& node) const { return node.next == &mLast; }
        bool            IsFirst  (const listnode_t& node) const { return node.prev == &mFirst; }
        bool            IsValid  (const listnode_t& node) const { return &node && &node != &mNull && 
                                                    node.next!=&mNull && node.prev!=&mNull; }

/////////////////////////////////////////////////////////////////////////////////////////////
///������ / ��������� ��������        
/////////////////////////////////////////////////////////////////////////////////////////////
       listnode_t&      GetFirst     () const { return mFirst.next != &mLast ? *mFirst.next : *(listnode_t*)NULL; }
       listnode_t&      GetLast      () const { return mLast.prev != &mFirst ? *mLast.prev : *(listnode_t*)NULL; } 

/////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ������ ������ / � ����� ������
/////////////////////////////////////////////////////////////////////////////////////////////
        void            SetFirst   (listnode_t& node)  { InsertFirst (node); }
        void            SetLast    (listnode_t& node)  { InsertLast (node); }

  private:
        listnode_t        mFirst, mLast;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class   List: public Collection<T,BaseList>
{
  public:
        List (Pool* pool = Memory::M_GetPool ()) : Collection<T,BaseList> (pool) { }
};

#endif
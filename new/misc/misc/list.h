#ifndef __MISC_BASE_LIST__
#define __MISC_BASE_LIST__

#include <misc\iter.h>
#include <memory\pool.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс списка
/////////////////////////////////////////////////////////////////////////////////////////////
class   BaseList
{
  protected:
        struct listnode_t;
        friend struct listnode_t;

  public:
                        BaseList ();
        virtual         ~BaseList () {}

/////////////////////////////////////////////////////////////////////////////////////////////
///Кол-во элементов
/////////////////////////////////////////////////////////////////////////////////////////////
        int             count        () const { return mCount; }
        bool            empty        () const { return mFirst.next == (listnode_t*)&mLast; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Сброс без освобождения памяти
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void    reset        ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словарные операции        
/////////////////////////////////////////////////////////////////////////////////////////////
        void            InsertFirst  (listnode_t&); 
        void            InsertLast   (listnode_t&); 
        void            Insert       (listnode_t& node) { InsertFirst (node); } //по умолчанию в начало

        bool            InsertAfter  (listnode_t& after,listnode_t& ins); 
        bool            InsertBefore (listnode_t& before,listnode_t& ins);

        void            Erase        (listnode_t&); 

//Дубляж

        void            insert  (listnode_t& node)  { Insert (node); }
        void            add     (listnode_t& node)  { Insert (node); }
        void            erase   (listnode_t& node)  { Erase  (node); }
        void            extract (listnode_t& node)  { Erase  (node); }
        void            del     (listnode_t& node)  { Erase  (node); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Проверка
/////////////////////////////////////////////////////////////////////////////////////////////
        bool            IsLast   (const listnode_t& node) const { return node.next == (listnode_t*)&mLast; }
        bool            IsFirst  (const listnode_t& node) const { return node.prev == (listnode_t*)&mFirst; }
        bool            IsValid  (const listnode_t& node) const { return &node && &node != &mNull; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Первый / последний элементы        
/////////////////////////////////////////////////////////////////////////////////////////////
       listnode_t&      GetFirst     () const { return mFirst.next != (listnode_t*)&mLast ? *mFirst.next : *(listnode_t*)NULL; }
       listnode_t&      GetLast      () const { return mLast.prev !=(listnode_t*) &mFirst ? *mLast.prev : *(listnode_t*)NULL; } 

/////////////////////////////////////////////////////////////////////////////////////////////
///Перемещение в начало списка / в конец списка
/////////////////////////////////////////////////////////////////////////////////////////////
        void            SetFirst   (listnode_t&);
        void            SetLast    (listnode_t&);

  protected:
        struct  listnode_t
        {
          listnode_t  *prev, *next;

          listnode_t&  IterNext () const { return next->bind (); }
          listnode_t&  IterPrev () const { return prev->bind (); }

          virtual listnode_t& bind () const { return (listnode_t&)*this; }
                         
                       listnode_t (): prev (&mNull), next (&mNull) {}
                       listnode_t (listnode_t* p,listnode_t* n):prev (p), next (n) {}
        };

  protected:
        struct firstnode_t: public listnode_t
        {
          virtual listnode_t& bind () const { return prev ? *prev->prev : *(listnode_t*)NULL;}
        };

        struct lastnode_t: public listnode_t
        {
          virtual listnode_t& bind () const { return next ? *next->next : *(listnode_t*)NULL;}
        };

  protected:
        firstnode_t mFirst;
        lastnode_t  mLast;

  private:        
        static listnode_t mNull;
        int               mCount;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Шаблон списка
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class   List: public MemObject, protected BaseList
{
  protected:
        struct BaseList::listnode_t;
        struct node_t;        
        friend struct List<T>::node_t;

  public:
/////////////////////////////////////////////////////////////////////////////////////////////
///Итераторы  
/////////////////////////////////////////////////////////////////////////////////////////////
        typedef ::iterator<node_t,T,List>     iterator;
        typedef ::dataiterator<node_t,T,List> dataiterator;
        typedef iterator                      iter;
        typedef dataiterator                  diter;

                List  (Pool* pool = Memory::M_GetPool());
     virtual    ~List ();

                BaseList::count;                

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словарные операции
/////////////////////////////////////////////////////////////////////////////////////////////
     bool       InsertFirst  (const T&,bool exc = false) throw (ExcNoMemory&);   
     bool       InsertLast   (const T&,bool exc = false) throw (ExcNoMemory&);   

     bool       insert       (const T& x,bool exc = false) throw (ExcNoMemory&) 
                {  return InsertFirst (x,exc); }
     void       insert       (iterator& i) { SetFirst (i); }

     bool       InsertAfter  (iterator& after,iterator& insertIterator);
     bool       InsertBefore (iterator& before,iterator& insertIterator);

     bool       InsertAfter  (iterator& after,const T&,bool exc = false)  throw (ExcNoMemory&); 
     bool       InsertBefore (iterator& before,const T&,bool exc = false)  throw (ExcNoMemory&); 

     void       erase        (iterator&,bool free_memory = true);
     void       erase        (const T& x,bool free_memory = true) { erase (find (x),free_memory); }

     iterator   find         (const T&) const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Для общей схемы работы со множествами
/////////////////////////////////////////////////////////////////////////////////////////////
     List&      operator +=  (const T& x)  throw (ExcNoMemory&) { insert (x,true); return *this;}
     List&      operator +=  (iterator& i)                      { insert (i); return *this; }
     List&      operator -=  (const T& x)                       { erase (x); return *this; }
     List&      operator -=  (iterator& i)                      { erase (i); return *this; }

//     List&      operator +=  (const List&);
//     List&      operator -=  (const List&);
//     List&      operator &=  (const List&);
//     List&      operator |=  (const List&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Взятие начального и конечного элемента
/////////////////////////////////////////////////////////////////////////////////////////////
     iterator   GetFirst  () const { return iterator ((node_t&)BaseList::GetFirst ()); }
     iterator   GetLast   () const { return iterator ((node_t&)BaseList::GetLast ()); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Перемещение в начало списка / в конец списка
/////////////////////////////////////////////////////////////////////////////////////////////
     void       SetFirst   (iterator& i) { if (&i) BaseList::SetFirst (i.node ()); }
     void       SetLast    (iterator& i) { if (&i) BaseList::SetLast (i.node ());  }

/////////////////////////////////////////////////////////////////////////////////////////////
///Проверка
/////////////////////////////////////////////////////////////////////////////////////////////
     bool       IsLast   (const iterator& i) const { return IsLast (i.node ()); }
     bool       IsFirst  (const iterator& i) const { return IsFirst (i.node ()); }
     bool       IsValid  (const iterator& i) const { return &i && IsValid (i.node ()); }

  protected:
        struct node_t: public BaseList::listnode_t
        {
          T     data;

          T&    IterData () const { return (T&)data; }

                node_t (const T& x): data (x) {}

          static node_t* create (const T& x,Pool* pool) {
            node_t* node = (node_t*)pool->alloc (sizeof (node_t));

            if (node) node->node_t::node_t (x);

            return node;
          }
        };
};

#include <misc\list.inl>

#endif
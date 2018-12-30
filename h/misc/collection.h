#ifndef __MISC_Collection__
#define __MISC_Collection__

#include <memory\pool.h>
#include <misc\iter.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс для коллекций (кольца / списки)
/////////////////////////////////////////////////////////////////////////////////////////////
class   BaseCollection
{
  protected:
        struct listnode_t;
        friend struct listnode_t;

  public:
                        BaseCollection ();
        virtual         ~BaseCollection () {}

/////////////////////////////////////////////////////////////////////////////////////////////
///Кол-во элементов
/////////////////////////////////////////////////////////////////////////////////////////////
        int             count        () const { return mCount; }
        bool            empty        () const { return mCount == 0; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Сброс без освобождения памяти
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual void    reset        () { mCount = 0; }

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словарные операции        
/////////////////////////////////////////////////////////////////////////////////////////////
        bool            InsertAfter  (listnode_t& after,listnode_t& ins); 
        bool            InsertBefore (listnode_t& before,listnode_t& ins);

        void            Erase        (listnode_t&); 

  protected:
#pragma pack (push)
#pragma pack (1)
        struct  listnode_t
        {
          listnode_t  *prev, *next;

          listnode_t&  IterNext () const { return *next; }
          listnode_t&  IterPrev () const { return *prev; }
                         
                       listnode_t (): prev (&mNull), next (&mNull) {}
                       listnode_t (listnode_t* p,listnode_t* n):prev (p), next (n) {}
        };
#pragma pack (pop)

  protected:
        static listnode_t mNull;
        int               mCount;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Коллекция
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class Base>
class   Collection: public MemObject, protected Base
{
  protected:
        struct node_t;        
        friend struct node_t;

  public:
/////////////////////////////////////////////////////////////////////////////////////////////
///Итераторы  
/////////////////////////////////////////////////////////////////////////////////////////////
        typedef ::iterator<node_t,T,Collection> iterator, iter;
        typedef ::dataiterator<iterator>        dataiterator, diter;

                Collection  (Pool* pool = Memory::M_GetPool());
     virtual    ~Collection ();

                Base::count;

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словарные операции
/////////////////////////////////////////////////////////////////////////////////////////////
     bool       InsertFirst  (const T&,bool exc = false) throw (ExcNoMemory&);
     bool       InsertLast   (const T&,bool exc = false) throw (ExcNoMemory&);

     bool       insert       (const T& x,bool exc = false) throw (ExcNoMemory&) 
                {  return InsertLast (x,exc); }
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
     Collection&      operator +=  (const T& x)  throw (ExcNoMemory&) { insert (x,true); return *this;}
     Collection&      operator +=  (iterator& i)                      { insert (i); return *this; }
     Collection&      operator -=  (const T& x)                       { erase (x); return *this; }
     Collection&      operator -=  (iterator& i)                      { erase (i); return *this; }

//     List&      operator +=  (const List&);
//     List&      operator -=  (const List&);
//     List&      operator &=  (const List&);
//     List&      operator |=  (const List&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Взятие начального и конечного элемента
/////////////////////////////////////////////////////////////////////////////////////////////
     iterator   GetFirst  () const { return iterator ((node_t&)Base::GetFirst ()); }
     iterator   GetLast   () const { return iterator ((node_t&)Base::GetLast ()); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Перемещение в начало списка / в конец списка
/////////////////////////////////////////////////////////////////////////////////////////////
     void       SetFirst   (iterator& i) { if (&i) Base::SetFirst (i.node ()); }
     void       SetLast    (iterator& i) { if (&i) Base::SetLast (i.node ());  }

/////////////////////////////////////////////////////////////////////////////////////////////
///Проверка
/////////////////////////////////////////////////////////////////////////////////////////////
     bool       IsLast   (const iterator& i) const { return IsLast (i.node ()); }
     bool       IsFirst  (const iterator& i) const { return IsFirst (i.node ()); }
     bool       IsValid  (const iterator& i) const { return &i && Base::IsValid (i.node ()); }

////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
////////////////////////////////////////////////////////////////////////////////////////////
     void       clear    ();

  protected:
        struct node_t: public Base::listnode_t
        {
          T     data;

          T&    IterData () const { return (T&)data; }

                node_t (const T& x): data (x) {}

          static node_t* create (const T& x,Pool* pool) {
            node_t* node = (node_t*)pool->alloc (sizeof (node_t));

            if (node) node->node_t::node_t (x);

            return node;
          }
          static void dispose (node_t* node,Pool* pool) {
            node->data.~T ();
            pool->free (node);        
          }
        };  
};

#include <misc\Collection.inl>

#endif
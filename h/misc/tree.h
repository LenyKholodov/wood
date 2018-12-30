#ifndef __MISC_RED_BLACK_TREE__
#define __MISC_RED_BLACK_TREE__

#include <misc\rbtree.h>
#include <memory\object.h>
#include <misc\iter.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Двоичное дерево
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class key_t = signed int>
class BinTree: public MemObject, private RedBlackTree
{
  protected:
//        struct RedBlackTree::mapnode_t;
        struct node_t;
        friend struct BinTree<T,key_t>::node_t;

  public:
                BinTree (Pool* pool = Memory::M_GetPool ()): MemObject (pool) {}
    virtual     ~BinTree () { clear (); }

                MemObject::GetPool;
                MemObject::SetPool;

/////////////////////////////////////////////////////////////////////////////////////////////
///Итераторы    
/////////////////////////////////////////////////////////////////////////////////////////////
    class iterator: public ::iterator<node_t,T,BinTree>
    {
      public:
                    iterator () {}
                    iterator (const iterator& i): ::iterator<node_t,T,BinTree> ((::iterator<node_t,T,BinTree>&)i) {}
                    iterator (BinTree& tree): ::iterator<node_t,T,BinTree> (tree) {}
                    iterator (node_t& node): ::iterator<node_t,T,BinTree> (node) {}

                      //Пробег по дереву
                    iterator left   () const   { return iterator (*(node_t*)mCurrent->left); }
                    iterator right  () const   { return iterator (*(node_t*)mCurrent->right); }
                    iterator parent () const   { return iterator (*(node_t*)mCurrent->parent); }
                    const key_t&   key () const { return mCurrent->key; }
    };

    typedef ::dataiterator<iterator>  dataiterator, diter;
    typedef iterator                  iter;

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словрне операции
/////////////////////////////////////////////////////////////////////////////////////////////
      bool      insert  (const key_t&,const T&,bool exc = false) throw (ExcNoMemory&);

      void      erase   (const key_t& key) { erase (find (key)); }
      void      erase   (iterator&,bool free_memory = true);

      iterator  find    (const key_t&)    const;
      bool      find    (const key_t&,T&) const;

      bool      search  (const key_t& key,T& res) const { return find (key,res); }
      iterator  search  (const key_t& key) const { return find (key); }

      bool      present (const key_t&)     const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Очистка дерева
/////////////////////////////////////////////////////////////////////////////////////////////
      void      clear   ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Минимальный / максимальный
/////////////////////////////////////////////////////////////////////////////////////////////
      iterator  GetMin   (const iterator&)  const;
      iterator  GetMax   (const iterator&)  const;

      iterator  GetMin   ()  const;
      iterator  GetMax   ()  const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Пробег итератором
/////////////////////////////////////////////////////////////////////////////////////////////
      iterator  GetFirst () const { return GetMin (); }
      iterator  GetRoot  () const { return iterator ((node_t&)RedBlackTree::GetRoot ()); }
      iterator  GetLast  () const { return GetMax (); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Количество
/////////////////////////////////////////////////////////////////////////////////////////////
                RedBlackTree::count;

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///Узел
/////////////////////////////////////////////////////////////////////////////////////////////
    struct node_t: public RedBlackTree::mapnode_t
    {
      key_t key;
      T     data;              

      T&    IterData () const { return (T&)data; }

            node_t   (const key_t& k,const T& x): key(k),data(x) {}

      static node_t* create (const key_t& key,const T& x,Pool* pool)  {
        node_t* node = (node_t*)pool->alloc (sizeof (node_t));

        if (node)
          node->node_t::node_t (key,x);

        return node;
      }
      static void dispose (node_t* node,Pool* pool) {
        node->data.~T ();
        pool->free (node);        
      }
    };

  private:
      node_t*   _find    (const key_t&) const;
      void      clear    (node_t*);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Словарь, ключами которого являются строки
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class   StringDict: public BinTree<T,ulong>
{
  public:
                StringDict  (Pool* pool = Memory::M_GetPool ()): BinTree<T,ulong> (pool) {}

/////////////////////////////////////////////////////////////////////////////////////////////
///Основные словрне операции
/////////////////////////////////////////////////////////////////////////////////////////////
      BinTree<T,ulong>::iterator  find    (const char*)    const;
      bool                        find    (const char*,T&) const;

      bool                        insert  (const char*,const T&,bool exc = false) throw (ExcNoMemory&);
      void                        erase   (const char* x);
      void                        erase   (BinTree<T,ulong>::iterator&,bool free_mem = true);

      bool                        search  (const char* key,T& res) const;
      BinTree<T,ulong>::iterator  search  (const char* key) const;

      bool                        present (const char* x)  const;

////////////////////////////////////////////////////////////////////////////////////////////
///Используем числовой ключ
////////////////////////////////////////////////////////////////////////////////////////////
      bool                        insert  (ulong,const T&,bool exc = false) throw (ExcNoMemory&);

      void                        erase   (ulong);

      BinTree<T,ulong>::iterator  find    (ulong)    const;
      bool                        find    (ulong,T&) const;

      bool                        search  (ulong,T&) const;
      BinTree<T,ulong>::iterator  search  (ulong) const;

      bool                        present (ulong)     const;

  private:
      ulong     toKey   (const char* x) const { return CRC32 (x,strlen (x)); }  
};

#include <misc\tree.inl>

#endif
#ifndef __Dictionary_TREE_TMPL__
#define __Dictionary_TREE_TMPL__

#include <memory\pool.h>
#include <debug\defs.h>
#include <misc\excpt.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Словарь на основе бинарного дерева
/////////////////////////////////////////////////////////////////////////////////////////////
template <class Type> class DictionaryTmpl: public MemObject
{
  public:
    struct treenode_t;

    typedef long key_t;

  public:
                        DictionaryTmpl  (Pool*);
                        DictionaryTmpl  ();
          virtual       ~DictionaryTmpl ();

////////////////////////////////////////////////////////////////////////////////
///Итератор
////////////////////////////////////////////////////////////////////////////////
          class iterator
          {
            friend class DictionaryTmpl<Type>;
            protected:
                            iterator (treenode_t* node):mCurrent (node) {} 
            public:
                            iterator (const DictionaryTmpl& ring):mCurrent (ring.first()) {} 
                            iterator (const iterator& iter):mCurrent (iter.mCurrent) {}

                  iterator& operator =  (const iterator& iter) { mCurrent = iter.mCurrent; return *this;}
                  iterator& operator =  (const DictionaryTmpl& ring) { mCurrent = ring.first(); return *this;}
                                
                  Type&     operator -> () const { return data(); }
                  operator  Type&       () const { return data(); }

                  iterator   left       () { return iterator (mCurrent?mCurrent->left:NULL);  }
                  iterator   right      () { return iterator (mCurrent?mCurrent->right:NULL); }
                  iterator   prev       () { return iterator (mCurrent?mCurrent->prev:NULL);  }

                        bool            more () { return mCurrent != NULL; }
                        Type&           data () const throw (ExcNoElement&) { 
                           if (mCurrent) return mCurrent->data;
                           else          throw  ExcNoElement ("Dict::iterator::data");       
                         }

             protected:               
                        treenode_t*     mCurrent;
          };

          friend class  DictionaryTmpl::iterator;

////////////////////////////////////////////////////////////////////////////////
///Добавление - удаление по номеру (добавление по верху - замена)
////////////////////////////////////////////////////////////////////////////////          
          bool          insert     (key_t,const Type&);
          void          erase      (key_t);
          void          erase      (iterator&);

          void          clear      () { clear (mNode); mNode = NULL; }

////////////////////////////////////////////////////////////////////////////////
///Минимум - максимум дерева
////////////////////////////////////////////////////////////////////////////////
          Type&         GetMin     () const throw (ExcNoElement&);
          Type&         GetMax     () const throw (ExcNoElement&);  

////////////////////////////////////////////////////////////////////////////////
///Поиск по дереву
////////////////////////////////////////////////////////////////////////////////
          Type&         search     (key_t)        const throw (ExcNoElement&);
          Type&         search     (const char* s) const throw (ExcNoElement&) {
            return search (CRC32 (s,strlen (s)));
          }
          Type&         operator[] (key_t)        const throw (ExcNoElement&);
          Type&         operator[] (const char*)  const throw (ExcNoElement&);

          bool          search     (key_t,Type&)  const;
          bool          search     (key_t,Type*&)  const;
          bool          search     (const char* s,Type& dest)  const {
            return search (CRC32 (s,strlen (s)),dest);
          }
          bool          search     (const char* s,Type*& dest)  const {
            return search (CRC32 (s,strlen (s)),dest);
          }
          bool          present    (key_t)        const;

////////////////////////////////////////////////////////////////////////////////
///Количество элементов в дереве
////////////////////////////////////////////////////////////////////////////////
          int           count      ()           const { return mCount; }

  public:
    struct treenode_t
    {
      treenode_t*   prev;
      treenode_t*   left;
      treenode_t*   right;
      key_t          id;
      Type          data;
    };

  protected:

////////////////////////////////////////////////////////////////////////////////
///Служебные
////////////////////////////////////////////////////////////////////////////////
          treenode_t*   successor  (treenode_t*) const;
          treenode_t*   find       (key_t id)     const;
          treenode_t*   GetMax     (treenode_t*) const;
          treenode_t*   GetMin     (treenode_t*) const;
          treenode_t*   first      ()            const { return mNode; }

          void          clear      (treenode_t*);

  private:
    treenode_t*         mNode;
    int                 mCount;
};

#include <misc\dict.inl>

#define Dict DictionaryTmpl

#endif
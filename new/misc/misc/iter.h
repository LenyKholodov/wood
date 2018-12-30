#ifndef __MISC_ITERATOR__
#define __MISC_ITERATOR__

#include <types.h>
#include <misc\excpt.h>

class DefaultIterList;
template <typename Node,typename Data,class List> class dataiterator;

/////////////////////////////////////////////////////////////////////////////////////////////
///Итератор
/////////////////////////////////////////////////////////////////////////////////////////////
template <typename Node,typename Data,class List = DefaultIterList>  class iterator
{
  public:
                  iterator ();
                  iterator (iterator& i): mCurrent (i.mCurrent) {}
                  iterator (const List&); //List должен содержать "iterator GetFirst ()"
                  iterator (const Node&);

        Data&     operator ->    () const { return data(); }                   
                  operator Data& () const { return data(); }
                  operator bool  () const { return mCurrent != NULL; }
                  operator dataiterator<Node,Data,List>& () const { return (dataiterator<Node,Data,List>&)*this; }
        dataiterator<Node,Data,List>& toData () const  { return (dataiterator<Node,Data,List>&)*this; }                                      

        bool       valid   () const { return mCurrent != NULL; }

        iterator& operator =     (const List&);
        iterator& operator =     (const Node&);
        iterator& operator =     (const Node*);
        iterator& operator =     (const Data& x) throw (ExcNoElement&) { data () = x; }
        iterator& operator =     (const iterator& i) { mCurrent = i.mCurrent; return *this; }

        iterator&             operator  ++ ()               { return next (); }
        iterator              operator  ++ (int)            { iterator i = *this; next (); return i; }
        iterator&             operator  -- ()               { return prev (); }
        iterator              operator  -- (int)            { iterator i = *this; prev (); return i; }

        iterator&             operator  += (int);
        iterator&             operator  -= (int);

        static iterator       operator   + (const iterator&,int);
        static iterator       operator   - (const iterator&,int);

        Data&                 data         () const throw (ExcNoElement&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Ссылка на узел
/////////////////////////////////////////////////////////////////////////////////////////////
        Node&                 node         () const { return *mCurrent; }

  private:
          iterator&       next    () { mCurrent = mCurrent ? (Node*)&mCurrent->IterNext () : NULL; return *this; }
          iterator&       prev    () { mCurrent = mCurrent ? (Node*)&mCurrent->IterPrev () : NULL; return *this; }

  protected:
          Node*              mCurrent;
};

template <typename Node,typename Data,class List = DefaultIterList>
class dataiterator: public iterator <Node,Data,List>
{
  public:
                  dataiterator () {}
                  dataiterator (const iterator& i): iterator<Node,Data,List> (i) {}
                  dataiterator (const List& l): iterator<Node,Data,List> (l) {}
                  dataiterator (const Node& n): iterator<Node,Data,List> (n) {}

        Data*     operator ->    () const { return &data(); } 
                  operator Data* () const { return &data(); }
};

#include <misc\iter.inl>

#endif

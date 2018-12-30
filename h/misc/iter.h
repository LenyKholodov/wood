#ifndef __MISC_ITERATOR__
#define __MISC_ITERATOR__

#include <types.h>
#include <misc\excpt.h>

#pragma warning (disable : 4786) //truncate dunbug information (255 char)

class DefaultIterList;

/////////////////////////////////////////////////////////////////////////////////////////////
///»тератор
/////////////////////////////////////////////////////////////////////////////////////////////
template <typename Node,typename Data,class List = DefaultIterList>  class iterator
{
  public:
    typedef Data IData;
    typedef List IList;

                  iterator ();
                  iterator (const iterator& i): mCurrent (i.mCurrent) {}
                  iterator (const List&); //List должен содержать "iterator GetFirst ()"
                  iterator (const Node&);

        const Data&   operator ->    () const { return data(); }
              Data&   operator ->    () { return data(); }

                      operator Data& () const  { return data(); }                 

        bool       valid   () const { return mCurrent != NULL; }

        iterator& operator =     (const List&);
        iterator& operator =     (const Node&);
        iterator& operator =     (const Node*);
        iterator& operator =     (const Data& x) throw (ExcNoElement&) { data () = x; }
        iterator& operator =     (const iterator& i) { mCurrent = i.mCurrent; return *this; }

        bool      operator ==    (const iterator& i) const    { return i.mCurrent == mCurrent; }
        bool      operator !=    (const iterator& i) const    { return i.mCurrent != mCurrent; }

        iterator&             operator  ++ ()               { return next (); }
        iterator              operator  ++ (int)            { iterator i = *this; next (); return i; }
        iterator&             operator  -- ()               { return prev (); }
        iterator              operator  -- (int)            { iterator i = *this; prev (); return i; }

        iterator&             operator  += (int);
        iterator&             operator  -= (int);

        friend iterator       operator   + (const iterator&,int);
        friend iterator       operator   - (const iterator&,int);        

        Data&                 data         () const throw (ExcNoElement&);

/////////////////////////////////////////////////////////////////////////////////////////////
///—сылка на узел
/////////////////////////////////////////////////////////////////////////////////////////////
        Node&                 node         () const { return *mCurrent; }

  private:
          iterator&       next    () { mCurrent = mCurrent ? (Node*)&mCurrent->IterNext () : NULL; return *this; }
          iterator&       prev    () { mCurrent = mCurrent ? (Node*)&mCurrent->IterPrev () : NULL; return *this; }

  protected:
          Node*              mCurrent;
};

template <class Iter> class dataiterator: public Iter
{
  public:
                  dataiterator () {}
                  dataiterator (const Iter& i): Iter (i) {}

        const typename Iter::IData*  operator ->    () const { return &data(); } 
              typename Iter::IData*  operator ->    ()       { return &data(); } 

                      operator typename Iter::IData* () const { return &data(); }
};

#include <misc\iter.inl>

#endif

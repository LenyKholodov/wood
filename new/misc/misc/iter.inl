template <typename T,typename TD,class List> 
iterator<T,TD,List>& iterator<T,TD,List>::operator  += (int c)
{
  if (c > 0) for (int i=0;i<c;next ());
  else       for (int i=0;i<-c;prev ());

  return *this;
}

template <typename T,typename TD,class List> 
iterator<T,TD,List>& iterator<T,TD,List>::operator  -= (int c)
{
  if (c > 0) for (int i=0;i<c;prev ());
  else       for (int i=0;i<-c;next ());

  return *this;
}

template <typename T,typename TD,class List> 
iterator<T,TD,List> iterator<T,TD,List>::operator  + (const iterator<T,TD,List>& i,int c)
{
  iterator<T,TD,List> res = i;

  return res += c;
}

template <typename T,typename TD,class List> 
iterator<T,TD,List> iterator<T,TD,List>::operator  - (const iterator<T,TD,List>& i,int c)
{
  iterator<T,TD,List> res = i;

  return res -= c;
}

template <typename T,typename TD,class List>
iterator<T,TD,List>& iterator<T,TD,List>::operator = (const List& l)
{
  mCurrent = l.GetFirst ().mCurrent;
}

template <typename T,typename TD,class List>
iterator<T,TD,List>& iterator<T,TD,List>::operator = (const Node& n)
{
  mCurrent = (Node*)&n;
  return *this;
}

template <typename T,typename TD,class List>
iterator<T,TD,List>& iterator<T,TD,List>::operator = (const Node* n)
{
  mCurrent = (Node*)n;
  return *this;
}

template <typename T,typename TD,class List>
TD&     iterator<T,TD,List>::data () const throw (ExcNoElement&) 
{ 
  if (mCurrent) return mCurrent->IterData ();
  else          throw  ExcNoElement ("iterator::data");  
}

template <typename T,typename TD,class List> 
iterator<T,TD,List>::iterator ()
                 : mCurrent (NULL)
 {   }

template <typename T,typename TD,class List> 
iterator<T,TD,List>::iterator (const T& node)
                 : mCurrent ((T*)&node)
 {   }

template <typename T,typename TD,class List> 
iterator<T,TD,List>::iterator (const List& l)
                    : mCurrent ((Node*)&l.GetFirst ().node ())
 {   }


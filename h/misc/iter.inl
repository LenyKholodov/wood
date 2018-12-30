template <typename Node,typename Data,class List> 
iterator<Node,Data,List>& iterator<Node,Data,List>::operator  += (int c)
{
  if (c > 0) for (int i=0;i<c;next (),i++);
  else       for (int i=0;i<-c;prev (),i++);

  return *this;
}

template <typename Node,typename Data,class List> 
iterator<Node,Data,List>& iterator<Node,Data,List>::operator  -= (int c)
{
  if (c > 0) for (int i=0;i<c;prev ()),i++;
  else       for (int i=0;i<-c;next (),i++);

  return *this;
}

template <typename Node,typename Data,class List> 
iterator<Node,Data,List> operator  + (const iterator<Node,Data,List>& i,int c)
{
  iterator<Node,Data,List> res = (iterator<Node,Data,List>&)i;

  return res += c;
}

template <typename Node,typename Data,class List> 
iterator<Node,Data,List> operator  - (const iterator<Node,Data,List>& i,int c)
{
  iterator<Node,Data,List> res = (iterator<Node,Data,List>&)i;

  return res -= c;
}

template <typename Node,typename Data,class List>
iterator<Node,Data,List>& iterator<Node,Data,List>::operator = (const List& l)
{
  mCurrent = l.GetFirst ().mCurrent;
  return *this;
}

template <typename Node,typename Data,class List>
iterator<Node,Data,List>& iterator<Node,Data,List>::operator = (const Node& n)
{
  mCurrent = (Node*)&n;
  return *this;
}

template <typename Node,typename Data,class List>
iterator<Node,Data,List>& iterator<Node,Data,List>::operator = (const Node* n)
{
  mCurrent = (Node*)n;
  return *this;
}

template <typename Node,typename Data,class List>
Data&     iterator<Node,Data,List>::data () const throw (ExcNoElement&) 
{ 
  if (mCurrent) return mCurrent->IterData ();
  else          throw  ExcNoElement ("iterator::data");  
}

template <typename Node,typename Data,class List> 
iterator<Node,Data,List>::iterator ()
                 : mCurrent (NULL)
 {   }

template <typename Node,typename Data,class List> 
iterator<Node,Data,List>::iterator (const Node& node)
                 : mCurrent ((Node*)&node)
 {   }

template <typename Node,typename Data,class List> 
iterator<Node,Data,List>::iterator (const List& l)
                    : mCurrent ((Node*)&l.GetFirst ().node ())
 {   }


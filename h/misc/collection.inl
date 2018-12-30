template <class T,class Base> Collection<T,Base>::Collection (Pool* pool)
                           : MemObject (pool)
{  }

template <class T,class Base> Collection<T,Base>::~Collection ()
{
  int count    = this->count ();
  node_t* node = (node_t*)&GetFirst ().node ();

  if (node)
  {
    node_t* next = (node_t*)node->next;

    for (int i=0;i<count;i++,node=next,next = (node_t*)next->next)
      node_t::dispose (node,GetPool ());
//      free (node);
  }   
}

template <class T,class Base> bool Collection<T,Base>::InsertFirst (const T& obj,bool exc) throw (ExcNoMemory&) 
{ 
  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("Collection::insert",sizeof (node_t),this);
    return false;
  }  

  Base::InsertFirst (*node);
               
  return true; 
}

template <class T,class Base> bool Collection<T,Base>::InsertLast (const T& obj,bool exc) throw (ExcNoMemory&) 
{ 
  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("Collection::insert",sizeof (node_t),this);
    return false;
  }  

  Base::InsertLast (*node);
               
  return true; 
}

template <class T,class Base> bool  Collection<T,Base>::InsertAfter (iterator& i,iterator& ins) 
{
  return Base::InsertAfter (i.node (),ins.node ());
}

template <class T,class Base> bool  Collection<T,Base>::InsertBefore (iterator& i,iterator& ins) 
{
  return Base::InsertBefore (i.node (),ins.node ());
}

template <class T,class Base> 
bool  Collection<T,Base>::InsertAfter (iterator& i,const T& obj,bool exc) throw (ExcNoMemory&) 
{
  if (!&i.node ()) return false;

  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("Collection::InsertAfter",sizeof (node_t),this);
    return false;              
  }  

  return InsertAfter (i,iterator (*node));
}

template <class T,class Base> 
bool  Collection<T,Base>::InsertBefore (iterator& i,const T& obj,bool exc) throw (ExcNoMemory&)
{
  if (!&i.node ()) return false;

  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("Collection::InsertBefore",sizeof (node_t),this);
    return false;              
  }  

  return InsertBefore (i,iterator (*node));
}

template <class T,class Base> typename Collection<T,Base>::iterator Collection<T,Base>::find (const T& obj) const
{
  iterator iter = *this;

  for (int i=0;i<count();i++,iter++)
//    if (iter.data() == obj)
    if (!memcmp (&iter.data(),&obj,sizeof (T))) //убрать после перехода к норм. рендеру
      return iter;

  return iterator ();
}

template <class T,class Base> void Collection<T,Base>::erase  (typename Collection<T,Base>::iterator& iter,bool autoDel)
{
  node_t& node = iter.node ();

  if (!&node)
    return;    

  Base::Erase  (node);

  if (autoDel)
  {
    iter++;
    node_t::dispose (&node,GetPool ());
//    free (&node);
  }
}

template <class T,class Base> void Collection<T,Base>::clear ()
{  
  int cnt = mCount;

  for (int i=0;i<cnt;i++)
    erase (GetFirst ());
}

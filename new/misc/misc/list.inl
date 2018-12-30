template <class T> List<T>::List (Pool* pool)
                           : MemObject (pool)
{  }

template <class T> List<T>::~List ()
{
  int count = this->count (), i = 0;
  for (iterator iter = *this;i<count;erase(iter),i++);
}

template <class T> bool List<T>::InsertFirst (const T& obj,bool exc) throw (ExcNoMemory&) 
{ 
  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("List::insert",sizeof (node_t),this);
    return false;
  }  

  BaseList::InsertFirst (*node);
               
  return true; 
}

template <class T> bool List<T>::InsertLast (const T& obj,bool exc) throw (ExcNoMemory&) 
{ 
  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("List::insert",sizeof (node_t),this);
    return false;
  }  

  BaseList::InsertLast (*node);
               
  return true; 
}

template <class T> bool  List<T>::InsertAfter (iterator& i,iterator& ins) 
{
  return BaseList::InsertAfter (i.node (),ins.node ());
}

template <class T> bool  List<T>::InsertBefore (iterator& i,iterator& ins) 
{
  return BaseList::InsertBefore (i.node (),ins.node ());
}

template <class T> 
bool  List<T>::InsertAfter (iterator& i,const T& obj,bool exc) throw (ExcNoMemory&) 
{
  if (!&i.node ()) return false;

  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("List::InsertAfter",sizeof (node_t),this);
    return false;              
  }  

  return InsertAfter (i,iterator (*node));
}

template <class T> 
bool  List<T>::InsertBefore (iterator& i,const T& obj,bool exc) throw (ExcNoMemory&)
{
  if (!&i.node ()) return false;

  node_t* node = node_t::create (obj,GetPool());

  if (!node)
  {
    if (exc) throw ExcNoMemory ("List::InsertBefore",sizeof (node_t),this);
    return false;              
  }  

  return InsertBefore (i,iterator (*node));
}

template <class T> List<T>::iterator List<T>::find (const T& obj) const
{
  iterator iter = *this;

  for (int i=0;i<count();i++,iter++)
//    if (iter.data() == obj)
    if (!memcmp (&iter.data(),&obj,sizeof (T))) //убрать после перехода к норм. рендеру
      return iter;

  return iterator ();
}

template <class T> void List<T>::erase  (List<T>::iterator& iter,bool autoDel)
{
  node_t& node = iter.node ();

//  if (!&node || &node == &mNull || &node == (listnode_t*)&mFirst || &node == (listnode_t*)&mLast)
  if (!&node)
    return;    

  BaseList::Erase  (node);

  if (autoDel)
  {
    iter++;
    free (&node);
//    iter = (node_t*)NULL;
  }
}

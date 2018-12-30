template <class T,class key_t>
typename BinTree<T,key_t>::node_t* BinTree<T,key_t>::_find (const key_t& key) const
{
  for (node_t* node = (node_t*)&RedBlackTree::GetRoot ();node->valid ();)
    if      (key == node->key)  return node;    
    else 
      if (key >  node->key)  node = (node_t*)node->right;
      else                   node = (node_t*)node->left;         

  return NULL;    
}

template <class T,class key_t>
typename BinTree<T,key_t>::iterator  BinTree<T,key_t>::GetMin ()  const
{
  node_t& node = (node_t&)RedBlackTree::GetMin ();

  return iterator (node.valid () ? node : *(node_t*)NULL);
}

template <class T,class key_t>
typename BinTree<T,key_t>::iterator  BinTree<T,key_t>::GetMax ()  const
{
  node_t& node = (node_t&)RedBlackTree::GetMax ();

  return iterator (node.valid () ? node : *(node_t*)NULL);
}

template <class T,class key_t>
typename BinTree<T,key_t>::iterator  BinTree<T,key_t>::find  (const key_t& key) const
{
  return iterator (*_find (key));
}

template <class T,class key_t>
bool  BinTree<T,key_t>::find  (const key_t& key,T& res) const
{
  node_t* node = _find (key);

  if (node)
  {
    res = node->data;

    return true;
  }
  else return false;  
}

template <class T,class key_t>
bool  BinTree<T,key_t>::present  (const key_t& key) const
{
  return _find (key) != NULL;
}

template <class T,class key_t>
bool  BinTree<T,key_t>::insert  (const key_t& key,const T& data,bool exc) throw (ExcNoMemory&)
{
  node_t* parent = NULL;

  if (RedBlackTree::GetRoot ().valid ())
  {
    for (parent = (node_t*)&RedBlackTree::GetRoot ();parent->valid ();)
    {
      if (key == parent->key)
      {
        parent->data = data; //already exist
        return true;
      }  
      else 
        if (key < parent->key)  
           if (parent->left->valid ())   parent = (node_t*)parent->left;
           else                        break;
        else if (key > parent->key) 
               if (parent->right->valid ())  parent = (node_t*)parent->right;
               else                        break;               
    }           
  }

  node_t* ins = node_t::create (key,data,GetPool ());

  if (!ins)
  {
    if (exc)
      throw ExcNoMemory ("BinTree::insert",sizeof (node_t),this);      
    return false;
  }  

  ins->parent = parent;

  if (parent->valid ())
  {
    if (key > parent->key) parent->right = ins;
    else                   parent->left  = ins;
  }  
  else
    RedBlackTree::SetRoot (*ins); 

  RedBlackTree::insert (*ins);

  return true;
}

template <class T,class key_t>
void   BinTree<T,key_t>::erase (iterator& iter,bool del)
{
  if (iter.valid ())
  {
    node_t& node = (node_t&)iter.node ();

    if (node.valid ()) 
    {
      RedBlackTree::erase (node);

      if (del)
      {
        iter++;
        node_t::dispose (&node,GetPool ());
//        free (&node);
      }  
    }  
  }  
}

template <class T,class key_t>
void      BinTree<T,key_t>::clear    (node_t* node)
{
  if (node->left->valid ())  clear ((node_t*)node->left);
  if (node->right->valid ()) clear ((node_t*)node->right);

  node_t::dispose (node,GetPool ());
//  free (node);
}

template <class T,class key_t>
void      BinTree<T,key_t>::clear    ()
{
  if (RedBlackTree::GetRoot ().valid ())
  {
    clear  ((node_t*)&RedBlackTree::GetRoot ());
    reset  ();
  }
}

template <class T>
bool StringDict<T>::insert  (const char* s,const T& x,bool exc) throw (ExcNoMemory&)
{
  return BinTree<T,ulong>::insert (toKey(s),x,exc);
}

template <class T>
typename StringDict<T>::iterator  StringDict<T>::find    (const char* s)    const
{
  return BinTree<T,ulong>::find (toKey (s));
}

template <class T>
bool      StringDict<T>::find    (const char* s,T& x) const
{
  return BinTree<T,ulong>::find (toKey (s),x);
}

template <class T>
void StringDict<T>::erase (const char* x) 
{ 
  BinTree<T,ulong>::erase (BinTree<T,ulong>::find (toKey (x))); 
}

template <class T>
void StringDict<T>::erase (BinTree<T,ulong>::iterator& i,bool free_mem) 
{ 
  BinTree<T,ulong>::erase (i,free_mem); 
}

template <class T>
bool StringDict<T>::search  (const char* key,T& res) const 
{ 
  return find (key,res); 
}

template <class T>
typename BinTree<T,ulong>::iterator  StringDict<T>::search  (const char* key) const        
{ 
  return find (key); 
}

template <class T>
bool StringDict<T>::present (const char* x) const  
{ 
  return present (toKey (x)); 
}

template <class T>
bool StringDict<T>::insert  (ulong key,const T& x,bool exc) throw (ExcNoMemory&)
{
  return BinTree<T,ulong>::insert (key,x,exc);
}

template <class T>
typename StringDict<T>::iterator  StringDict<T>::find  (ulong key)    const
{
  return BinTree<T,ulong>::find (key);
}

template <class T>
bool      StringDict<T>::find    (ulong key,T& x) const
{
  return BinTree<T,ulong>::find (key,x);
}

template <class T>
void StringDict<T>::erase (ulong x) 
{ 
  BinTree<T,ulong>::erase (BinTree<T,ulong>::find (x)); 
}

template <class T>
bool StringDict<T>::search  (ulong key,T& res) const 
{ 
  return find (key,res); 
}

template <class T>
typename BinTree<T,ulong>::iterator  StringDict<T>::search  (ulong key) const        
{ 
  return find (key); 
}

template <class T>
bool StringDict<T>::present (ulong x) const  
{ 
  return present (x); 
}

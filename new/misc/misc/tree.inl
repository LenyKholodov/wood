template <class T,class key_t>
BinTree<T,key_t>::node_t* BinTree<T,key_t>::_find (const key_t& key) const
{
  for (node_t* node = (node_t*)&GetRoot ();node->valid ();)
    if      (key == node->key)  return node;    
    else 
      if (key >  node->key)  node = (node_t*)node->right;
      else                   node = (node_t*)node->left;         

  return NULL;    
}

template <class T,class key_t>
BinTree<T,key_t>::iterator  BinTree<T,key_t>::GetMin ()  const
{
  node_t& node = (node_t&)RedBlackTree::GetMin ();

  return iterator (node.valid () ? node : *(node_t*)NULL);
}

template <class T,class key_t>
BinTree<T,key_t>::iterator  BinTree<T,key_t>::GetMax ()  const
{
  node_t& node = (node_t&)RedBlackTree::GetMax ();

  return iterator (node.valid () ? node : *(node_t*)NULL);
}

template <class T,class key_t>
void    BinTree<T,key_t>::swap (mapnode_t& _dst,mapnode_t& _src)
{
  node_t &src = (node_t&)_src,
         &dst = (node_t&)_dst;

  dst.data    = src.data;
  dst.key     = src.key;  
}

template <class T,class key_t>
BinTree<T,key_t>::iterator  BinTree<T,key_t>::find  (const key_t& key) const
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

  if (GetRoot ().valid ())
  {
    for (parent = (node_t*)&GetRoot ();parent->valid ();)
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
        free (&node);
      }  
    }  
  }  
}

template <class T,class key_t>
void      BinTree<T,key_t>::clear    (node_t* node)
{
  if (node->left->valid ())  clear ((node_t*)node->left);
  if (node->right->valid ()) clear ((node_t*)node->right);

  free (node);
}

template <class T,class key_t>
void      BinTree<T,key_t>::clear    ()
{
  if (GetRoot ().valid ())
  {
    clear  ((node_t*)&GetRoot ());
    reset  ();
  }
}

template <class T>
bool      StringDict<T>::insert  (const char* s,const T& x,bool exc) throw (ExcNoMemory)
{
  return BinTree<T,ulong>::insert (toKey(s),x,exc);
}

template <class T>
StringDict<T>::iterator  StringDict<T>::find    (const char* s)    const
{
  return BinTree<T,ulong>::find (toKey (s));
}

template <class T>
bool      StringDict<T>::find    (const char* s,T& x) const
{
  return BinTree<T,ulong>::find (toKey (s),x);
}
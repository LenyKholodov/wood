template <class Type> inline bool DictionaryTmpl<Type>::insert (DictionaryTmpl<Type>::key_t id,const Type& data)
{
  treenode_t* node = NULL;

  if (mNode)
  {
    for (node = mNode;node;)
      if      (id == node->id)               
      {
        node->data = data;
        return true;
      }  
      else if (id < node->id)  
           if (node->left)       node = node->left;
           else                  break;
      else if (id > node->id) 
          if (node->right)      node = node->right;
          else                  break;               
  }

  treenode_t* ins = (treenode_t*)alloc (sizeof(treenode_t));

  if (!ins)
    return false;

  ins->left = ins->right = NULL;
  ins->prev = node;
  ins->id   = id;
  ins->data = data;

  if (node)
    if (id > node->id) node->right = ins;
    else               node->left  = ins;
  else
    mNode   = ins;

  mCount++;

  return true;
}

template <class Type> inline void DictionaryTmpl<Type>::erase (DictionaryTmpl<Type>::iterator& iter)
{
  if (iter.data()) 
  {
    treenode_t* prev = iter.mCurrent->prev;

    erase (iter.mCurrent->id);

    iter.mCurrent = prev;
  }
}

template <class Type> inline void DictionaryTmpl<Type>::erase      (DictionaryTmpl<Type>::key_t id)
{
  treenode_t* del  = find (id);
  treenode_t* node = NULL;
  treenode_t* x    = NULL;
  
  if (del == NULL)  return ;
  if (del->left == NULL || del->right == NULL)  node = del;
  else                                          node = successor (del);

  if (!node) return ;
  if (node->left) x = node->left;
  else            x = node->right;

  if (x)                  x->prev = node->prev;
  if (node->prev == NULL) mNode   = x;
  else if (node == node->prev->left) node->prev->left  = x;
       else                          node->prev->right = x;
 
  if (node != del)        
  {
    del->id   = node->id;
    del->data = node->data;
  }

  free (node);

  mCount--;
}

template <class Type> inline Type& DictionaryTmpl<Type>::GetMax () const
{
  treenode_t* node = mNode;

  if (mNode) for (;node->right;node=node->right);
  else       throw ExcNoElement ("Dict::GetMax");

  return node->data;
}

template <class Type> inline Type& DictionaryTmpl<Type>::GetMin () const
{
  treenode_t* node = mNode;

  if (mNode) for (;node->left;node=node->left);  
  else       throw ExcNoElement ("Dict::GetMin");
  
  return node->data;
}

template <class Type> inline 
         DictionaryTmpl<Type>::treenode_t* 
         DictionaryTmpl<Type>::GetMin (DictionaryTmpl<Type>::treenode_t* _node) const
{
  if (_node)  
  {
    for (treenode_t* node = _node;node->left;node=node->left);
    return node;
  }  
  else        return NULL;

}

template <class Type> inline 
         DictionaryTmpl<Type>::treenode_t* 
         DictionaryTmpl<Type>::GetMax (DictionaryTmpl<Type>::treenode_t* _node) const
{
  if (_node) 
  {
    for (treenode_t* node = _node;node->right;node=node->right);
    return node;
  }
  else       return NULL;
}

template <class Type> 
inline Type& DictionaryTmpl<Type>::operator[] (DictionaryTmpl<Type>::key_t id) const throw (ExcNoElement&)
{
  return search (id); 
}

template <class Type> 
inline Type& DictionaryTmpl<Type>::operator[] (const char* s)  const throw (ExcNoElement&)
{
  return search (CRC32 (s,strlen (s)));
}

template <class Type> inline Type& DictionaryTmpl<Type>::search (DictionaryTmpl<Type>::key_t id) const throw (ExcNoElement&)
{
  treenode_t*   node = find (id);

  if (node)     return node->data;
  else          throw  ExcNoElement ("Dict::search");
}

template <class Type> 
inline bool DictionaryTmpl<Type>::search     (DictionaryTmpl<Type>::key_t id,Type& data) const
{
  treenode_t*   node = find (id);

  if (node)     
  {
    data = node->data;
    return true;
  } 
  else      
    return false;
}

template <class Type> 
inline bool DictionaryTmpl<Type>::search     (DictionaryTmpl<Type>::key_t id,Type*& data) const
{
  treenode_t*   node = find (id);

  if (node)     
  {
    data = &node->data;
    return true;
  } 
  else      
    return false;
}
  
template <class Type> inline bool DictionaryTmpl<Type>::present    (DictionaryTmpl<Type>::key_t id) const
{
  return find (id) != NULL;
}

template <class Type> inline DictionaryTmpl<Type>::treenode_t* DictionaryTmpl<Type>::find (DictionaryTmpl<Type>::key_t id) const
{  
  for (treenode_t* node = mNode;node;)
    if      (id == node->id)  return node;    
    else if (id >  node->id)  node = node->right;
    else                      node = node->left;         

  return NULL;
}

template <class Type> inline DictionaryTmpl<Type>::treenode_t* DictionaryTmpl<Type>::successor  (DictionaryTmpl<Type>::treenode_t* node) const
{
  if (node == NULL)  return NULL;  
  if (node->right)   return GetMin (node->right);

  for (treenode_t* prev = node->prev;prev && node == prev->right;
       node = prev,prev = prev->prev);

  return prev;
}

template <class Type> void DictionaryTmpl<Type>::clear (DictionaryTmpl<Type>::treenode_t* node)
{
  if (node == NULL) return  ;
  if (node->left)   clear (node->left);
  if (node->right)  clear (node->right);
  
  free (node);
}

template <class Type>
DictionaryTmpl<Type>::DictionaryTmpl  (Pool* _Pool)
           : MemObject (_Pool),
             mNode     (NULL),
             mCount    (0)
{    }

template <class Type>
DictionaryTmpl<Type>::DictionaryTmpl  ()
           : MemObject (::M_GetPool()),
             mNode     (NULL),
             mCount    (0)
{    }

template <class Type> DictionaryTmpl<Type>::~DictionaryTmpl ()
{
  if (mNode) clear (mNode);
  mNode = NULL;
}
////////////////////implementation of class VertexCache/////////////////////////

/*int        VertexCache::set             (int index,uint count)
{
}

vcnode_t*       VertexCache::alloc           (uint count)
{
  vcnode_t* node = mFirstFree;

  if (mFirstFree)
  {
    for (;node && node->count < count;node=node->nextFree);

    if (node->count < count)
      return NULL;
    else
    {
      vcnode_t* newNode = mNodePool.alloc ();

      if (!newnode)
        return NULL;

      node->use          = FALSE;  
      node->count       -= count;
      node->base        += count;

      newNode->count     = count;
      newNode->base      = node->base;
      newNode->prevNode  = node->prevNode;
      newNode->nextNode  = node;      
      newNode->prevFree  = NULL;
      newNode->nextFree  = NULL;
      newNode->use       = TRUE;
      newNode->prev      = &mFirst;
      newNode->next      = mFirst.next;
      mFirst.next->prev  = newNode;
      mFirst.next        = newNode;      

      if (node->prevNode) node->prevNode->nextNode = newNode;

      node->prevNode     = newNode;      

      free (node);

      return newNode;
    }
  }
  else
    return NULL;
}

void            VertexCache::free            (vcnode_t* node)
{
  node->use  = FALSE;

  if (node->prevNode && !node->prevNode->use)
  {
    vcnode_t* left  = node->prevNode;

    node->count    += left->count;
    node->base      = left->base;
    node->prevNode  = left->prevNode;
    
    left->prev->next = left->next;
    left->next->prev = left->prev;

    if (left->prevNode) left->prevNode->nextNode = node;

    mNodePool.free (left);
  }

  if (node->nextNode && !node->nextNode->use)
  {
    vcnode_t* right = node->nextNode;

    node->count    += right->count;
    node->nextNode  = right->nextNode;
    
    right->prev->next = right->next;
    right->next->prev = right->prev;

    if (right->nextNode) right->nextNode->prevNode = node;

    mNodePool.free (right);
  }

  for (vcnode_t* x = node->prevNode;x && x->use;x=x->prevNode);

  if (x)
  {
    node->prevFree  = x;
    node->nextFree  = x->nextNode;
    if (x->nextFree) x->nextFree->prevFree = node;
    x->nextFree     = node;
  }
  else
  {
    node->nextFree  = mFirstFree;
    node->prevFree  = NULL;

    if (mFirstFree) mFirstFree->prevFree = node;

    mFirstFree      = node;   
  }

  node->prev->next = node->next;
  node->next->prev = node->prev;

  node->prev       = mLast.prev;
  mLast.prev->next = node;
  mLast.prev       = node;
}

void       VertexCache::defrag          () //нет
{
  if (mFirstFree)   
  {
    for (vcnode_t* node = mFirstFree;node;node=mFirstFree)
    {
      vcnode_t* left  = node;
      vcnode_t* right = node->nextNode;

      //left - free; right - data
             
      if (right == NULL)
        continue;           
            
      vcnode_t   temp      = *left;

      left->count          = right->count;
      right->count         = temp.      

      memmove (left,right,right->size);   //memcpy

      //left - data; right - free

      right             = (freedesc_t*)((char*)left+left->size);

      left->prev        = temp.prev;
      *right            = temp;
      right->prev       = left;
      
      if (right->prevFree) right->prevFree->nextFree  = right;  
      else                 mFirstFree                 = right; 
      if (right->nextFree) right->nextFree->prevFree  = right;

      for (;this->next (right) && right->nextFree == this->next (right);right = _concat (right,this->next (right)));
        
      return true;        
    }
  }    
}

vcnode_t*  VertexCache::AllocChunk      (uint count)
{
  D_Try
  {
    vcnode_t* node = mLast.prev;

    if (node->count < count)
    {
      uint _free = node->count;

      for (;node!=&mFirst && _free < count;node=node->prev,_free += node->count)
      {

      }  

      if (node->count < count)
        defrag ();
    }

  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"VertexCache: Error at AllocChunk"<<endl;
    #endif  
    return NULL;
  }
}

VertexCache::VertexCache     (Pool*,VertexBuffer*,int count,int indexCount)
{
}

VertexCache::~VertexCache    ()
{
}
  */
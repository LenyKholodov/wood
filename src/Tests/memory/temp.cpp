size_t  FloatPool::_correct (size_t bytes)
{
  dout<<"BYTES "<<bytes<<endl;

  for (int i=0;bytes>0;bytes>>=1,i++);

  int size = 1<<i;

  dout<<"Hey-ho! "<<size<<" i = "<<i<<endl;

//  return (1<<(i-1) > bytes) ? 1<<(i-1) : 1<<i;
  return 1<<i;
}

void         Cache::defrag          (void)
{
  D_Try
  {
    for (resource_t* node = mFirst->next;node && node!=mLast;node=node->next)
    {
      if (node->size != 0 && node->id != RESOURCE_EMPTY)
      {      
        if (node->prev->size != 0 && node->prev->id == RESOURCE_EMPTY)
          node              = node->prev;
        if (node->next->size != 0 && node->next->id == RESOURCE_EMPTY)
        {    
          node->size       += node->next->size;
          node->next->prev  = node;
          node->next        = node->next->next;
        }
      }  
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Cache: Error at cache->defrag () "<<this<<endl;
    #endif
  }
}

void         FloatCache::dump            (ostream& os)
{
  os<<"Float cache dump:"<<endl;
 
  for (resource_t* node = (resource_t*)lump();node;node = NEXT_LUMP(node))
  {
    os<<"Node:\t"<<node<<" (size: "<<node->size<<" prev: "<<PREV_LUMP(node)
      <<" next: "<<NEXT_LUMP (node)<<")"<<endl;
  }  

  os<<endl;
}

void         Cache::SetLast         (resource_t* res)
{
  D_Try
  {
    res->next       = mLast;
    res->prev       = mLast->prev;
    mLast->prev     = mLast->prev->next = res;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Cache: Error at SetLast () "<<this<<endl;
    #endif
  }
}

void         Cache::SetFirst        (resource_t* res)
{
  D_Try
  {
    res->prev       = mFirst;
    res->next       = mFirst->next;

    mFirst->next    = mFirst->next->prev = res;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Cache: Error at SetFirst () "<<this<<endl;
    #endif
  }
}

  FloatPool pool (M_GetPool(),4*1024*1024);

  M_PushPool (&pool);

  void *  x = new char [2*1024*1024];
  void *  y = new char [4*1024*1024];

  cout<<x<<" "<<y<<endl;

  M_PopPool  ();

///////////////////////////implementation of class DefragCache//////////////////

#define IsIndexValid(index) ((mNode && index < mNum) || !mNode)

inline  dcachenode_t*   DefragCache::GetNode      (uint index)
{
  if (mNode && index < mNum)
    return mNode [index];
  else
  {
    for (cachenode_t* node=mFirst.next;node!=&mLast && node->index!=index;node=node->next);
    if  (node->index != index) node = NULL; 
    return (dcachenode_t*)node;
  }
}

void            DefragCache::FreeChunk    (dcachenode_t* node)
{
  if (node)
  {
    FloatCache::FreeChunk (node);
    mNode [node->index] = NULL; 
  }
}

void*           DefragCache::lock         (uint index)
{
  D_Try
  {
    if (IsIndexValid (index))
    {   
      dcachenode_t* node = GetNode (index);

      if (!node)  
        if ( !(node = LoadData (index)) ) return NULL;
        else if (mNode) mNode [index] = node;

      FloatCache::start (node);
      
      if (!(mFlags & NO_LOCK)) FloatCache::lock (node);

      node->index = node->param = index;      
        
      return node->buf;            
    }

    return NULL;
  }
  D_ExcRet ("DefragCache: Error at lock",NULL);
}

void*           DefragCache::get          (uint index)
{
  D_Try
  {
    if (IsIndexValid (index))
    {
      dcachenode_t* node = GetNode (index);

      if (!node)  
        if ( !(node = LoadData (index)) ) return NULL;
        else if (mNode) mNode [index] = node;

      FloatCache::start (node);

      node->index = node->param = index;
        
      return node->buf;            
    }

    return NULL;
  }
  D_ExcRet ("DefragCache: Error at lget",NULL);
}

void            DefragCache::unlock       (uint index)
{
  if (IsIndexValid (index))
    FloatCache::unlock (GetNode (index));
}

void            DefragCache::start        (uint index)
{
  if (IsIndexValid (index))  
    FloatCache::start (GetNode (index));
}

void            DefragCache::end          (uint index)
{
  if (IsIndexValid (index))  
    FloatCache::end (GetNode (index));
}

DefragCache::DefragCache (Pool* _Pool,size_t _CacheSize,int _Num,uint _Flags)
            : FloatCache  (_Pool,_CacheSize,_Flags),
              mNum        (_Num),
              mNode       (NULL)
{
  D_Try
  {
    if (mNum)
    {
      mNode = (dcachenode_t**)alloc (sizeof (dcachenode_t*) * mNum);

      if (!mNode)
      {
        D_Message ("DefragCache: No enough memory for sys data");
        return;
      }

      memset (mNode,0,sizeof (dcachenode_t*) * mNum);
    }
  }
  D_Exc ("DefragCache: Error at construct");
}

DefragCache::~DefragCache ()
{
  D_Try
  {
    if (mNode) free (mNode);
  }
  D_Exc ("DefragCache: Error at destruct");
}

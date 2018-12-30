#include <pch.h>
#include <pool.h>
#include <malloc.h>

accessptr_t<MemoryService> memoryService;

#pragma pack (1)

struct mallocnode_t
{
  Pool*  pool;
  char   data [];
};

#pragma pack ()

extern void*  StdAlloc (size_t size)
{
  mallocnode_t* node = (mallocnode_t*)malloc (size+sizeof (mallocnode_t));

  if (!node)
    return NULL;

  node->pool = NULL;

  return node->data;
}

extern void   StdFree  (void* ptr)
{
  mallocnode_t* node = (mallocnode_t*)((char*)ptr-sizeof (mallocnode_t));

  if (ptr && node && !node->pool)
    free (node);
}

void*       MemoryService::AllocChunk  (size_t size)
{
  return memoryService->alloc (size);
}

void        MemoryService::FreeChunk   (void* x)
{
  memoryService->free (x);
}

void*      MemoryService::alloc         (size_t bytes)
{
  mallocnode_t* node = (mallocnode_t*)mPoolStack [mPoolPos]->alloc (bytes+sizeof (mallocnode_t));

  if (!node)
    return NULL;

  node->pool = mPoolStack [mPoolPos];

  return node->data;
}

void*      MemoryService::alloc         (size_t bytes,Pool* pool)
{
  mallocnode_t* node = (mallocnode_t*)pool->alloc (bytes+sizeof (mallocnode_t));

  if (!node)
    return NULL;

  node->pool = pool;

  return node->data;
}

void        MemoryService::free          (void* ptr)
{
  D_Try
  {
    mallocnode_t* node = (mallocnode_t*)((char*)ptr-sizeof (mallocnode_t));

    if (ptr && node)
      if (node->pool)  node->pool->free (node);
      else             free             (node);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
}

bool       MemoryService::PushPool      (Pool* pool)
{
  if (mPoolPos >= POOL_STACK_SIZE - 1)  return false;

  mPoolStack [++mPoolPos] = pool;

  return true; 
}

void        MemoryService::PopPool       ()
{
  if (--mPoolPos < 0) mPoolPos = 0;
}

MemoryService::MemoryService  (Pool* pool,size_t size)
           : Service   (&memoryService),
             mPoolPos  (0),
             mMainPool (size ? new (pool) FloatPool (pool,size) : pool)
{
  mPoolStack [0] = mMainPool;

  if (!size)
    mMainPool = NULL;
}

MemoryService::~MemoryService ()
{
  if (mMainPool) delete mMainPool;
}

////////////////////////////////////////////////////////////////////////////////

MemoryService*   M_Load   (Pool* pool,size_t size)
{
  return new (pool) MemoryService (pool,size);
}


void   M_Done  (MemoryService* srv)
{
  if (srv) delete srv;
}

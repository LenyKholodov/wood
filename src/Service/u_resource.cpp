#include <pch.h>
#include <service\resource.h>

Resource*       ResourceService::GetResource (const char* name) const
{
  StringDict<Resource*>::iterator iter = mResources.find (name);

  return iter.valid () ? iter.data () : NULL;
}

bool            ResourceService::AddResource (const char* name,Resource* res)
{
  return mResources.insert (name,res);
}

void            ResourceService::DelResource (const char* name)
{
  mResources.erase (name);
}

ResourceGroup*  ResourceService::GetResGroup (const char* name) const
{
  StringDict<ResourceGroup*>::iterator iter = mResGroups.find (name);

  return iter.valid () ? iter.data () : NULL;  
}

bool            ResourceService::AddResGroup (const char* name,ResourceGroup* group)
{
  return mResGroups.insert (name,group);
}

void            ResourceService::DelResGroup (const char* name)
{
  mResGroups.erase (name);
}

ResourceService::ResourceService  (Pool* pool,size_t size)
                : Service   (&resourceService),
                  mMainPool (size ? pool = new FloatPool (pool,size) : pool),
                  mResources (pool),
                  mResGroups (pool),
                  mCreatePool (size?true:false)
{   }

ResourceService::~ResourceService ()
{   
  if (!mCreatePool)
    mMainPool = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ResourceService*     RES_Load       (Pool* pool,size_t size)
{
  return new (pool) ResourceService (pool,size);
}

void             RES_Done       (FileService* srv)
{
  if (srv) delete srv;
}


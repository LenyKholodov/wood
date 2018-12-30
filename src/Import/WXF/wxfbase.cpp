#include <pch.h>
#include <import\wxf\wxfimp.h>

WXFBase::WXFBase (WXFImport* imp)
        : owner (imp)
{   }

Pool*   WXFBase::GetPool () const
{
  return owner->GetPool ();
}

void*   WXFBase::operator new (size_t size,WXFImport* imp)
{
  return ::operator new (size,imp->GetPool ());
}

void    WXFBase::operator delete (void* obj)
{
  ::operator delete (obj);
}
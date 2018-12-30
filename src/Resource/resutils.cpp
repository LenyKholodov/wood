#include <pch.h>
#include <memory\resutils.h>
#include <binfile.h>

bool   FileResource::scan () 
{
  autoptr_t<BinFile> file (NULL);

  if (mResGroup) file = F_OpenFile (mFileName,BF_OPEN_RO,mResGroup);
  else           file = F_OpenFile (mFileName,BF_OPEN_RO);

  if (!IsValid (file))
  {
    set (WRONG);
    return false;
  }    

  size_t size = check (file);

  if (!size)
  {
    set (WRONG);
    return false;
  }  

  clear (WRONG);

  return true;
}

void*  FileResource::load ()
{
  autoptr_t<BinFile> file (NULL);

  if (mResGroup) file = F_OpenFile (mFileName,BF_OPEN_RO,mResGroup);
  else           file = F_OpenFile (mFileName,BF_OPEN_RO);

  if (!IsValid (file))
  {
    D_Message ("FileResource: File not found '"<<mFileName<<"'"); 
    set (WRONG);
    return NULL;
  }  

  size_t size = check (file);

  if (!size)
  {
    D_Message ("FileResource: Wrong file format '"<<mFileName<<"'");
    set (WRONG);
    return NULL;
  }

  void* data  = alloc (size);

  if (!data)
    return NULL;

  if (!read (file,data,size))
  {
    set  (WRONG); 
    free (true);
    return NULL;
  }

  return data;
}

FileResource::FileResource  (const Str128& fname,const Str32& gname,Cache* cache)
             : Resource   (cache),
               mFileName  (fname)
{    
  if (&gname)
    mResGroup = gname;

  set (MOVE);
}

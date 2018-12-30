#include <pch.h>
#include <binfile\DiskPack.h>
#include <binfile\SysFile.h>

class  DiskSearch: public SearchFile
{
  public:
                DiskSearch (DiskPackFile*,const char*);
     virtual    ~DiskSearch   ();

     virtual    const char*    name  ();
     virtual    filesize_t     size  ();
     virtual    bool           valid () const;
     virtual    bool           next  ();
     virtual    fileattr_t     attr  ();
     virtual    filetime_t     time  (FileTimeType);

  private:
      WIN32_FIND_DATA   search;
      HANDLE            handle;  
};

class  SysBinFileX: public SysBinFile, public PackLump
{
  public:
                SysBinFileX (DiskPackFile* file,const char* name,int mode)
                  : PackLump (file,this),
                    SysBinFile (name,mode) 
                  {  }
};

BinFile*   DiskPackFile::open  (const char* name,int mode)
{
  return ext_new SysBinFileX (this,name,mode);
} 

SearchFile*     DiskPackFile::_OpenSearch (const char* tmpl)
{
  return ext_new DiskSearch (this,tmpl);
}

DiskPackFile::DiskPackFile  (Pool* pool,size_t size)
            : PackFile (pool,size)
{   }

DiskPackFile::~DiskPackFile ()
{   
}

DiskSearch::DiskSearch (DiskPackFile* pak,const char* tmpl)
               : SearchFile (pak)
{
  handle = FindFirstFile (tmpl,&search);
}

DiskSearch::~DiskSearch   ()
{
  if (handle != INVALID_HANDLE_VALUE)
    FindClose (handle); 
}

const char*    DiskSearch::name  ()
{
  return handle != INVALID_HANDLE_VALUE ? search.cFileName : NULL;
}

fileattr_t    DiskSearch::attr  ()
{
  if (handle != INVALID_HANDLE_VALUE)
  {
    uint sattr = search.dwFileAttributes,
         attr  = (((sattr & FILE_ATTRIBUTE_ARCHIVE) != 0) << 0) |
                 (((sattr & FILE_ATTRIBUTE_DIRECTORY) != 0) << 1) |
                 (((sattr & FILE_ATTRIBUTE_READONLY) != 0) << 2);

    return attr;
  }
  else return BF_ATTR_NORMAL;
}

filetime_t         DiskSearch::time  (FileTimeType type)
{
  if (handle != INVALID_HANDLE_VALUE)
  {
    switch (type)
    {
      case CREATE: return *(filetime_t*)&search.ftCreationTime;
      case WRITE:  return *(filetime_t*)&search.ftLastWriteTime;
      case ACCESS: 
      default:     return *(filetime_t*)&search.ftLastAccessTime;
    }
  }
  else return 0;
}

filesize_t         DiskSearch::size  ()
{
  return handle != INVALID_HANDLE_VALUE ? 
         (filesize_t(search.nFileSizeHigh)<<32) + search.nFileSizeLow :
         0;
}

bool           DiskSearch::valid () const
{
  return handle != INVALID_HANDLE_VALUE;
}

bool           DiskSearch::next  ()
{
  if (handle != INVALID_HANDLE_VALUE)
  {
    if (!FindNextFile (handle,&search))
    {
      FindClose (handle);
      handle = INVALID_HANDLE_VALUE;
      return false;
    }        
    
    return true;
  }  
  
  return false;
}

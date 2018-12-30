#include <pch.h>
#include <service\memory.h>
#include <binfile.h>
#include <string.h>
#include <pool.h>

#include <stdio.h>

#include "excpt.h"
#include "fgroup.h"

#include <binfile\sysfile.h>
#include <binfile\packstream.h>
#include <binfile\diskpack.h>
#include <binfile\zippack.h>

accessptr_t<FileService>    fileService;

const char*  FileService::_GroupSign = ":\\";

static char* fileSrvErr [] = {
  "No error",
  "File not found",
  "Group not found",
  "No enough memory",
  "Error register pak file"
};

class MainPackSearch: public SearchFile
{  
  public:
                MainPackSearch  (FileGroup* g,const char* tmpl)
                  : SearchFile (g), group (g), search (group->OpenSearch (tmpl))  {}
    virtual     ~MainPackSearch () { if (search) delete search; }

    virtual const char* name () { return search ? search->name () : NULL; }
    virtual filesize_t  size () { return search ? search->size () : 0; }    
    virtual fileattr_t  attr () { return search ? search->attr () : BF_ATTR_NORMAL; }    
    virtual filetime_t  time (FileTimeType t) { return search ? search->time (t) : 0; }
    virtual bool        next () { 
      if (search)
        if (!search->next ())
        {
          delete this;
          return false;
        }
        else return true;
      return false;
    }    
    virtual bool        valid () const { return search && search->valid (); }

  private:
        FileGroup*     group;
        SearchFile*    search;
};

////////////////////////implementation of class FileService////////////////////////

const char*  FileService::GetErrorMessage (int error) const
{
  return error > 0 && error < LAST_ERROR ? fileSrvErr [error] : Service::GetErrorMessage (error); 
}

uint    FileService::count      (bool rec) const
{
  GDict::iterator iter = mGroups.GetFirst ();
  int             cnt  = mGroups.count ();
  int             res  = 0;

  for (int i=0;i<cnt;i++,iter++)
  {
    res += iter->count (rec);
  }  

  return res;
}

void    FileService::dump       (ostream& os,bool rec,int left) const
{
  GDict::iterator iter = mGroups.GetFirst ();
  int             cnt  = mGroups.count ();

  dout<<tab (left)<<"Dump file system"<<endl;

  for (int i=0;i<cnt;i++,iter++)
    iter->dump (os,rec,left+8);

  dout<<tab (left)<<"End dump file system"<<endl;
}

bool        FileService::RegisterGroup   (const char* groupname,const char* name,BinFile* file,bool autodel)
{
  D_Try
  {
    CSEntry trans (mCS);

    FileGroup* group = NULL;

    if (groupname)      
    {
      if (!mGroups.search (groupname,group))
      {
        group = ext_new FileGroup (groupname,this); 
        if (!group)
        {
          SetError (NO_ENOUGH_MEMORY);
          return false;
        }  
      }
    }
    else group = mDefault;        

    return name ? group->Register (name,file,autodel) : true;
  }
  D_ExcRet ("FileService: Error at RegisterGroup ()",false);
}

void        FileService::UnregisterGroup (const char* groupname,const char* name)
{
  CSEntry trans (mCS);

  D_Try
  {
    if (!groupname)
      return ;

    FileGroup* group = NULL;

    if (!mGroups.search (groupname,group))
    {
      SetError (GROUP_NOT_FOUND);
      return ;
    }      

    if (name)  group->Unregister (name);
    else       delete group;
  }
  D_Exc ("FileService: Error at UnregisterGroup ()");
}

BinFile*    FileService::OpenFile  (const char* groupName,const char* fileName,int mode)
{
  CSEntry trans (mCS);

  D_Try        
  {
    char        name  [128]; name  [0] = 0;
    const char* fname = name;

    locase (fileName,name);

    fileName    = name;

    BinFile*   file  = NULL;
    FileGroup* group = GetGroup (fileName,groupName,&fileName); 

    if (!group)
    {
      file = ((PackFile*)mDiskPack)->open (fname,mode);

      if (!IsValid (file))
      {
        if (file) delete file;
        SetError (GROUP_NOT_FOUND);
        return NULL;
      }  
    }
    else 
      file = group->open (fileName,mode&BF_FILE_ATTR_MASK);

    if (!IsValid (file))
    {
      if (file) delete file;
      SetError (FILE_NOT_FOUND);
      return NULL;
    }

    if (mode & BF_OPEN_PACK_STREAM)
    {
      int level = ((mode >> BF_PACK_ATTR_SHIFT) & BF_PACK_ATTR_MASK) - 1;

      if (level < 0)    level = BF_PACK_BEST;
      else if (level > BF_PACK_BEST) level = BF_PACK_BEST;

      PackStream* pak = ext_new PackStream (file,mode,level,GetPool());

      if (!IsValid ((BinFile*)pak))
      {
        if (pak) delete pak;
      }
      else
      {
        file = pak;
        pak->SetAutodel (true);
      }
    }

    if (mode & BF_OPEN_CACHE)
    {
      CacheBinFile* sfile = ext_new CacheBinFile (file,GetPool());

      if (!IsValid ((BinFile*)sfile))
      {
        if (sfile) delete sfile;
      }  
      else 
      {
        file = sfile;
        sfile->SetAutodel (true);
      }  
    }

    return file;
  }
  D_ExcRet ("FileService: Error at OpenFile '"<<fileName<<"'",NULL);
}

void        FileService::CloseFile       (BinFile* file)
{
  CSEntry trans (mCS);

  D_Try
  {
    delete file;
  }
  D_Exc ("FileService: Error at closeFile () file: "<<file);
}

bool        FileService::AddFastPath (const char* fast_path,const char* full_path)
{
  CSEntry trans (mCS);

  D_Try
  {
    Str128 path, fpath;

    locase (full_path,fpath);
    locase (fast_path,path);    

    PackFile* pak = ext_new FastPathPackFile (this,fpath);

    if (!pak)
    {
      SetError (NO_ENOUGH_MEMORY);
      return false;
    }

    return RegisterGroup (path,path,pak,true);
  }
  D_ExcRet ("FileService: Error at AddFastPath",false);
}

void        FileService::DelFastPath      (const char* fast_path)
{
  CSEntry trans (mCS);

  D_Try
  {
    Str128 path;

    locase (fast_path,path);    

    UnregisterGroup (path);
  }
  D_Exc ("FileService: Error at DelFastPath");
} 

void    FileService::SetCurrentDir    (const char* cur_dir)
{
  CSEntry trans (mCS);

  D_Try
  {
    if (cur_dir)
    {
      Str128 path;

      locase (cur_dir,path);    

      mCurDir->SetDirName (path);
    }  
  }
  D_Exc ("FileService: Error at SetCurrentDir");  
}

PackFile* FileService::OpenZipFile      (const char* name,Pool* pool)
{
  return new (pool) ZipPackFile (name,pool);
}

bool        FileService::RegisterPackFile (const char* name,OpenPackFileFn  fn,const char* groupname)
{
  CSEntry trans (mCS);

  D_Try
  {
    PackFile* file = (*fn)(name,mFilePool);    
    
    if (!IsValid (file))
    {
      SetError (ERROR_PAK_REGISTER);
      return false;
    }  

    if (!RegisterGroup (groupname,name,file,true))
      return false;

    return true;
  }
  D_ExcRet ("FileService: Error at OpenPackFile",false);
}

bool     FileService::AddAlias  (const char* groupName,const char* name,BinFile* file)
{
  CSEntry trans (mCS);

  D_Try
  {
    FileGroup* group = GetGroup (name,groupName,&name,true);

    if (group)   return group->Register (name,file);
    else         return false;
  }
  D_ExcRet  ("FileService: Error at AddAlias",false);
}

void     FileService::DelAlias         (const char* groupName,const char* name)
{
  CSEntry trans (mCS);

  D_Try
  {
    FileGroup* group = GetGroup (name,groupName,&name);

    if (group)
    {
      if (!*name) UnregisterGroup (groupName);
      else        group->Unregister (name);
    }
    else
      SetError (GROUP_NOT_FOUND);
  }  
  D_Exc ("FileService: Error at DelAlias");
}

FileGroup*  FileService::GetGroup (const char* full_name,const char* group_name,const char** file_name,bool create)
{
  char  gname [128]; gname [0] = 0;
  FileGroup* group = NULL;

  if (group_name)
  {
    locase (group_name,gname);
    group = GetGroup (group_name = gname);
  }
  else
  {
    char* sign  = strstr (full_name,_GroupSign);

    if (sign && sign != full_name)
    {  
      int    len = sign-full_name;
      memcpy (gname,full_name,len); 
      gname [len] = 0;
       
      if (file_name) *file_name = sign + strlen (_GroupSign);

      group = GetGroup (group_name = gname); 
    }  
    else
      return mDefault;      
  }

  if (!group && create)  
  {
    RegisterGroup (gname);
    group = GetGroup (gname);
  }  

  return group;
}

FileGroup*  FileService::GetGroup   (const char* name) const  
{ 
  FileGroup* group = NULL;
  mGroups.search (name,group);
  return group;
}

SearchFile* FileService::OpenSearch (const char* fileName)
{
  CSEntry trans (mCS);

  D_Try
  {
    if (!fileName)
      return NULL;

    char*             sign   = strstr (fileName,_GroupSign);
    FileGroup*        group  = NULL;

    if (!sign || sign == fileName)  group = mDefault;
    else  
    {  
      char   dest  [MAX_PATH_LEN];
      int    len = sign-fileName;
      memcpy (dest,fileName,len); 
      dest [len] = 0;           
      group      = GetGroup (dest);
    }

    if (group && group->count ())
    {
      SearchFile* s = ext_new MainPackSearch (group,fileName);

      if (!s) 
      {
        SetError (NO_ENOUGH_MEMORY);
        return NULL;
      }  

      return s;
    }
    else  return NULL;
  }  
  D_ExcRet ("FileService: Error at FirstFile",NULL);
}

FileService::FileService     (Pool* pool,size_t size)
         : Service       (&fileService),
           PackFile    (pool,size),
           mGroups       (mFilePool),
           mDiskPack     (NULL),
           mDefault      (NULL),
           mCurDir       (NULL)
{
  CSEntry trans (mCS);  

  mDiskPack = ext_new DiskPackFile (pool);  

  if (!mDiskPack)
    SetError (NO_ENOUGH_MEMORY);

  mCurDir = ext_new CurrentDirPak (this);

  if (!IsValid (mCurDir))
  {
    SetError (NO_ENOUGH_MEMORY);
    D_Message ("FileService: Error at create current dir access");
    return;
  }

  mCurDir->SetDirName (".");

  RegisterGroup ("default","disks",mDiskPack,true);
  RegisterGroup ("default",".",mCurDir,true);

  mDefault = GetGroup ("default");

  mDefault->SetFirstPak (1);
}

FileService::~FileService ()
{    
  CSEntry trans (mCS);

  FileGroup*   groups [32];

  int         count = mGroups.count (); 

  while (count)
  {
    int         cnt  = count > 32 ? 32 : count;
    GDict::iter iter = mGroups;

    for (int i=0;i<cnt;groups [i++] = (FileGroup*)iter++);
    for (i=0;i<cnt;i++)
    {
//      try
//      {
        delete groups [i];
//      }
//      catch (Exception& exc)
//      {
//        dout<<exc<<endl;
//      }
//      catch (...)
//      {
  //      dout<<"Error"<<endl;
    //  }  
    }  

    count -= cnt;
  }

//  if (mCurDir)
//    delete mCurDir;

}

///////////////////////////////////////////////////////////////////////////////

FileService*     F_Load       (Pool* pool,size_t size)
{
  return new (pool) FileService (pool,size);
}

void             F_Done       (FileService* srv)
{
  if (srv) delete srv;
}


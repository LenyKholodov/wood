//////////////////////////implementation of class FileGroup/////////////////////

BOOL      FileGroup::Register   (const char* _GroupName,BinFile* file)
{
  D_Try
  {
    ulong   crc   = CRC32 (_GroupName,strlen (_GroupName));
    FRing*  ring  = NULL;

    if (mDict.search (crc,ring))
    {
      if (!file)
        return FALSE;
      else
      {
        if (!ring->insert (file))
          return FALSE;
      }  
    }
    else
    {
      ring = ext_new FRing (M_GetPool ());

      if (!ring)
      {
        D_Message ("FileGroup: No enough memory ");
        return FALSE;
      }   

      mDict.insert (crc,ring);

      if (file)
      {
        if (!ring->insert (file))
          return FALSE;
      }
    }

    return TRUE;
  }
  D_ExcRet ("FileGroup: Error at Register",FALSE);
}

void      FileGroup::Unregister (const char* _GroupName,BinFile* _File)
{
  D_Try
  {
    ulong   crc   = CRC32 (_GroupName,strlen (_GroupName));
    FRing*  ring  = NULL;

    if (mDict.search (crc,ring))
    {
      if (!_File)
      {  
        mDict.erase (crc);
        delete ring;
      }  
      else
        ring->erase (_File);
    }
  }
  D_Exc ("FileGroup: Error at Unregister");
}

FileGroup::FRing*    FileGroup::GetGroup   (const char* _GroupName)
{
  D_Try
  {
    FileGroup::FRing* res = NULL; 

    return mDict.search (_GroupName,res) ? res : NULL;
  }
  D_ExcRet ("FileGroup: Error at GetGroup",NULL);
}

FileGroup::FileGroup  (Pool* _Pool)
          : MemObject  (_Pool),
            mDict      (_Pool)
{   
  if (!IsValid (&mDict))
    D_Message ("FileGroup: Error at construct: No enough memory");
}

FileGroup::~FileGroup ()
{
  //close!!!
}

SEARCHFILE  FileService::FirstFile        (const char* fileName)
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
      SEARCHFILE s = ext_new fssearch_t (fileName,group);

      if (!s) 
      {
        SetError (NO_ENOUGH_MEMORY);
        return NULL;
      }  

      return NextFile (s); 
    }
    else  return NULL;
  }  
  D_ExcRet ("FileService: Error at FirstFile",NULL);
}

void        FileService::CloseSearch      (SEARCHFILE s)
{
  CSEntry trans (mCS);

  D_Try
  {    
    if (s) delete s;
  }
  D_Exc ("FileService: Error at closeSearch");
}



SEARCHFILE  FileService::NextFile         (SEARCHFILE s)
{
  CSEntry trans (mCS);

  D_Try
  {
    if (!s)
      return NULL;

    if (s) 
    {
      if (s->next ()) return s;
      else  
      {
        CloseSearch (s);
        return NULL;
      }
    }  
    else return NULL;
  }
  D_ExcRet ("FileService: Error at NextFile",NULL);
}

void FileGroup::NextFile (searchhandle_t* s) 
{
  if (s->search.param)
  {
    if (s->iter->file->IsPack ()) 
      ((pakBinFile*)s->iter->file)->close (s->search);
    s->search.param = 0;                  
  }

  s->self = false;
  s->count--;
  s->iter++;
}

const char* FileGroup::RetName (searchhandle_t* s,const char* res,searchfile_t& sf) 
{
  s->res  = mName;
  s->res += FileService::_GroupSign;
  s->res += res;
  sf.size = s->search.size;

  return s->res;                
}

const char* FileGroup::next (searchfile_t& s) 
{
  if (!s.param)
  {
    s.param = (DWORD)ext_new searchhandle_t (s.fname,this);
    if (!s.param)
      return NULL;
  }                

  searchhandle_t* search = (searchhandle_t*)s.param;

  if (search->search.param)
  {
    const char* res = ((pakBinFile*)search->iter->file)->next (search->search);
    if (res) return RetName (search,res,s);
    else     NextFile (search);
  }

  while (search->count > 0)
  {
    filedesc_t* desc = &search->iter.data ();

    if (!search->self)
    {
      search->self = true;

      if (searchfile_t::cmp (desc->name,search->tmpl))
        return RetName (search,desc->name,s);
    }  

    if (desc->file->IsPack ())
    {
      pakBinFile* pak = (pakBinFile*)desc->file;
      
      search->search.fname  = search->tmpl;

      const char* res = pak->next (search->search);

      if (res)
        return RetName (search,res,s);
    }

    NextFile (search);
  }

  close (s);

  return NULL;
}

void      FileGroup::close (searchfile_t& s) 
{
  if (s.param)
  {
    delete (searchhandle_t*)s.param;
    s.param = 0;
  }
}

//////////////////////////////////////////////////////////////////////////////////
///Реинтерпретация путей
//////////////////////////////////////////////////////////////////////////////////
class FastPathGroup: public pakBinFile
{
  public:
                FastPathGroup (const char* fast_path,FileService*);

   virtual    BinFile*     open       (const char* name,int mode);
   virtual    const char*  next       (searchfile_t& s);
   virtual    void         close      (searchfile_t& s);

   virtual    SearchFile*  OpenSearch (const char*);

  private:      
    FileService*        mService;
    Str128              mPath;
};

struct searchhandle_t
{
  public:
    searchfile_t    search;
    FDRing::diter   iter;
    int             count;
    bool            self;
    Str128          tmpl;
    Str128          res;

                    searchhandle_t  (const char*,FileGroup*);
                    ~searchhandle_t ();
};

/////////////////////searchhandle_t//////////////////////////////////////////////////////

searchhandle_t::searchhandle_t (const char* t,FileGroup* group)
               : tmpl (t),
                 iter (group->mFiles), 
                 count (group->mFiles.count ()),
                 self (false) 
{   }

searchhandle_t::~searchhandle_t   () 
{
  if (search.param)
  {
    if (iter->file->IsPack ())
      ((pakBinFile*)iter->file)->close (search);
    search.param = 0;
  }    
}

/////////////////////////FastPathGroup//////////////////////////////////////////////

BinFile*     FastPathGroup::open       (const char* name,int mode)
{
  Str256 fname  = mPath;
  fname        += name;

  return mService ? mService->OpenFile (fname,mode) : NULL;
}

const char*  FastPathGroup::next       (searchfile_t& s)
{
  if (!mService)
    return NULL;

  if (!s.param)
  {
    Str256 fname  = mPath;
    fname        += s.fname ;

    s.param = (DWORD)mService->FirstFile (fname);

    if (!s.param)
      return NULL;
  }

  s.param = (DWORD)mService->NextFile ((SEARCHFILE)s.param);

  if (!s.param)  return NULL;
  else                
  {
    s.size = ((SEARCHFILE)s.param)->size ();
    return ((SEARCHFILE)s.param)->name ();
  }
}

void         FastPathGroup::close      (searchfile_t& s)
{
  if (s.param && mService)
    mService->CloseSearch ((SEARCHFILE)s.param);
}

FastPathGroup::FastPathGroup (const char* fast_path,FileService* srv)
              : pakBinFile (),
                mPath (fast_path),
                mService (srv)
{     }


/*typedef struct  usersearch_t
{
  friend class FileService;
  public:
////////////////////////////////////////////////////////////////////////////////
///Текущие данные
////////////////////////////////////////////////////////////////////////////////
      virtual const char* name () const = 0;
      virtual size_t      size () const = 0;

///////////////////////////////////////////////////////////////////////////////
///Поиск - итерация
///////////////////////////////////////////////////////////////////////////////
      virtual bool        next () = 0;

  protected:
               usersearch_t () {}
       virtual ~usersearch_t () {}
}*SEARCHFILE;
*/

struct  searchfile_t
{
  Str128        fname;          //имя файла
  size_t        size;           //размер файла  
  DWORD         param;          //используется поисковиком

                searchfile_t (): param (0), size (0) {}
  static bool   cmp          (const char* tmpl,const char* fname);
};

        virtual const char*     next        (searchfile_t&) { return NULL; }        
        virtual void            close       (searchfile_t&) { }

        virtual const char*     next  (searchfile_t&);

  private:
                void            close (searchfile_t&);

        virtual const char*     next  (searchfile_t&);
        virtual void            close (searchfile_t&);


        virtual const char*     next  (searchfile_t&);
        virtual void            close (searchfile_t&);

struct dbfsearch_t
{
  WIN32_FIND_DATA   search;
  HANDLE            handle;  
};                     

const char*     diskBinFile::next  (searchfile_t& s)
{
  if (!s.param)
  {
    s.param = (DWORD)ext_new dbfsearch_t;

    if (!s.param)
      return NULL;

    mSearches.insert (&s);  

    dbfsearch_t* search = (dbfsearch_t*)s.param; 

    search->handle = FindFirstFile (s.fname,&search->search);

    if (search->handle == INVALID_HANDLE_VALUE)
    {
      close (s);
      return NULL;
    }

    s.size  = search->search.nFileSizeLow;

    return search->search.cFileName;
  }  
  else
  {
    dbfsearch_t* search = (dbfsearch_t*)s.param;               

    if (!FindNextFile  (search->handle,&search->search))
    {
      close (s);
      return NULL;
    }     

    s.size  = search->search.nFileSizeLow;

    return search->search.cFileName;
  }
}

void            diskBinFile::close (searchfile_t& s)
{
  if (s.param)
  {
    dbfsearch_t* search = (dbfsearch_t*)s.param;         
    if (search->handle)
      FindClose (search->handle);
    delete search;
    s.param = 0;
    mSearches.erase (&s);
  }
}


const char*     dirBinFile::next  (searchfile_t& s)
{
  return mPak ? mPak->next (s) : NULL;
}

void            dirBinFile::close (searchfile_t& s)
{
  if (mPak)
    mPak->close (s);
}

bool            zipBinFile::cmp   (const char* s1,const char* s2)
{
  int len  = strlen (s1),
      len1 = strlen (s2);
           
  if (len > len1) len = len1;

  for (int j=0;j<len;j++)
  {
    if (*s1 != '*')
    {      
      if (*s1 == *s2 || *s1 == '?') { s1++; s2++; }      
      else                          return false;
    }   
    else
    {
      if (s1 [1]) //далее ещё есть символ
      {
        if (s1 [1] == '*')
          return false; //error!

        s1++;
        s2--;

        do 
        {
          s2 = strchr (s2+1,*s1); 
        } while (s2 && !cmp (s1,s2)); 

        return s2 != NULL;
      }
      else return true;
    }  
  }

  if (*s1 || *s2)
    return false;

  return true;
}


const char*     zipBinFile::next  (searchfile_t& s)
{
  char    name [256];
  char*   fname = s.fname;

  if (s.param >= (int)mPackFile.gi.number_entry)
  {
    return NULL;
  }  

  ConvNames (s.fname,name);

  for (int i=s.param;i<(int)mPackFile.gi.number_entry;i++)
  {
    if (cmp (name,mPackFile.fi[i].name))
    {
      s.param = i + 1;
      s.size  = mPackFile.fi[i].size;
      return mPackFile.fi[i].name;
    }  
  }  

  return NULL;
}


void            zipBinFile::close (searchfile_t& s)
{
  s.param = 0;
}

bool searchfile_t::cmp (const char* s1,const char* s2)
{
  int len  = strlen (s1),
      len1 = strlen (s2);
           
  if (len > len1) len = len1;

  for (int j=0;j<len;j++)
  {
    if (*s1 != '*')
    {      
      if (*s1 == *s2 || *s1 == '?') { s1++; s2++; }      
      else                          return false;
    }   
    else
    {
      if (s1 [1]) //далее ещё есть символ
      {
        if (s1 [1] == '*')
          return false; //error!

        s1++;
        s2--;

        do 
        {
          s2 = strchr (s2+1,*s1); 
        } while (s2 && !cmp (s1,s2)); 

        return s2 != NULL;
      }
      else return true;
    }  
  }

  if (*s1 || *s2)
    return false;

  return true;
}

bool        DebugService::FindLine   
 ( const void* crash_addr,
   char*       file_name,
   char*       func_name,
   uint&       line,
   uint&       bytes,
   const char* mfile ) 
{

  ifstream  map (mfile);  

  ulong    addr  = (ulong)crash_addr;
  ulong    start = 0;

  if (!find)
    return false;

  find = false;

  map.getline (string,2048);

  while (map) //Определим дескриптор адреса по каталогу
  {

  }

  if (!find)
    return false;  

  dout<<"FIND: "<<info [sel].file<<endl;

  char lineInfo [1024] = "Line numbers for ";
  char fileName [1024];

  strcat (lineInfo,info [sel].file);
  strcpy (func_name,info [sel].func);
  strcpy (file_name,info [sel].file);

  find = false;

  char* str = NULL;

  while (map)
  {
    map.getline (string,2048);             

    str = strstr (string,lineInfo);

    if (str)
    {
      find = true;
      break;
    }
  }

  if (!find)
    return false;

  find = false;

  ulong foffs = addr - start - 0x1000;
  ulong min   = (ulong)-1;

  while (map)
  {
    char   textFile [1024];
    uint   ln = 0;

    str += strlen (lineInfo);

    ReadLine (str,fileName,'(',')');

    map.getline (string,2048); 
    map.getline (string,2048); 

    while (strlen (string) > 1)
    {
      str = string;

      for (int i=0;i<4;i++)
      {
        uint  l    = strtol (str,&str,10);
        ulong seg  = strtol (str,&str,16); str++;
        ulong offs = strtol (str,&str,16);              

        if (foffs < offs && offs < min)
        {
          line  = ln;
          bytes = offs-foffs;
          min   = offs;

          strcpy (file_name,fileName);
        }

        ln = l;

        if (!*str) 
          break;
      }             

      map.getline (string,2048); 
    }
  }

  if (min != (ulong)-1)
    return true;
}

  return false;
}

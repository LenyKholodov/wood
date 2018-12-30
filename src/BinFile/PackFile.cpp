#include <pch.h>
#include <binfile\PackFile.h>
#include <pool.h>

class    PackSearch: public SearchFile
{
  public:
        PackSearch (PackFile* pak,SearchFile* s):
          SearchFile (pak), search (s) { if (pak && s) pak->insert (s); }
        virtual ~PackSearch () {
          if (mPak && search) 
            mPak->erase (search);          
        }

        virtual bool valid () const { return search && search->valid (); }
        virtual bool next  ()       { return search && search->next (); }
        virtual const char* name () { return search ? search->name () : NULL; }
        virtual filesize_t size ()  { return search ? search->size () : 0;  }
        virtual fileattr_t attr ()  { return search ? search->attr () : BF_ATTR_NORMAL; }
        virtual filetime_t time (FileTimeType t) { return search ? search->time (t) : 0; }

                void       release () {
                  if (search) delete search;
                  search = NULL;
                }

  private:
        SearchFile*     search;
};

void     PackFile::_flush   ()
{
  FileRing::iterator iter  = mFiles;
  int                count = mFiles.count ();  

  for (int i=0;i<count;i++,iter++)
    iter->flush ();
}

SearchFile* PackFile::OpenSearch (const char* tmpl)
{
  return ext_new PackSearch (this,_OpenSearch (tmpl));
}

uint   PackFile::count (bool rec) const
{
  return _count ("",rec);
}

void   PackFile::dump  (ostream& os,bool rec,int left) const
{
  _dump ("",os,left,rec); 
}

void  PackFile::_dump (const char* dir,ostream& os,int left,bool recurse) const
{
  Str128 dname  = dir; 
  char   end    = *(dir + strlen (dir) - 1);

  if (*dir && end!= '/' && end != '\\')
    dname += '\\';

  SearchFile* s = ((PackFile*)this)->OpenSearch (dname+'*');  

  for (;s && s->valid ();s->next ())
  {
    os<<tab (left)<<dname<<s->name ()<<endl;

    if (recurse && (s->attr () & BF_ATTR_DIRECTORY))
    {
      int         len  = strlen (s->name ());
      const char* name = s->name () + len - 1;

      if (*name == '/' || *name == '\\')
      {
        name--;
        len--;
      }

      for (int i=0;len && *name == '.';len--,name--,i++);

      if (i && i<=2)
        continue;               

      _dump (dname+s->name (),os,left+4,recurse);
    }  
  }  

  delete s;  
}

uint  PackFile::_count (const char* dir,bool recurse) const
{
  Str128 dname  = dir; 
  uint   res    = 0;

  if (*dir)
    dname += '\\';

  SearchFile* s = ((PackFile*)this)->OpenSearch (dname+'*');  

  for (;s && s->valid ();s->next ())
  {
    res++;    

    if (recurse && (s->attr () & BF_ATTR_DIRECTORY))
    {
      int         len  = strlen (s->name ());
      const char* name = s->name () + len - 1;
      for (int i=0;len && *name != '\\' && *name == '.';len--,name--,i++);

      if (i && i<=2)
        continue;       

      res += _count (dname+s->name (),recurse);
    }  
  }  

  delete s;  

  return res;
}

PackFile::PackFile  (Pool* pool,size_t size)
           : mFilePool  (size ? new (pool) FloatPool (pool,size) : pool),
             mPoolOwner (size ? true : false),
             mSearches  (mFilePool),
             mFiles     (mFilePool)  
{     
  mMode |= can_pack|can_write; //can_write for flush
}

PackFile::PackFile  (PackFile* pak)
           : mFilePool  (pak->GetPool ()),
             mPoolOwner (false),
             mSearches  (mFilePool),
             mFiles     (mFilePool)  
{     
  mMode |= can_pack|can_write; //can_write for flush
}

PackFile::~PackFile ()
{
  FileRing::iterator iter  = mFiles;
  FileRing::iterator next  = iter + 1;
  int                count = mFiles.count ();

  for (int i=0;i<count;i++,iter=next,next++)
    delete iter; 

  SearchFileRing::iter siter = mSearches;
  count                      = mSearches.count ();

  for (i=0;i<count;i++,siter++)
    ((PackSearch*)siter.data ())->release ();

  if (!mPoolOwner)
    mFilePool = NULL;    
}

PackLump::PackLump (PackFile* pak,BinFile* file)
            : mPak (pak),
              mXFile (file)
{
  if (pak)
    pak->insert (file);
}                         

PackLump::~PackLump ()
{
  if (mPak)
    mPak->erase (mXFile);
}

bool SearchFile::cmp (const char* s1,const char* s2)
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
                         
SearchFile::SearchFile  (PackFile* pak)
           : mPak (pak)
{   }

SearchFile::~SearchFile ()
{   }

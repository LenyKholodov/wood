#include <pch.h>
#include <binfile\FastPathPack.h>
#include <pool.h>

class    FastPathSearch: public SearchFile
{
  public:       
                FastPathSearch (FastPathPackFile* pak,const char* tmpl);
    virtual     ~FastPathSearch () { if (mSearch) delete mSearch; }

        virtual bool        next  ()       { return mSearch ? mSearch->next () : false; }
        virtual const char* name  ();
        virtual filesize_t  size  () { return mSearch ? mSearch->size () : 0;     }
        virtual fileattr_t  attr  () { return mSearch ? mSearch->attr () : BF_ATTR_NORMAL; }
        virtual filetime_t  time  (FileTimeType t) { return mSearch ? mSearch->time (t) : 0; }
        virtual bool        valid () const { return mSearch ? mSearch->valid () : false;} 

  private:
        SearchFile*  mSearch;
        Str256       mName;
};

BinFile*        FastPathPackFile::open  (const char* name,int mode)
{
  Str256 fname = mDirName;

  return mPak ? mPak->open (fname+'\\'+name,mode) : NULL;
}

SearchFile*     FastPathPackFile::_OpenSearch (const char* tmpl)
{
  return ext_new FastPathSearch (this,tmpl);
}

void            FastPathPackFile::close (BinFile* file)
{
  if (mPak)
    mPak->close (file);
}

void            FastPathPackFile::dump  (ostream& os,bool rec,int left)   const
{
  for (int i=0;i<left;i++) os<<' ';
  os<<"Dump fastPath: '"<<mDirName<<"'"<<endl;

  PackFile::dump (os,rec,left+4);

  for (i=0;i<left;i++) os<<' ';
  os<<"End dump: '"<<mDirName<<"'"<<endl;
}

FastPathPackFile::FastPathPackFile  (PackFile* pak,const char* dname)
           : PackFile  (pak),
             mPak        (pak),
             mDirName    (dname)
{  }

FastPathPackFile::~FastPathPackFile ()
{  }

const char* FastPathSearch::name  () 
{
  if (mSearch && mSearch->valid ())
  {
    mName  = ((FastPathPackFile*)mPak)->mDirName;
    mName += mSearch->name ();

    return mName;
  }  

  return NULL;
}

FastPathSearch::FastPathSearch (FastPathPackFile* pak,const char* t)
           : SearchFile (pak),
             mSearch    (NULL)
{
  Str256 tmp  = pak->mDirName;
  tmp        += t;

  mSearch = pak->mPak->OpenSearch (tmp);
}

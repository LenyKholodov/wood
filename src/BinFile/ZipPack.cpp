#include <pch.h>

#include <binfile\ZipPack.h>
#include <binfile\MemFile.h>
#include <binfile\FrameFile.h>
#include <binfile\SysFile.h>

#include <zip\unzip.h>
#include <zip\zlib.h>
#include <string.h>

#pragma comment (lib,"zip.lib")

//////////////////////unzip structs/////////////////////////////////////////////

/* unz_file_info_interntal contain internal info about a file in zipfile*/
typedef struct unz_file_info_internal_s {
    uLong offset_curfile;/* relative offset of local header 4 bytes */
} unz_file_info_internal;


/* file_in_zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
typedef struct {
        char  *read_buffer;         /* internal buffer for compressed data */
        z_stream stream;            /* zLib stream structure for inflate */

        uLong pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
        uLong stream_initialised;   /* flag set if stream structure is initialised*/

        uLong offset_local_extrafield;/* offset of the local extra field */
        uInt  size_local_extrafield;/* size of the local extra field */
        uLong pos_local_extrafield;   /* position in the local extra field in read*/

        uLong crc32;                /* crc32 of all data uncompressed */
        uLong crc32_wait;           /* crc32 we must obtain after decompress all */
        uLong rest_read_compressed; /* number of byte to be decompressed */
        uLong rest_read_uncompressed;/*number of byte to be obtained after decomp*/
        FILE* file;                 /* io structore of the zipfile */
        uLong compression_method;   /* compression method (0==store) */
        uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
} file_in_zip_read_info_s;


/* unz_s contain internal information about the zipfile
*/
typedef struct {
        FILE* file;                 /* io structore of the zipfile */
        unz_global_info gi;       /* public global information */
        uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
        uLong num_file;             /* number of the current file in the zipfile*/
        uLong pos_in_central_dir;   /* pos of the current file in the central dir*/
        uLong current_file_ok;      /* flag about the usability of the current file*/
        uLong central_pos;          /* position of the beginning of the central dir*/

        uLong size_central_dir;     /* size of the central directory  */
        uLong offset_central_dir;   /* offset of start of central directory with
                                                                   respect to the starting disk number */

        unz_file_info cur_file_info; /* public info about the current file in zip*/
        unz_file_info_internal cur_file_info_internal; /* private info about it*/
    file_in_zip_read_info_s* pfile_in_zip_read; /* structure about the current
                                            file if we are decompressing it */
} unz_s;

//////////////////////ZipSearch/////////////////////////////////////////////

class   ZipSearch: public SearchFile
{
  public:
                ZipSearch (ZipPackFile* file,const char* tmpl);

    virtual     bool           next ();
    virtual     const char*    name ();
    virtual     filesize_t     size ();
    virtual     fileattr_t     attr ();
    virtual     filetime_t     time (FileTimeType);
    virtual     bool           valid () const { return pos < num; }

  private:
    int         num, pos;
    fileinfo_t* info;
    Str256      tmpl;
};

/////////////////////UnpackNoCompression////////////////////////////////////////

class   UnpackNoCompression: public FrameBinFile, public PackLump
{
  public:
                UnpackNoCompression (ZipPackFile* file,filepos_t offs,size_t size)
                : PackLump     (file,this),
                  FrameBinFile (file->mStdFile,offs,size) 
                  {  }
};

////////////////////UnpackCompression///////////////////////////////////////////

class   UnpackCompression: public MemBinFile, public PackLump
{
  public:
                UnpackCompression (ZipPackFile* file,void* buf,size_t size)
                : MemBinFile (buf,size), 
                  PackLump   (file,this)
                {  }
    virtual     ~UnpackCompression () { if (GetBuffer ())   delete [] GetBuffer (); }
};

////////////////////////ZipPackFile//////////////////////////////////////////////

void            ZipPackFile::dump  (ostream& os,bool rec,int left) const
{
  D_Try
  {
    os<<tab (left)<<"Dump zip pack: '"<<mPackFile.pak_name
      <<"' find: "<<mPackFile.gi.number_entry<<" files "<<endl;

    PackFile::dump (os,rec,left+4);

    os<<tab(left)<<"End dump zip pack: '"<<mPackFile.pak_name<<"'"<<endl;
/*    for (int i=0;i<left;i++) os<<' ';
    os<<"Dump zip pack: '"<<mPackFile.pak_name
      <<"' find: "<<mPackFile.gi.number_entry<<" files "<<endl;

    if (mPackFile.gi.number_entry)
    {
      for (int num = 0;num < (int)mPackFile.gi.number_entry;num++)
      {
        for (int i=0;i<left+8;i++) os<<' ';
        os<<"("<<num<<") '"<<mPackFile.fi[num].name<<"'"<<endl;
      }  
    }
    else
      os<<"...Empty dir"<<endl;      
    for (i=0;i<left;i++) os<<' ';
    os<<"End dump zip pack: '"<<mPackFile.pak_name<<"'"<<endl;*/
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
  }
}

void            ZipPackFile::ConvNames (const char* s1,char* s2)
{
  for (;*s1;s1++,s2++)  
    if (*s1 == '\\') *s2 = '/';
    else             *s2 = ToUpperEx(*s1);

  *s2 = NULL;
}

BinFile*        ZipPackFile::open  (const char* fname,int)
{
  char  name [256];
  char* buf = NULL;  

  DWORD   err;
  long    crc, num;
  BOOL    find = FALSE;

  ConvNames (fname,name);

  crc       = CRC32 (name,strlen (name));   

  if (mIndex.search (crc,num))
    if (!strcmp (name,mPackFile.fi[num].name))
      find = TRUE;

  if (!find)
  {
    for (num = 0;num < (int)mPackFile.gi.number_entry;num++)
      if (!strcmp (name,mPackFile.fi[num].name)) 
        break;

    if (num >= (int)mPackFile.gi.number_entry)
      return NULL;
  }

  BinFile* file = NULL;
  unz_s*   s    = (unz_s*)mPackFile.uf;

  unzLocateFileMy    (s,num, mPackFile.fi[num].c_offset);
  unzOpenCurrentFile (s);

  if (s->pfile_in_zip_read->compression_method == 0)
  {
    dout<<"ko-ko!-unpack"<<endl;
    file = ext_new UnpackNoCompression (this,s->pfile_in_zip_read->pos_in_zipfile,mPackFile.fi[num].size);
  }  
  else
  {
/*    dout<<"ko-ko!"<<endl;
    return ext_new gzBinFile (ext_new UnpackNoCompression (this,
                              s->pfile_in_zip_read->pos_in_zipfile + 
                              s->pfile_in_zip_read->byte_before_the_zipfile,
                              mPackFile.fi[num].size),
                              BF_OPEN_RO,
                              GetPool ()
                             );*/
    buf = ext_new char [mPackFile.fi[num].size];

    if (!buf)
      return NULL;

    err  = unzReadCurrentFile (s,buf,mPackFile.fi[num].size);     

    if (err != mPackFile.fi[num].size)
    {
      delete [] buf; 
      buf = NULL;
      return NULL;
    }

    file = ext_new UnpackCompression (this,buf,mPackFile.fi[num].size);
  }

  unzCloseCurrentFile (s);

  if (!file)
  {
    if (buf) delete [] buf; 
    buf = NULL;
    return NULL;
  }

  return file;
}

SearchFile*     ZipPackFile::_OpenSearch  (const char* tmpl)
{
  return ext_new ZipSearch (this,tmpl);
}

uint            ZipPackFile::count (bool) const
{
  return mPackFile.gi.number_entry;
}

bool            ZipPackFile::OpenPack    (const char* zname)
{
  strcpy (mPackFile.pak_name,zname);

  mPackFile.uf = unzOpen (mPackFile.pak_name);

  if (!mPackFile.uf)
    return false;

  unzGetGlobalInfo (mPackFile.uf,&mPackFile.gi);

  mPackFile.fi = ext_new fileinfo_t [mPackFile.gi.number_entry];

  if (!mPackFile.fi)
  {
    unzClose (mPackFile.uf);
    return false;
  }

  int             max = mPackFile.gi.number_entry;
  unz_file_info   file_info;

  for (int x=0;x<max;x++)
  {
    unzGetCurrentFileInfo (mPackFile.uf,&file_info,mPackFile.fi[x].name,sizeof(mPackFile.fi[x].name),NULL,0,NULL,0);

    mPackFile.fi[x].attr     = file_info.external_fa;
    mPackFile.fi[x].offset   = file_info.offset;
    mPackFile.fi[x].size     = file_info.uncompressed_size;
    mPackFile.fi[x].c_offset = file_info.c_offset;
    mPackFile.fi[x].time     = file_info.dosDate;

    ConvNames (mPackFile.fi[x].name,mPackFile.fi[x].name);

    unzGoToNextFile (mPackFile.uf);
  }

  mStdFile = ext_new SysBinFile (zname,BF_OPEN_RO);

  for (int num = 0;num < (int)mPackFile.gi.number_entry;num++)
    mIndex.insert (CRC32 (mPackFile.fi[num].name,strlen (mPackFile.fi[num].name)),num);

  return true;
}

ZipPackFile::ZipPackFile  (const char* zname,Pool* pool,size_t pool_size)
           : PackFile   (pool,pool_size),
             mIndex     (mFilePool),
             mStdFile   (NULL), mCacheFile  (NULL)             
{
  OpenPack (zname);
}

ZipPackFile::~ZipPackFile ()
{
  if (mCacheFile)    delete mCacheFile;
  if (mStdFile)      delete mStdFile;
  if (mPackFile.fi)  delete [] mPackFile.fi;

  if (mPackFile.uf)  unzClose (mPackFile.uf);
}


///////////////////ZipSearch/////////////////////////////////////////////////

bool           ZipSearch::next ()
{
  if (pos < num)
    for (pos++;pos<num;pos++)
      if (cmp (tmpl,info [pos].name))
        return true;

  return false;
}

const char*    ZipSearch::name () 
{
  return pos < num ? info [pos].name : NULL;
}

filesize_t         ZipSearch::size () 
{
  return pos < num ? info [pos].size : 0;
}

fileattr_t         ZipSearch::attr () 
{
  if (pos < num)
  {
    uint sattr = info [pos].attr,
         attr  = (((sattr & FILE_ATTRIBUTE_ARCHIVE) != 0) << 0) |
                 (((sattr & FILE_ATTRIBUTE_DIRECTORY) != 0) << 1) |
                 (((sattr & FILE_ATTRIBUTE_READONLY) != 0) << 2);
    return attr;                 
  }
  else return BF_ATTR_NORMAL;
}

filetime_t         ZipSearch::time (FileTimeType t)
{
  if (pos < num)  return info [pos].time; 
  else            return 0;
}

ZipSearch::ZipSearch (ZipPackFile* file,const char* t)
              : SearchFile (file),
                tmpl       (t),
                pos        (-1),
                num        (file->mPackFile.gi.number_entry),
                info       (file->mPackFile.fi)
{
  ZipPackFile::ConvNames (tmpl,tmpl);    
  next ();
}


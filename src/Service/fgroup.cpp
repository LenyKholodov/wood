#include <pch.h>
#include <service\file.h>

#include "fgroup.h"

class   GroupSearch: public SearchFile
{
  public:
                GroupSearch  (FileGroup*,const char*);
    virtual     ~GroupSearch ();

    virtual     bool             next   ();
    virtual     bool             valid  () const;
    virtual     const char*      name   ()         { return res;   }
    virtual     filesize_t       size   ()         { return fSize; }
    virtual     fileattr_t       attr   ()         { return fAttr; }
    virtual     filetime_t       time   (FileTimeType t)  { return fTime [t]; }

  private:
                void             NextPak  (); //переход к слудующему паку
                void             Change   (const char*,filesize_t);
                void             Change   (SearchFile&);

  private:    
    SearchFile*     search;   //поиск в текущем паке
    FDRing::diter   iter;     //указатель на текущиё пак
    int             count;    //кол-во паков
    bool            self;     //если найден пак
    filesize_t      fSize;    //размер найд. файла
    fileattr_t      fAttr;    //атрибуты найденного файла
    filetime_t      fTime [3]; //время
    Str256          tmpl;     //шаблон поиска
    Str256          res;      //результат
};


//////////////////////////FileGroup/////////////////////////////////////////////////////

bool      FileGroup::Register   (const char* name,BinFile* file,bool autodel) 
{ 
  Unregister    (name); 
  return mFileDesc.insert (filedesc_t (name,file,autodel));
}

bool      FileGroup::Register   (const char* name,PackFile* file,bool autodel) 
{ 
  Unregister    (name); 
  return mFileDesc.insert (filedesc_t (name,file,autodel));
}

void      FileGroup::Unregister (const char* name)    
{
  FDRing::diter  iter  = mFileDesc;
  int            count = mFileDesc.count ();
  ulong          crc   = CRC32 (name,strlen (name));

  for (int i=0;i<count;i++,iter++)
    if (iter->crc == crc && iter->name == name)
    {
      if (iter->autodel)
        delete iter->file;        

      mFileDesc.erase (iter);

      return;
    }  
}              

BinFile*  FileGroup::open       (const char* name,int mode) 
{
  uint           len   = strlen (name);
  uint           crc   = CRC32 (name,len);              
  FDRing::diter  iter  = mFileDesc; iter+=mFirst;
  int            count = mFileDesc.count ();

  for (int i=0;i<count;i++,iter--)
  {
    if (len == iter->len && crc == iter->crc && iter->name == name) 
      return iter->file;

    if (iter->file->canpack ())
    {
      PackFile* pak  = (PackFile*)iter->file;
      BinFile*    file = pak->open (name,mode);

      if (IsValid (file))
        return file;

      if (file) delete file;
    }  
  }    

  return NULL;
}

SearchFile*  FileGroup::_OpenSearch (const char* tmpl)
{
  return ext_new GroupSearch (this,tmpl);
}

uint FileGroup::count (bool rec) const 
{ 
  return mFileDesc.count (); //  
//  return pakBionFile::count (rec);
}

void FileGroup::dump (ostream& os,bool rec,int left) const 
{
  FDRing::diter     iter  = mFileDesc;iter+=mFirst;
  int               count = mFileDesc.count ();

  os<<tab (left)<<"Dump file group: '"<<mName<<"'"<<endl;

  for (int i=0;i<count;i++,iter--)
  {
    os<<tab (left+8);
    if (iter->file->canpack ())
    {
      os<<"Dump pak file: '"<<iter->name<<"'"<<endl;
      ((PackFile*)iter->file)->dump (os,rec,left+16);
      os<<tab (left+8)<<"End dump pak file: '"<<iter->name<<"'"<<endl;
    }  
    else
      os<<iter->name<<endl;
  }

  os<<tab (left)<<"End dump file group: '"<<mName<<"'"<<endl;
}

FileGroup::FileGroup  (const char* group_name,Pool* pool,size_t size)
          : PackFile (pool,size),
            mName      (group_name), 
            mFileDesc  (mFilePool),
            mOwner     (NULL),
            mFirst     (-1)
{  }

FileGroup::FileGroup  (const char* group_name,FileService* owner)
          : PackFile (owner->GetPool()),
            mName      (group_name), 
            mFileDesc  (mFilePool),
            mOwner     (owner),
            mFirst     (-1)
{
  if (owner)
    owner->mGroups.insert (mName,this);
}

FileGroup::~FileGroup () 
{ 
  FDRing::diter  iter  = mFileDesc;
  int            count = mFileDesc.count ();

  for (int i=0;i<count;i++,iter++) 
    if (iter->autodel && iter->file)
      delete iter->file;

  if (mOwner)
    mOwner->mGroups.erase (mName);
}

bool  GroupSearch::next   ()
{
  if (search)
  {
    if (search->next ())  
    {
      Change (*search);      
      return true;
    }  
    else  NextPak ();
  }  
  else if (count <= 0)
  {    
    res [0] = 0;
    return false;
  }  

  while (count > 0)
  {
    if (!self) //пытаемся подставит пак
    {
      self = true;

      if (cmp (tmpl,iter->name)) 
      {
        Change (iter->name,iter->file->size ());
        return true; 
      }  
    }  

    if (iter->file->canpack ()) //открываем поиск в паке
    {
      PackFile* pak = (PackFile*)iter->file;
      search          = pak->OpenSearch (tmpl);

      if (search && search->valid ())
      {
        Change (*search);
        return true;
      }  
    }

    NextPak ();
  }

  res [0] = 0;

  return false;
}

void  GroupSearch::NextPak ()
{
  if (search)
    delete search;

  search = NULL;
  self   = false;

  count--;
  iter--;  

  res [0] = 0;
}

void  GroupSearch::Change (const char* name,filesize_t s)
{
  res  = ((FileGroup*)mPak)->mName;
  res += FileService::_GroupSign;
  res += name;
  
  fSize = s;
  fAttr = BF_ATTR_ARCHIVE;
  
  for (int i=0;i<3;fTime [i++] = 0);
}

void  GroupSearch::Change (SearchFile& s)
{
  res  = ((FileGroup*)mPak)->mName;
  res += FileService::_GroupSign;
  res += s.name ();
  
  fSize = s.size ();
  fAttr = s.attr ();

  for (int i=0;i<3;i++)
    fTime [i] = s.time ((FileTimeType)i);
}

bool  GroupSearch::valid  () const
{
  return res [0] != 0;
}

GroupSearch::GroupSearch  (FileGroup* pak,const char* t)
                : SearchFile (pak),
                  tmpl       (t),
                  count      (((FileGroup*)pak)->mFileDesc.count ()),
                  iter       (((FileGroup*)pak)->mFileDesc),
                  self       (false),
                  fSize      (0),
                  search     (NULL)
{
  iter+=pak->mFirst;
  next ();
}

GroupSearch::~GroupSearch ()
{
  if (search)
    delete search;
}

CurrentDirPak::CurrentDirPak  (PackFile* file)
              : FastPathPackFile (file,NULL), mOpen (false), mOpenSearch (false)
{  }

BinFile*  CurrentDirPak::open  (const char* name,int mode)
{
  if (!mOpen)
  {
    mOpen = true;
    
    BinFile* file = FastPathPackFile::open (name,mode);

    mOpen = false;

    return file;
  }
  else return NULL;  
}

SearchFile*  CurrentDirPak:: _OpenSearch (const char* tmpl)
{
  if (!mOpenSearch)
  {
    mOpenSearch = true;

    SearchFile* search = FastPathPackFile::_OpenSearch (tmpl);

    mOpenSearch = false; 

    return search;
  }
  else return NULL;  
}

#include <pch.h>
#include <scripts\framerdr.h>
#include <binfile.h>

#include "reader.h"

bool FrameReader::ReadStream (Reader& rdr)
{    
  return mRoot ? mRoot->process (&rdr) : false;
}

bool  FrameReader::bind  (const char* name,void* fn,void* self,size_t size)
{
  return mCurFrame->AddTag (name,(ReadValueFn)fn,self,size);
}

bool  FrameReader::bindtag (const char* name,void** ptr,void* fn,void* self,size_t size)
{
  return mCurFrame->AddTag (name,ptr,(CreateTagFn)fn,self,size);
}

bool  FrameReader::BindReader  (const char* tag,void* fn,void* self)
{
  return bind (tag,fn,self);
}

bool  FrameReader::BindString   (const char* tag,char* string)
{
  return bind (tag,(ReadValueFn)ReadString,string);
}

bool  FrameReader::DeclareFrame (const char* tag)
{
  return bindtag (tag,NULL,NULL,NULL);
}

bool  FrameReader::BindCreater  (const char* tag,void* createrFn,void* self)
{
  return bindtag (tag,NULL,createrFn,self);
}

void* FrameReader::GetFrameObj () const
{
  return mCurFrame ? mCurFrame->GetObj () : NULL;
}

void* FrameReader::GetParentFrameObj () const
{
  return mCurFrame ? mCurFrame->GetParent () ? mCurFrame->GetParent ()->GetObj () : NULL : NULL;
}

extern bool ReadString (istream& is,char* str)
{
  is>>*str;

  int  end = ' ';

  if      (*str == '\'') end = '\'';
  else if (*str == '"')  end = '"';
  else str++;

  is.getline (str,FrameReader::max_string_len,end);  

  str [strlen (str)] = 0;

  return is.good ();
}

bool FrameReader::ReadString   (istream& is,char* str)
{
  return ::ReadString (is,str);
}

bool  FrameReader::ReadStream  (const char* in_file,ostream& err)
{
  BinFile* file = F_OpenFile (in_file,BF_OPEN_RO);

  if (!IsValid (file))
  {
    err<<"File not found '"<<in_file<<"'"<<endl;
    return false;  
  }

  const char* s = in_file + strlen (in_file) - 1;

  for (;*s != '/' && *s != '\\' && s != in_file;s--);

  Reader rdr (file,NULL,err,GetPool ());

  if (s != in_file) 
  {
    Str128 dir;
    const char*  src = in_file;

    for (char *dest=dir;src!=s;*dest++=*src++);

    *dest = 0;

    rdr.ChangeAssignDir (dir);
  }

  return ReadStream (rdr);
}

FrameReader::FrameReader  (Pool* pool)
                 : MemObject (pool),
                   mRoot     (NULL), mCurFrame (NULL)
{   
  mCurFrame = mRoot = ext_new Frame (this);
}

FrameReader::~FrameReader ()
{
  if (mRoot) delete mRoot;  
}

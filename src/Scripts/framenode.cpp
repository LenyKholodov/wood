#include <pch.h>
#include <scripts\framerdr.h>

#include "reader.h"

bool  FrameReader::Frame::process (FrameReader::Reader* rdr)
{
  Frame* old = *mCurFrame;

  *mCurFrame = this;

  if (!open (rdr))
  {
    *mCurFrame = old;
    return false;
  }  

  while (mOpen) next ();  

  close ();

  *mCurFrame = old;

  return true; 
}

bool  FrameReader::Frame::open (FrameReader::Reader* rdr)
{
  if (!mOpen)
  {
    mReader    = rdr;

    mReader->OpenTag (mState);  //запоминаем состояние читателя

    if (mHandler)
    {
      if (!(mObj=mHandler (mObj=NULL,*mOwner,mSelf)))
      {
        mReader->CloseTag (mState);
        return false;
      }    
    }  

    mOpen = true;

    return true;
  }
  else return false;
}

bool  FrameReader::Frame::close ()
{
  if (mOpen)
  {
    bool res = true;

    if (mHandler)
      res = mHandler (mObj,*mOwner,mSelf);      

    if (mPtr)
      *mPtr = mObj;

    mReader->CloseTag (mState);

    mOpen      = false;

    return res;
  }
  else return false;
}

void  FrameReader::Frame::next ()
{
  if (!mOpen)
    return;

  ulong       crc  = 0xFFFFFFFF;
  const char* name = mReader->next (crc);

  if (!name) //конец фрейма
  {
    close ();
    return;
  }

  if (name [0] == '.')  //находим тэг
  {
    if (!ReadDirective (++name))
    {
      mReader->TraceTag ();
      return;
    }
  }  
  else  
  {
    tag_t* tag = NULL;

    if (crc == mLast.crc) //быстрый поиск в однородных тэгах
      tag = mLast.tag;
    else  
    {
      TagDict::diter iter = mTags.find (name);

      if (!iter.valid ())
      {
        mReader->TraceTag ();
        return;
      }
      else tag = iter;
    }

    mLast.crc = crc;
    mLast.tag = tag;

    if (!ReadFrame (tag))
    {
      mReader->error    ("Error at read tag '%s'",name);
      mReader->TraceTag ();
      return;
    }
  }  
}

bool    FrameReader::Frame::ReadFrame (tag_t* tag)
{
  if (tag->fn)
  {    
    D_Try
    {
      if (!tag->fn (mReader->stream (),tag->data))
        return false;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      return false;
    }
  } 

  if (tag->frame) 
    tag->frame->process (mReader);

  tag->data = (char*)tag->data + tag->size;  //Для массивов 

  return true;
}

bool    FrameReader::Frame::ReadDirective (const char* name)
{
  if (!strcmp (name,"assign")) //присоединение файла поддержки
  {
    ReadDirAssign ();
  }
  else if (!strcmp (name,"chunk")) //указание чтения из файла поддержки
  { 
    if (!ReadDirChunk ())
      return false;
  }   
  else return false;

  return true;
}

void  FrameReader::Frame::ReadDirAssign ()
{
  Str128   fname, compress;
  istream& is = mReader->stream ();

  ReadString (is,fname);
  is>>compress;

  if (fname [0])
    mReader->ChangeAssignFile (fname,compress == "compress");
}

bool  FrameReader::Frame::ReadDirChunk  ()
{
  istream& is        = mReader->stream ();
  tag_t*   tag       = NULL;
  long     chunkOffs = -1; //В случае если идёт чтение куска
  size_t   chunkSize = 0;
  Str32    temp;
  String<64> name;

  is>>name;

  for (int i=0;i<2;i++)
  {
    is>>temp;

    if (temp == "offs")       is>>chunkOffs;
    else if (temp == "size")  is>>chunkSize;
  }   

  TagDict::diter iter = mTags.find (name);

  if (!iter.valid ())  return false;
  else                 tag = iter;

  mReader->read (tag->data,chunkSize,chunkOffs);

  if (tag->size)
    tag->data = (char*)tag->data + chunkSize;

  return true; 
}

FrameReader::Frame*  FrameReader::Frame::find (const char* src,const char*& dest) const
{
  for (char* s=(char*)src;*s && *s != '\\' && *s != '/' && *s != '.';s++);
 
  if (*s == '.' || *s == '\\' || *s == '/') //рекурсивный спуск для вложенных тэгов
  {
    char  old    = *s;    
    *s           = 0;
    char* first  = ++s;
    
    for (;*s != '.' && *s && *s != '\\' && *s != '/';s++);

    Frame* frame = NULL;
    bool  res    = mChildren.find (src,frame);
    first [-1]   = old;

    return res ? frame->find (first,dest) : NULL;
  }    
  else 
  {
    Frame* frame = NULL;

    if (mChildren.find (src,frame))
    {
      dest = "";
      return frame;
    }  
    else
    {
      dest = src;
      return (Frame*)this;
    }
  }  
}

bool    FrameReader::Frame::AddTag  (const char* name,ReadValueFn fn,void* self,size_t size)
{
  Frame* frame = find (name,name);

  if (!frame || !name [0])
    return false;  

  TagDict::diter ptr = frame->mTags.find (name);

  if (ptr.valid ())
  {  
    ptr->fn   = fn;
    ptr->data = self;
    ptr->size = size;

    return true;
  }
  else return frame->mTags.insert (name,tag_t (fn,self,size));
}

bool    FrameReader::Frame::AddTag  (const char* name,void** ptr,CreateTagFn fn,void* self,size_t size)
{
  Frame* frame = find (name,name);

  if (!frame)
    return false;  

  if (name [0])
  {
    Frame* child = ext_new Frame (this);
    tag_t  tag (child,NULL,size); 

    if (!child)
      return false;

    if (!frame->mTags.insert (name,tag))
    {
      delete child;
      return false;
    }

    if (!frame->mChildren.insert (name,child))
    {
      frame->mTags.erase (name);
      delete frame;
      return false;
    }

    frame = child;
  }

  frame->SetParams (ptr,fn,self);

  return true;
}

void    FrameReader::Frame::DelTag     (const char* name)
{
  Frame* frame = find (name,name);

  if (frame)
  {
    frame->mTags.erase     (name);
    frame->mChildren.erase (name);
  }
}

void FrameReader::Frame::SetParams (void** ptr,CreateTagFn fn,void* self)
{
  mHandler = fn;
  mSelf    = self;
  mPtr     = ptr;
}

FrameReader::Frame::Frame (FrameReader* own)
            : MemObject  (own->GetPool ()),
              mTags      (own->GetPool ()), mChildren (own->GetPool ()),
              mOwner     (own), mParent (NULL),
              mReader    (NULL),
              mOpen      (false), mCurFrame (&own->mCurFrame),
              mHandler   (NULL), mSelf (NULL), mPtr (NULL)
{    }

FrameReader::Frame::Frame (Frame* frame)
            : MemObject  (frame->GetPool ()),
              mTags      (frame->GetPool ()), mChildren (frame->GetPool ()),
              mOwner     (frame->mOwner), mParent (frame),
              mReader    (NULL),
              mOpen      (false), mCurFrame (frame->mCurFrame),
              mHandler   (NULL), mSelf (NULL), mPtr (NULL)
{    }

void    FrameReader::Frame::clear (ChildDict::iter i)
{
  if (i.valid () && i.data ())
  {
    clear (i.right ());
    clear (i.left  ());

    delete i;
  }
}

FrameReader::Frame::~Frame ()
{
  clear (mChildren.GetRoot ());
}

FrameReader::Frame::tag_t::tag_t (FrameReader::ReadValueFn _fn,void* self,size_t s)
{
  data  = self;
  fn    = _fn;
  frame = NULL;
  size  = s;
}

FrameReader::Frame::tag_t::tag_t (FrameReader::Frame* _frame,void* self,size_t s)
{
  data  = self;
  frame = _frame;  
  fn    = NULL;
  size  = s;
}
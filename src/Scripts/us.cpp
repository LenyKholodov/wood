#include <scripts\us.h>
#include <string.h>
#include <fstream.h>
#include <misc.h>

void      USStreamReader::BindCallFn       (const char* cmd,USCallFn fn)
{
  ulong     crc = CRC32 (cmd,strlen (cmd));
  USCallFn  old = NULL;

  if (mFnDict.search (crc,old)) mFnDict.erase (crc);

  mFnDict.insert (crc,fn);  
}

void      USStreamReader::_bind            (const char* cmd,USReader* rdr)
{
  ulong     crc = CRC32 (cmd,strlen (cmd));
  USReader* old = NULL;

  if (mPtrDict.search (crc,old)) mPtrDict.erase (crc);

  mPtrDict.insert (crc,rdr);
}

void      USStreamReader::BindReader      (const char* cmd,USStreamReader* rdr,USPrepareFn fn,DWORD par)
{
  ulong     crc = CRC32 (cmd,strlen (cmd));
  tag_t     old;

  if (mTagDict.search (crc,old)) mTagDict.erase (crc);

  old.stream = rdr;
  old.fn     = fn;
  old.param  = par;

  mTagDict.insert (crc,old);
}

void      USStreamReader::_read            (istream& is)
{
  for (;is;)
  {
    static char cmd [1024];
    ulong       crc;
    tag_t       tag;

    is>>cmd;

    locase (cmd,cmd);

    if (!strcmp (cmd,"}"))
      break;

    if (!mTagDict.search (crc=CRC32 (cmd,strlen (cmd)),tag))
    {
      USReader*   rdr = NULL;

      if (!mPtrDict.search (crc,rdr))
      {
        if (!strcmp (cmd,"msg"))  
        {
          is.getline (cmd,1024);
          (*mOs)<<&cmd[1]<<endl;
        }
        if (!strcmp (cmd,"call"))
        {
          USCallFn fn = NULL;

          is>>cmd;

          locase (cmd,cmd);

          if (mFnDict.search (CRC32 (cmd,strlen (cmd)),fn))
          {
            D_Try
            {
              (*fn)();
            }
            D_Exception  (EXCEPTION_EXECUTE_HANDLER)
            {
              (*mOs)<<"Error at execute function: "<<cmd<<endl;
            }
          }
        }
        continue;
      }

      rdr->read (is);
    }
    else
    {
      if (tag.fn) (*tag.fn)(tag.stream,US_PRECALL,tag.param);

      is>>cmd;

      if (strcmp (cmd,"{"))
        continue;

      tag.stream->read    (is);      

      if (tag.fn) (*tag.fn)(tag.stream,US_POSTCALL,tag.param);
    }
  }
}

void      USStreamReader::read            (const char* name)
{
  mCreateFlag = TRUE;
  mStream     = ext_new ifstream (name,ios::in);

  _read (*mStream);
}

void      USStreamReader::read            (istream& is)
{
  mCreateFlag = FALSE;
  mStream     = &is;

  _read (is);
}

USStreamReader::USStreamReader (Pool* _Pool,ostream& os)
               : MemObject (_Pool),
                 mPtrDict  (_Pool),
                 mFnDict   (_Pool),
                 mTagDict  (_Pool),
                 mCreateFlag (FALSE),
                 mStream    (NULL),
                 mOs        (&os)
{     }

USStreamReader::~USStreamReader ()
{
//  if (mCreateFlag) delete mStream;

     //delete readers
}

void USStringReader::read      (istream& is)
{
  is>>mObj;

  if (mObj[0] == '"' || mObj[0] == 0x27)
  {
    memmove (mObj,mObj+1,strlen(mObj));
    
    char c;
    int  i = strlen (mObj);

    if (!i)
      return;

    if (mObj [i-1] != '"' && mObj [i-1] != 0x27)
    {
      for (is.get (c);is && c != '"' && c != 0x27;is.get(c))
        mObj [i++] = c;  
     
      mObj [i] = 0;    
    }  
    else
      mObj [i-1] = 0;
  }
}

#include <pch.h>
#include <binfile\BinFile.h>
#include <iomanip.h>

size_t            BinFile::read     (void* buf,size_t size,filepos_t pos)
{
  if (canread ())
  {
    bool def = false;

    if (pos < 0) 
    {
      pos = mFilePos;
      def = true;
    }  

    if (pos != mFilePos)
    {
      size_t end  = seek (pos);
      size       -= pos - end;
    }  

    if (pos + size > mFileLen)
      if (!canreadresize ()) size = mFileLen - pos;
      else               
      {
        int end  = pos + size;
        end      = end - resize (end);

        if (pos > 0) size -= end;
      }

    if (lock (pos,size))
    {
      size_t res  = _read (buf,size,pos);

      if (def) 
        mFilePos += res;

      unlock (pos,size);

      return res;
    }
    else return 0;
  }
  else  return 0;
}

size_t          BinFile::write    (const void* buf,size_t size,filepos_t pos)
{
  if (canwrite ())
  {
    bool def = false;

    if (pos < 0)
    {
      pos = mFilePos;
      def = true;
    }

    if (pos != mFilePos)
    {
      size_t end  = seek (pos);
      size       -= pos - end;
    }  

    if (pos + size > mFileLen)
      if (!canresize ()) size = mFileLen - pos;
      else               
      {
        int end  = pos + size;
        end      = end - resize (end);

        if (pos > 0) size -= end;
      }

    if (lock (pos,size))
    {
      size_t res  = _write (buf,size,pos);
     
      if (def) 
        mFilePos += res;

      unlock (pos,size);

      return res;
    }  
    else return 0;   
  }
  else return 0;  
}

bool            BinFile::reset    ()
{
  CSEntry cs (mCS);

  if (canreset () || canseek ())
  {
    if (!_reset ())
      if (_seek (0)) 
        return false;

    mFilePos = 0;  

    return true;
  }
  else return false;
}

BinFile::filepos_t   BinFile::seek     (filepos_t pos)
{
  CSEntry cs (mCS);

  if (canseek ())
  {
    if (pos <= 0) pos = 0;

    if (pos > mFileLen)
    {
      if (canresize ()) mFilePos = _seek (resize (pos));
      else              mFilePos = _seek (mFileLen);
    }
    else mFilePos = _seek (pos); 
  }

  return mFilePos;
}

size_t          BinFile::resize   (size_t size)
{
  CSEntry cs (mCS);

  if (canresize ())   return mFileLen = _resize (size);
  else                return mFileLen;
}

void            BinFile::flush    ()
{
  CSEntry cs (mCS);

  if (canwrite ())
    _flush ();
}

BinFile::BinFile  (int mode)
        : mMode        (mode&BF_FILE_ATTR),
          mFilePos     (0),
          mFileLen     (0)
{    }

BinFile::BinFile (const BinFile& file)
        : mMode     (file.mMode),
          mFilePos  (file.mFilePos),
          mFileLen  (file.mFileLen)
{    }

systime_t::systime_t (filetime_t t)
{
  if (!(t>>32))
  {
    DosDateTimeToFileTime (t>>16,t&0xFFFF,(FILETIME*)&t);
    FileTimeToSystemTime ((FILETIME*)&t,(SYSTEMTIME*)this);    
  }
  else
  {
    FileTimeToSystemTime ((FILETIME*)&t,(SYSTEMTIME*)this);    
    hour += 3;
  }     
}

systime_t& systime_t::operator = (filetime_t t)
{
  if (!(t>>32))
  {
    DosDateTimeToFileTime (t>>16,t&0xFFFF,(FILETIME*)&t);
    FileTimeToSystemTime ((FILETIME*)&t,(SYSTEMTIME*)this);    
  }
  else
  {
    FileTimeToSystemTime ((FILETIME*)&t,(SYSTEMTIME*)this);    
    hour += 3;
  }     

  return *this;
}

ostream& operator << (ostream& os,const systime_t& time)
{
  static const char* months [12] = {
    "january", "february", "march", "april", "may", "june", "july", "august",
    "spetember", "october", "november", "december"
  };
  static const char* days [7] = {
    "sunday", "monday", "tuesday", "wednesday", "thirsday", "friday", "saturday"
  };

/*  os<<"Year: "<<time.year
    <<" month: "<<months [time.month-1]
    <<" day: "<<time.day
    <<" ("<<days [time.dayOfWeek]<<") "
    <<time.hour<<':'<<time.minute<<':'<<time.second;*/
  os<<setfill('0')<<setw(2)<<time.day
    <<'.'<<setfill('0')<<setw(2)<<time.month
    <<'.'<<time.year
    <<" "<<setfill('0')<<setw(2)<<time.hour
    <<':'<<setfill('0')<<setw(2)<<time.minute
    <<':'<<setfill('0')<<setw(2)<<time.second;

  return os;
}
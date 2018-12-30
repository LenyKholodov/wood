#include <pch.h>
#include <binfile\SysFile.h>

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

BinFile::filepos_t  SysBinFile::_seek   (filepos_t pos)
{
  return ::lseek (mSysHandle,pos,SEEK_SET);
}

size_t       SysBinFile::_resize (size_t size)
{
  if (::chsize (mSysHandle,size) == -1)
    return 0;
  
  return ::filelength (mSysHandle);
}

size_t       SysBinFile::_read   (void *buf,size_t size,filepos_t pos)
{
  return ::lseek (mSysHandle,pos,SEEK_SET), ::read (mSysHandle,buf,size);
}

size_t       SysBinFile::_write  (const void *buf,size_t size,filepos_t pos)
{
  return ::lseek (mSysHandle,pos,SEEK_SET), ::write (mSysHandle,buf,size);
}

SysBinFile::SysBinFile  (const char* name,int type)
           : BinFile (type),
             mSysHandle (-1)
{
  if (!canwrite () && canread ())
  {
    mSysHandle  = ::open (name,O_RDONLY | O_BINARY);    
  }
  else if (canwrite ())
  {
    mSysHandle = ::open (name,O_RDWR|O_CREAT|O_BINARY,S_IWRITE|S_IREAD);    

    if (mMode & BF_OPEN_CREATE)
      ::chsize (mSysHandle,0);
  }

  if (mSysHandle != -1)
  {
    mFileLen   = ::filelength(mSysHandle);
  }  
/*  int fmode = 0;

  switch(type)
  {
    case BF_OPEN_RO:
      fmode       = can_read | can_seek | can_reset;
      mSysHandle  = ::open (name,O_RDONLY | O_BINARY);
      break;
    case BF_OPEN_RW:
    case BF_OPEN_CR:
      fmode      = can_read | can_seek | can_write | can_chsize | can_reset;
      mSysHandle = ::open (name,O_RDWR|O_CREAT|O_BINARY,S_IWRITE|S_IREAD);

      if (type == BF_OPEN_CR)
         ::chsize (mSysHandle,0);

      break;
    default: mSysHandle=-1;
  }

  if (mSysHandle < 0)
    return ;

  mFilePos   = 0;
  mFileLen   = ::filelength(mSysHandle);
  mMode      = (States)fmode;*/
}

SysBinFile::~SysBinFile ()
{
  if (mSysHandle >= 0) ::close (mSysHandle);
}
        
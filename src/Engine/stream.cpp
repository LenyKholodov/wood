#include <pch.h>
#include <engine\stream.h>
#include <misc.h>
#include <kernel\thread.h>

msg_t MessageStream::mDefTemp = 0;

////////////////////////////implementation of class MessageStream///////////////////////////

bool  MessageStream::LockWrite (msg_t msg)
{
  mCSWrite.enter ();

  if (mWrite)  return false; 
  else
  {
    if (!check (ST_WRITE) || !valid ())
    {      
      mCSWrite.leave ();
      return mWrite = false;
    }
    else 
    {
      mTrans.enter ();

      clear        (ST_WRITE_TOO_LARGE);

      bool res = _LockWrite (msg);

      mTrans.leave ();

      if (!res)
      {
        if (check (ST_WRITE_WAIT) && !check (ST_WRITE_TOO_LARGE))
        {
          CSEntry trans (mTrans);

          while (1)
          {
            if (!_LockWrite (msg))  
            {
//              dout<<"in write lock wait"<<endl;
  
              WaitWrite ();
  
//              dout<<"out write lock wait"<<endl;
            }  
            else break;
          }           
        }
        else
        {
          mCSWrite.leave ();
          return mWrite = false;
        }
      }

      if (mActions [ACT_LOCK_WRITE].fn)
        mActions [ACT_LOCK_WRITE].fn (ACT_LOCK_WRITE,mActions [ACT_LOCK_WRITE].data);

      return mWrite = true;
    }  
  }
}

void    MessageStream::UnlockWrite ()
{
  CSEntry trans (mCSWrite);    

  if (mWrite)
  {
    CSEntry entry (mTrans);

    if (_UnlockWrite   ())
      mMsgCount++;

    if (mActions [ACT_UNLOCK_WRITE].fn)
      mActions [ACT_UNLOCK_WRITE].fn (ACT_UNLOCK_WRITE,mActions [ACT_UNLOCK_WRITE].data);

    clear          (ST_WRITE_ERROR);
    ResumeRead     ();

    mWrite = false;

    mCSWrite.leave ();
  }
}

size_t  MessageStream::write           (const void* buf,size_t size) 
{
  CSEntry trans (mCSWrite);

  if (mWrite)
  {    
    mTrans.enter ();

    clear        (ST_WRITE_TOO_LARGE);

    size_t res = _write (buf,size);

    mTrans.leave ();
  
    if (res != size && check (ST_WRITE_WAIT) && !check (ST_WRITE_TOO_LARGE))
    {
      size_t  s    = size;

      char*   pos  = (char*)buf + res;
      s           -= res;

      CSEntry trans (mTrans);

      while (1)
      {        
        size_t r  = _write (pos,s);

        res      += r;
        s        -= r;

        if (s > 0) 
        {
//          dout<<"wait from write"<<endl;
          WaitWrite  ();
//          dout<<"end wait from write"<<endl;
        }  
        else       break;
      }
    }

    if (res != size)
      set (ST_WRITE_ERROR);

    return res;
  }
  else return 0;  
}

size_t  MessageStream::PutMessage      (msg_t msg,const void* buf,size_t size)
{
  if (LockWrite (msg))
  {
    size_t res = write (buf,size);
    UnlockWrite ();

    return res;
  }

  return 0;
}

bool    MessageStream::puts (const char* str)
{
  size_t len = strlen (str) + 1;
  return write (str,len) == len;
}

bool      MessageStream::LockRead (msg_t& msg)
{
  mCSRead.enter ();

  if (mRead)  return false; 
  else
  {
    if (!check (ST_READ) || !valid ())
    {      
      mCSRead.leave ();
      return mRead = false;
    }
//    else if (!check (ST_READ_WAIT))
//    {
//      CSEntry trans (mTrans);
//
//      if (!mMsgCount || !_LockRead (msg))
//      {
//        mCSRead.leave ();
//        return mRead = false;
//      }
//      else return mRead = true;
//    }
    else
    {
      mTrans.enter ();

      clear        (ST_READ_TOO_LARGE);

      bool res = mMsgCount && _LockRead (msg);

      mTrans.leave ();

      if (!res)
      {
        if (check (ST_READ_WAIT) && !check (ST_READ_TOO_LARGE))
        {
          CSEntry trans (mTrans);

          while (1)
          {
//            bool res = mMsgCount && _LockRead (msg);

//            if (!res)  
            if (!mMsgCount || !_LockRead (msg))
            {
//              dout<<"lock read wait"<<endl;
              WaitRead ();
//              dout<<"end lock read wait"<<endl;
            }  
            else       break;
          } 
        }
        else
        {
          mCSRead.leave ();
          return mRead = false;
        }
      }  

      if (mActions [ACT_LOCK_READ].fn)
        mActions [ACT_LOCK_READ].fn (ACT_LOCK_READ,mActions [ACT_LOCK_READ].data);

      return mRead = true;
    }  
  }
}

void      MessageStream::UnlockRead  ()
{
  CSEntry trans (mCSRead);    

  if (mRead && mMsgCount)
  {
    CSEntry entry (mTrans);

    if (_UnlockRead   ())
      mMsgCount--;

    if (mActions [ACT_UNLOCK_READ].fn)
      mActions [ACT_UNLOCK_READ].fn (ACT_UNLOCK_READ,mActions [ACT_UNLOCK_READ].data);

    clear          (ST_READ_ERROR);
    ResumeWrite    ();

    mRead = false;

    mCSRead.leave ();
  }
}

size_t  MessageStream::read  (void* buf,size_t size)
{
  CSEntry trans (mCSRead);

  if (mRead)
  {    
    mTrans.enter ();

    clear        (ST_READ_TOO_LARGE);

    size_t res = _read (buf,size);

    mTrans.leave ();
  
    if (res != size && check (ST_READ_WAIT) && !check (ST_READ_TOO_LARGE))
    {
      size_t  s    = size;

      char*   pos  = (char*)buf + res;
      s           -= res;

      CSEntry trans (mTrans);

      while (1)
      {        
        size_t r  = _read (pos,s);

        res      += r;
        s        -= r;

        if (s > 0) 
        {  
//          dout<<"in read wait"<<endl;
          WaitRead  ();
//          dout<<"out read wait"<<endl;
        }  
        else       break;
      }
    }

    if (res != size)
      set (ST_READ_ERROR);

    return res;
  }
  else return 0;  
}

size_t  MessageStream::GetMessage  (msg_t& msg,void* buf,size_t size)
{
  if (LockRead (msg))
  {
    size_t res = read (buf,size);
    UnlockRead ();

    return res;
  }

  return 0;  
}

size_t   MessageStream::_gets (char* str,bool& end)
{
  size_t total = 0;
  for (size_t res = _read (str,1);res && *str;total++,str++,res=_read (str,1));

  if (!*str && res)
    end = true;    

  return total;
}

size_t   MessageStream::gets (char* str)
{
  CSEntry trans (mCSRead);
  bool    end = false;
  size_t  res = 0;

  if (mRead)
  {    
    mTrans.enter ();

    clear        (ST_READ_TOO_LARGE);

    size_t r = _gets (str,end);

    res += r;
    str += r;

    mTrans.leave ();

    if (!end && check (ST_READ_WAIT) && !check (ST_READ_TOO_LARGE))
    {
      CSEntry trans (mTrans);

      while (1)
      {        
        size_t r  = _gets (str,end);

        res      += r;
        str      -= r;

        if (!end)  WaitRead  ();
        else       break;
      }
    }

    *str = 0;

    if (!end)
      set (ST_READ_ERROR);

    return res;
  }
  else return 0;          
}

void    MessageStream::BindAction (Action act,ActionHandler fn,void* data)
{
  mActions [act&3].fn   = fn;
  mActions [act&3].data = data;
}

//в WaitWrite/WaitRead считается что нить вошла уже с захваченным крит. участком кода
//выход в lock

void    MessageStream::WaitWrite ()
{
  if (!mWaitWrite)
  {
    mWaitWrite = K_GetCurrentThread ();

    do    
    {
      mTrans.leave ();

      if (mWaitWrite)
        mWaitWrite->pause ();

      mTrans.enter ();  
    } while (mWaitWrite);
  }
}

void    MessageStream::WaitRead ()
{
  if (!mWaitRead)
  {
    mWaitRead = K_GetCurrentThread ();

    do
    {
      mTrans.leave ();

      if (mWaitRead)
        mWaitRead->pause ();

      mTrans.enter ();
    } while (mWaitRead);
  }
}

void    MessageStream::ResumeWrite  ()
{
  CSEntry trans (mTrans);
  if (mWaitWrite)
  {
    Thread* thread = mWaitWrite;
    mWaitWrite     = NULL;
    thread->resume ();
  }  
}

void    MessageStream::ResumeRead ()
{
  CSEntry trans (mTrans);
  if (mWaitRead)
  {
    Thread* thread = mWaitRead;
    mWaitRead      = NULL;

    thread->resume ();
  }  
}

MessageStream::MessageStream  () 
              : mWrite (false), mRead (false),
                mMsgCount (0), 
                mWaitWrite (NULL),
                mWaitRead (NULL)
{   
  BindAction (ACT_LOCK_WRITE);
  BindAction (ACT_UNLOCK_WRITE);
  BindAction (ACT_LOCK_READ);
  BindAction (ACT_UNLOCK_READ);
} 

MessageStream::~MessageStream () 
{
  UnlockRead  (); //??
  UnlockWrite (); //??
}


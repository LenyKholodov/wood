#include <pch.h>
#include <engine\server.h>

bool    Connection::BindWriteStream (MessageStream& s)
{
  if (!mWrite)
  {
    mWrite = &s;

    if (mWrite)
      mWrite->BindAction (MessageStream::ACT_UNLOCK_WRITE,
                          (MessageStream::ActionHandler)ConnectionHandler,
                          this
                         );

    return true;
  }
  else return false;
}

bool    Connection::BindReadStream  (MessageStream& s)
{
  return mRead ? false : mRead = &s;
}

bool    Connection::BindMessage   (msg_t from,msg_t to)
{
  return mMsgMap.insert (from,to);
}

void    Connection::UnbindMessage (msg_t msg)
{
  mMsgMap.erase (msg);
}

void    Connection::BindServer   (Server& srv)
{
  mServer = &srv;
}

void    Connection::UnbindServer ()
{
  mServer = NULL;    
}

msg_t   Connection::ReindexMsg   (msg_t from) const
{
  msg_t msg = -1;

  return mMsgMap.find (from,msg) ? msg : -1;
}

void  Connection::ConnectionHandler (MessageStream::Action act,Connection& con)
{
  if (con.mServer)
    con.mServer->ConnectionHandler (act,con);
}

Connection::Connection  (Pool* pool)
           : mMsgMap (pool),
             mWrite  (NULL), mRead (NULL),
             mServer (NULL)
{   }

Connection::~Connection ()
{
  if (mWrite)
    mWrite->BindAction (MessageStream::ACT_UNLOCK_WRITE);

  if (mServer)
    mServer->DelConnection (*this);
}


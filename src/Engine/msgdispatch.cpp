#include <pch.h>
#include <engine\server.h>
#include <pool.h>

bool MessageHandler::ProcessMessage (msg_t msg,MessageStream& s,void* self)
{
  D_Try
  {
    return ((MessageHandler*)self)->process (msg,s);
  }
  D_ExcRet ("MessageHandler: Error at process message: "<<msg<<" at handler "<<self,false);
}

bool    MsgDispatch::BindMessage   (msg_t from,msg_t to,MsgHandler fn,void* self)
{
  if (!fn) 
  {
    fn    = ProcessMessage;
    self  = this;
  }

  if (to == -1)
    to = from;

  msgnode_t node;

  node.fn    = fn;
  node.data  = self;
  node.toMsg = to;

  MsgDict::diter ptr = mDict.find (from);

  if (!ptr.valid ()) return mDict.insert (from,node);
  else               return false;
}

bool    MsgDispatch::BindMessage   (MessageHandler& handler,msg_t from,msg_t to)
{
  return BindMessage (from,to,MessageHandler::ProcessMessage,&handler);
}

void    MsgDispatch::UnbindMessage (msg_t msg)
{
  mDict.erase (msg);
}

bool    MsgDispatch::PutMessage    (msg_t msg,MessageStream& s)
{
  return process (msg,s);
}

bool    MsgDispatch::process  (msg_t msg,MessageStream& s)
{
  MsgDict::diter ptr = mDict;

  if (ptr.valid ())
  {
    if (!ptr->fn) return false;

    return ptr->fn (ptr->toMsg,s,ptr->data);
  }
  else return false;
}

MsgDispatch::MsgDispatch   (Pool* pool)
            : mDict (pool)
{   }


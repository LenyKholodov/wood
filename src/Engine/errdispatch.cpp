#include <pch.h>
#include <engine\server.h>
#include <pool.h>

bool ErrorHandler::ProcessError (GameError& err,void* self)
{
  D_Try
  {
    return ((ErrorHandler*)self)->process (err);
  }
  D_ExcRet ("ErrorHandler: Error at process error: "<<err<<" at handler "<<self,false);
}

bool    ErrorDispatch::BindError (err_t err,ErrHandler fn,void* self)
{
  if (!fn) 
  {
    fn    = ProcessError;
    self  = this;
  }

  errnode_t node;

  node.fn    = fn;
  node.data  = self;

  ErrDict::diter ptr = mDict.find (err);

  if (!ptr.valid ()) return mDict.insert (err,node);
  else               return false;
}

bool    ErrorDispatch::BindError  (ErrorHandler& handler,err_t err)
{
  return BindError (err,ErrorHandler::ProcessError,&handler);
}

void    ErrorDispatch::UnbindError (err_t err)
{
  mDict.erase (err);
}

bool    ErrorDispatch::raise (GameError& err)
{
  return process (err);
}

bool    ErrorDispatch::process  (GameError& err)
{
  ErrDict::diter ptr = mDict;

  if (ptr.valid ())
  {
    if (!ptr->fn) return false;

    if (!ptr->fn (err,ptr->data))
    {
      if (mOwner) return mOwner->process (err);
      else        return false;
    }
    else return true;    
  }
  else return false;  
}

ErrorDispatch::ErrorDispatch  (ErrorHandler* owner,Pool* pool)
            : mDict  (pool),
              mOwner (owner)
{  }
  
ErrorDispatch::ErrorDispatch  (ErrorDispatch& disp)
            : mDict  (disp.mDict.GetPool ()),
              mOwner (&disp)
{  }
  
#include <pch.h>
#include <engine\server.h>
#include <kernel\thread.h>

class    ServerThread: public Thread
{
  public:
                ServerThread (Server& srv): mServer (srv), Thread (false) {
                  StartThread ();
                }

    virtual int  process () { 
        return mServer.ProcessThread ();
    }   

  private:
    Server&     mServer;
};

bool  Server::AddConnection (Connection& con)
{
  if (!mConnections.insert (&con))
    return false;

  con.UnbindServer ();
  con.BindServer   (*this); 

  return true;
}

void  Server::DelConnection (Connection& con)
{
  mConnections.erase (&con);
  con.UnbindServer   ();
}

bool  Server::ProcessMessage (msg_t msg,MessageStream&,MessageStream&)
{
  dout<<"Server: Error at process message: "<<msg<<endl;
  return false;
}

void  Server::ConnectionHandler (MessageStream::Action act,Connection& con)
{
  if (act == MessageStream::ACT_UNLOCK_WRITE)
  {
    CSEntry trans (mMsgTrans);

    if (mWaitMsgNum <= mMsgList.count () || !mMsgList.insert (&con))
    {
      mWaitThread = K_GetCurrentThread ();

      while (mWaitThread && mWaitMsgNum <= mMsgList.count () || !mMsgList.insert (&con))      
      {
        if (mWaitThread) mWaitThread->pause ();
      }
    }        

    if (mSleep && mThread)
      mThread->resume ();
  }
}

int   Server::ProcessThread     ()
{
  D_Try
  {
    while (1)
    {
      Connection* con = NULL;

      if (mWaitThread)
      {
        con = mMsgList.GetFirst ().data ();
        mMsgList.erase (con);

        Thread* thread = mWaitThread;
        mWaitThread    = NULL;

        thread->resume ();
      }
      else
      {
        mMsgTrans.enter ();

        if (!mMsgList.count ())
        {
          while (!mMsgList.count ())
          {
            mSleep = true;

            mMsgTrans.leave ();
            mThread->pause  ();
            mMsgTrans.enter ();
          }  
        }

        con = mMsgList.GetFirst ().data ();
        mMsgList.erase (con);

        mMsgTrans.leave ();
      }

      msg_t msg = -1;

      if (con->ws ().LockRead (msg))
      {
        msg = con->ReindexMsg (msg);

        if (msg != (msg_t)-1)
        {
          D_Try
          {        
            ProcessMessage (msg,con->ws (),con->rs ());
          }
          D_Exc ("Server: Error at process message: "<<msg);  
        }      

        con->ws ().UnlockRead ();
      }    
    }

    return 0;
  }
  D_ExcRet ("Server: Error at ProcessThread",1); //??
}

Server::Server (Pool* pool)
       : MemObject (pool),
         mSleep    (false),
         mThread   (NULL),
         mMsgList  (pool),
         mConnections (pool),
         mWaitThread (NULL)         
{   
  mThread = new (pool) ServerThread (*this);
}

Server::~Server ()
{
   //?????

  if (mThread)
  {
    delete [] mThread;
  }  
}
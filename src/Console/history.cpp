#include <pch.h>
#include <console\history.h>
#include <misc.h>
#include <string.h>

void         ConsoleHistory::first           (hisnode_t* node)
{
  if (node->next) node->next->prev = node->prev;
  else            mLast            = node->prev;
  if (node->prev) node->prev->next = node->next;
  else            mFirst           = node->next;

  node->prev = NULL;
  node->next = mFirst;

  if (mFirst)  
    mFirst->prev = node;
  
  mFirst = node;
}

void         ConsoleHistory::remember        (const char* cmd)
{
  D_Try
  {
    if (strlen (cmd) >= COMMAND_LINE_LEN)
      return;

    ulong crc = CRC32 (cmd,strlen (cmd));

    for (hisnode_t* node=mFirst;node;node=node->next)
      if (node->crc == crc && !strcmp (cmd,node->command))
        break;

    if (!node)
    {
      node      = mLast;
      node->crc = crc;
      
      strcpy (node->command,cmd);
    }

    first (node);

    mPos = mFirst;
  }
  D_Exc ("ConsoleHistury: Error at remember");
}

const char*  ConsoleHistory::up              ()
{
  D_Try
  {
    if (!mPos)
      return NULL;
    else
    {
      if (mPos->next && mPos->next->command [0])  
      {
        mPos = mPos->next;
        return mPos->prev->command;
      }
      else
        return mPos->command;          
    }
  }
  D_ExcRet ("ConsoleHistory: Error at up",NULL);  
}

const char*  ConsoleHistory::down            ()
{
  D_Try
  {
    if (!mPos || mPos == mFirst)
      return NULL;
    else
    {
      if (mPos->prev)
      {
        mPos = mPos->prev;
        return mPos->next->command;
      }  
      else
        return mPos->command;
    }
  }
  D_ExcRet ("ConsoleHistory: Error at down",NULL);  
}
        
ConsoleHistory::ConsoleHistory  (Pool* _Pool,int num)
               : MemObject (_Pool),
                 mNode     (NULL),
                 mFirst    (NULL), mLast (NULL), mPos (NULL)
{
//  D_Try
//  {
    mNode = (hisnode_t*)alloc (sizeof (hisnode_t) * num);     

    if (!mNode)
    {
      D_Message ("ConsoleHistory: No enough memory for sys data");
      return;
    }

    for (int i=0;i<num;i++)
    {
      mNode [i].command [0] = 0;
      mNode [i].crc         = 0;
      mNode [i].prev        = &mNode [i-1];
      mNode [i].next        = &mNode [i+1];
    }

    mFirst = mNode;
    mLast  = &mNode [num-1];

    mFirst->prev = NULL;
    mLast->next  = NULL;
//  }
//  D_Exc ("ConsoleHistory: Error at construct");
}

ConsoleHistory::~ConsoleHistory ()
{
//  D_Try
//  {
    if (mNode) free (mNode);
//  }
//  D_Exc ("ConsoleHistory: Error at destruct");
}


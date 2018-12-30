#include <pch.h>
#include <console\command.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>

uint    anal (const char* str)
{
  BOOL dig = TRUE;
  BOOL toc = FALSE;

  for (;*str;str++)
  {
    if (!isdigit (*str) && *str != 'x') dig = FALSE;
    if (*str == '.')                    toc = TRUE;
  }  

  return !dig ? VarPool::VAR_CHPTR :
         toc  ? VarPool::VAR_FLOAT : VarPool::VAR_INT;
}

/////////////////////implementation of class Command////////////////////////////

BOOL  Command::bind            (const char* cmd,CommandFn fn,DWORD par,int attr)
{
  return vpool.set (cmd,fn,par,mAccess,attr);
}

BOOL  Command::alias           (const char* src,const char* dest)
{
  return vpool.alias (src,dest);
} 

void  Command::unbind          (const char* cmd)
{
  vpool.kill (cmd);
}

void  Command::LoadConfig      (const char* fname)
{
//  D_Try
//  {
    ifstream is (fname,ios::in);
    char    cmd [COMMAND_LINE_LEN];

    for (;is;)
    {
      is.getline (cmd,COMMAND_LINE_LEN);
      command    (cmd);    
    }
//  }
//  D_Exc ("Command: Error at LoadConfig");
}

void  Command::SaveConfig      (const char* fname)
{
//  D_Try
//  {
    fstream os (fname,ios::out);

    for (int i=0;i<vpool.mCount;i++)
    {
      comvar_t* var = vpool.mVarPool.data (i);      

      if (var->use && var->type != VarPool::VAR_FUNC && 
          var->defvar.i != var->var.i)
      {
        os<<"set "<<var->name<<" ";

        switch (var->type)
        {
          case VarPool::VAR_INT: os<<var->var.i<<endl; break;
          case VarPool::VAR_INT_PTR: os<<*var->var.iptr<<endl; break;
          case VarPool::VAR_FLOAT: os<<var->var.f<<endl; break;
          case VarPool::VAR_FLOAT_PTR: os<<*var->var.fptr<<endl; break;
          case VarPool::VAR_CHPTR:     os<<var->var.chptr<<endl; break; 
          default:
            os<<endl;
            break;
        }
      }    
    }      
//  }  
//  D_Exc ("Command: Error at LoadConfig");
}

void  Command::changevar       ()
{
  switch (mPStack.count ())
  {      
    case 0:
    {
      char* var = mPStack.command ();
      
      switch (vpool.type (var))
      {
        case VarPool::VAR_INT:
        case VarPool::VAR_INT_PTR:
          (*mOs)<<'"'<<var<<'"'<<" is: "<<'"'<<vpool.geti (var,mAccess)<<'"'
                <<" default: "<<'"'<<vpool.getdefi (var,mAccess)<<'"'
                <<endl;
          break;
        case VarPool::VAR_FLOAT:
        case VarPool::VAR_FLOAT_PTR:
          (*mOs)<<'"'<<var<<'"'<<" is: "<<'"'<<vpool.getf (var,mAccess)<<'"'
                <<" default: "<<'"'<<vpool.getdeff (var,mAccess)<<'"'
                <<endl;
          break;
        case VarPool::VAR_CHPTR:
          (*mOs)<<'"'<<var<<'"'<<" is: "<<'"'<<vpool.gets (var,mAccess)<<'"'
                <<" default: "<<'"'<<vpool.getdefs (var,mAccess)<<'"'
                <<endl;
          break;          
        default: return;  
      }
     
      break;
    }  
    case 1:
    {
      switch (anal (mPStack.gets (0)))
      {
        case VarPool::VAR_INT:
          vpool.set (mPStack.command (),mPStack.geti (0),mAccess);
          break;
        case VarPool::VAR_FLOAT:
          vpool.set (mPStack.command (),mPStack.getf (0),mAccess);
          break;
        case VarPool::VAR_CHPTR:
          vpool.set (mPStack.command (),mPStack.gets (0),mAccess);
          break;
        default:
          return;
      }

      break;
    }
    default: return;
  }
}

void  Command::command         (const char* cmd)
{
  D_Try
  {     
    if (cmd [0] == '\\')
      cmd++;
     
    if (strlen (cmd) >= COMMAND_LINE_LEN)
    {
      (*mOs)<<"Command too long"<<endl;
      return;
    }     

    mPStack.refresh (cmd);

    if (!mPStack.command ())
      return;

    DWORD     par = 0;
    CommandFn fn  = vpool.getfunc (mPStack.command (),par,mAccess);

    if (!fn)
    {
      if (vpool.exist (mPStack.command ())) 
      {
        changevar ();
        return;
      }  
      (*mOs)<<"Unknown command "<<'"'<<mPStack.command ()<<'"'<<endl;
      return;
    }  

    (*fn)(vpool,mAccess,mPStack,*mOs,par);
  } 
  D_Exc ("Command: Error at command");
}
 
Command::Command  (Pool* _Pool,int _VarNum)
        : MemObject (_Pool),
          vpool     (_Pool,_VarNum),
          mPStack   (NULL),
          mAccess   (VarPool::AC_FULL),
          mOs       (&dout)
{
  bind ("set",CmdSet);
  bind ("kill",CmdKill);
  bind ("alias",CmdAlias);
  bind ("killmacros",CmdKillMacros);
}

Command::~Command ()
{
}

////////////////////default commands////////////////////////////////////////////

void    CmdSet    (VarPool& vp,uint access,ParamStack& pstack,ostream& os,DWORD)
{
  D_Try
  {
    switch (pstack.count ())
    {      
      case 1:
      {
        char* var = pstack.gets (0);
        
        switch (vp.type (var))
        {
          case VarPool::VAR_INT:
          case VarPool::VAR_INT_PTR:
            os<<'"'<<var<<'"'<<" is: "<<'"'<<vp.geti (var,access)<<'"'
              <<" default: "<<'"'<<vp.getdefi (var,access)<<'"'
              <<endl;
            break;
          case VarPool::VAR_FLOAT:
          case VarPool::VAR_FLOAT_PTR:
            os<<'"'<<var<<'"'<<" is: "<<'"'<<vp.getf (var,access)<<'"'
              <<" default: "<<'"'<<vp.getdeff (var,access)<<'"'
              <<endl;
            break;
          case VarPool::VAR_CHPTR:
            os<<'"'<<var<<'"'<<" is: "<<'"'<<vp.gets (var,access)<<'"'
              <<" default: "<<'"'<<vp.getdefs (var,access)<<'"'
              <<endl;
            break;          
          default: return;  
        }
       
        break;
      }  
      case 2:
      {
        switch (anal (pstack.gets (1)))
        {
          case VarPool::VAR_INT:
            vp.set (pstack.gets (0),pstack.geti (1),access);
            break;
          case VarPool::VAR_FLOAT:
            vp.set (pstack.gets (0),pstack.getf (1),access);
            break;
          case VarPool::VAR_CHPTR:
            vp.set (pstack.gets (0),pstack.gets (1),access);
            break;
          default:
            return;
        }

        break;
      }
      default: 
        os<<"set <variable> <value>"<<endl;
        return;
    }
  }
  D_Exc ("CmdSet: Error at execute");
}

void    CmdAlias  (VarPool& vp,uint access,ParamStack& pstack,ostream& os,DWORD)
{
  if (pstack.count () < 2)
  {
    os<<"alias <command> <macros>"<<endl;
    return;
  }

  vp.alias (pstack.gets (0),pstack.gets (1));
}

void    CmdKill   (VarPool& vp,uint access,ParamStack& pstack,ostream& os,DWORD)
{
  if (pstack.count () < 1)
  {
    os<<"kill <var>"<<endl;
    return;
  }

  vp.kill (pstack.gets (0),access);
}

void    CmdKillMacros   (VarPool& vp,uint access,ParamStack& pstack,ostream& os,DWORD)
{
  if (pstack.count () < 1)
  {
    os<<"kill <var>"<<endl;
    return;
  }

  vp.killmacros (pstack.gets (0));
}

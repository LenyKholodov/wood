#include <pch.h>
#include <console\vpool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <misc.h>

BOOL      VarPool::compatible  (int prior,int attr) const
{
  if ((attr & VAR_READ_ONLY) && !(prior & VAR_READ_ONLY))    return FALSE;
  if ((attr & VAR_CHEAT) && !(prior & VAR_CHEAT))    return FALSE;
  if ((attr & VAR_SYSTEM) && !(prior & VAR_SYSTEM))    return FALSE;
  if ((attr & VAR_USER) && !(prior & VAR_USER))    return FALSE;
  return TRUE;
}

comvar_t* VarPool::create (const char* name)
{
  D_Try
  {
    if (!name) return NULL;

    comvar_t* var = mVarPool.alloc ();

    if (var)
    {
      mDict.insert (CRC32 (name,strlen (name)),mVarPool.index (var));
      strcpy (var->name,name);
      var->use = TRUE;
      mPos++;
    }  

    return var;
  }
  D_ExcRet ("VarPool: Error at create",NULL);
}

comvar_t* VarPool::find  (const char* name) const
{
  if (!name) return NULL;

  ulong      crc = CRC32 (name,strlen (name));
  int        i   = -1;
                
  if (!mDict.search (crc,i))    return NULL;
  else                          
  {
    comvar_t* var = mVarPool.data (i);  

    if (var->type != VAR_MACROS)  return var;
    else                          return var->var.i != -1 ? mVarPool.data (var->var.i) : NULL;
  }  
}

BOOL    VarPool::alias   (const char* src,const char* dest)
{
  D_Try
  {
    comvar_t* var = find (src);
    comvar_t* mac = find (dest);

    if (!var)
      return FALSE;

    if (!mac)
    {
      mac         = create (dest);

      if (!mac) return FALSE;

      mac->attr     = VAR_USER;
      mac->type     = VAR_MACROS;
      mac->var.i    = mVarPool.index (var);

      return TRUE;
    }
    else if (mac->type == VAR_MACROS)
    {
      if (mac->attr & VAR_READ_ONLY)
        return FALSE;

      mac->var.i = mVarPool.index (var);

      return TRUE;
    }    
    else
      return FALSE;
  }
  D_ExcRet ("VarPool: Error at alias",FALSE);
}

BOOL    VarPool::set     (const char* name,int value,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr     = attr;
      var->type     = VAR_INT;
      var->var.i    = value;
      var->defvar.i = value;

      return TRUE;
    }
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_INT:       var->var.i = value; break;
        case VAR_INT_PTR:   *var->var.iptr = value; break;
        case VAR_FLOAT:     var->var.f = value; break;
        case VAR_FLOAT_PTR: *var->var.fptr = value; break;
        case VAR_STRING:
        {
          char buf [128];
          
          itoa (value,buf,10);

          if (mStrPool.realloc (strlen (buf)+1,&var->var.chptr))
          {
            strcpy (var->var.chptr,buf);
            break;
          }
        }  
        case VAR_CHPTR: itoa (value,var->var.chptr,10); break;
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at set (int)",FALSE);
}

BOOL    VarPool::set     (const char* name,float value,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior & VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr     = attr;
      var->type     = VAR_FLOAT;
      var->var.f    = value;
      var->defvar.f = value;

      return TRUE;
    }
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_INT:       var->var.i = value; break;
        case VAR_INT_PTR:   *var->var.iptr = value; break;
        case VAR_FLOAT:     var->var.f = value; break;
        case VAR_FLOAT_PTR: *var->var.fptr = value; break;
        //case VAR_STRING: ???
        case VAR_CHPTR: sprintf (var->var.chptr,"%f",value); break;
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at set (float)",FALSE);
}

BOOL    VarPool::set     (const char* name,CommandFn fn,DWORD param,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr      = attr;
      var->type      = VAR_FUNC;
      var->var.fn    = fn;
      var->defvar.fn = fn;
      var->param     = param;
      var->defParam  = param; 

      return TRUE;      
    }  
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_FUNC:  
          var->var.fn = fn;
          var->param  = param;
          break; 
        case VAR_STRING:
        {
          char buf [128];
          
          sprintf (buf,"%p",fn);

          if (mStrPool.realloc (strlen (buf)+1,&var->var.chptr))
          {
            strcpy (var->var.chptr,buf);
            break;
          }
        }  
        case VAR_CHPTR:
          sprintf (var->var.chptr,"%p",fn);
          break;
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at set (func)",FALSE);
}

BOOL    VarPool::set     (const char* name,char* str,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
          //creat string in pool

      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr         = attr;
      var->type         = VAR_STRING;
      var->var.chptr    = mStrPool.alloc (strlen (str)+1,&var->var.chptr);
      var->defvar.chptr = NULL;

      if (!var->var.chptr)
      {
        kill (name);
        return FALSE;
      }

      strcpy (var->var.chptr,str);

      return TRUE;      
    }  
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_INT:           var->var.i = atoi (str); break;
        case VAR_INT_PTR:       *var->var.iptr = atoi (str); break;
        case VAR_FLOAT:         var->var.f = atof (str); break;
        case VAR_FLOAT_PTR:     *var->var.fptr = atoi (str); break;
        case VAR_FUNC:  //!!!
        {
          char* buf = NULL;
          var->var.fn = (CommandFn)strtoul (str,&buf,16);
          break;
        }  
        case VAR_CHPTR:
          strcpy (var->var.chptr,str);
          break;
        case VAR_STRING:
        {
          char* old  = var->var.chptr;
          char* dest = mStrPool.alloc (strlen (str)+1,&var->var.chptr);

          if (dest)
          {
            strcpy (dest,str);
            mStrPool.free (old);
            var->var.chptr = dest;
          }
          else
          {
            var->var.chptr = old;
            return FALSE;
          }  

          break;
        }  
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at set (char*)",FALSE);
}

BOOL    VarPool::link    (const char* name,int* value,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr        = attr;
      var->type        = VAR_INT_PTR;
      var->var.iptr    = value;
      var->defvar.iptr = value;

      return TRUE;
    }
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_INT_PTR: var->var.iptr = value; break;
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at link (int*)",FALSE);
}

BOOL    VarPool::link    (const char* name,char* value,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr         = attr;
      var->type         = VAR_CHPTR;
      var->var.chptr    = value;
      var->defvar.chptr = value;

      return TRUE;
    }
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_STRING:
        {
          kill (name);
          return link (name,value,prior,attr);
        }  
        case VAR_CHPTR: var->var.chptr = value; break; 
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at link (char*)",FALSE);
}

BOOL    VarPool::link    (const char* name,float* value,int prior,int attr)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
    {
      if (!(prior&VAR_USER) || (prior&VAR_READ_ONLY))
        return FALSE;

      var         = create (name);

      if (!var) return FALSE;

      var->attr         = attr;
      var->type         = VAR_FLOAT_PTR;
      var->var.fptr     = value;
      var->defvar.fptr  = value;

      return TRUE;
    }
    else
    {
      if (!compatible (prior,attr))
        return FALSE;

      switch (var->type)
      {
        case VAR_FLOAT_PTR: var->var.fptr = value; break; 
        default:        return FALSE;
      }

      return TRUE;
    }
  }
  D_ExcRet ("VarPool: Error at link (float*)",FALSE);
}

BOOL    VarPool::restore (const char* name,int prior)
{
  D_Try
  {
    comvar_t* var = find (name);

    if (!var)
      return FALSE;

    if (!compatible (prior,var->attr))
      return FALSE;

    var->var   = var->defvar;
    var->param = var->defParam;

    return TRUE;     
  }
  D_ExcRet ("VarPool: Error at restore",FALSE); 
}

BOOL    VarPool::kill    (const char* name,int prior)
{
  D_Try
  {
    if (prior & VAR_READ_ONLY)
      return FALSE;

    comvar_t* var = find (name);

    if (!var)
      return FALSE;

    if (!compatible (prior,var->attr))
      return FALSE;

    var->use = FALSE;

    if (var->type == VAR_STRING)
      mStrPool.free (var->var.chptr);

    mDict.erase (CRC32 (name,strlen (name)));
    mVarPool.free (var);

    mPos--;

    return TRUE;
  }
  D_ExcRet ("VarPool: Error at kill",FALSE);
}

BOOL        VarPool::killmacros  (const char* name)
{
  D_Try
  {
    comvar_t* var = NULL;

    ulong      crc = CRC32 (name,strlen (name));
    int        i   = -1;
                
    if (!mDict.search (crc,i)) var = NULL;
    else                       var = mVarPool [i];  

    if (!var || var->type != VAR_MACROS)
      return FALSE;    

    mDict.erase (CRC32 (name,strlen (name)));
    mVarPool.free (var);

    var->use = FALSE;

    mPos--;

    return TRUE;
  }
  D_ExcRet ("VarPool: Error at killmacros",FALSE); 
}

CommandFn   VarPool::getfunc (const char* name,DWORD& par,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return NULL;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_FUNC: 
        par = var->param;
        return var->var.fn;
      default: return NULL; 
    }    
  }
  D_ExcRet ("VarPool: Error at getfunc",NULL);
}

int     VarPool::geti    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return NULL;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_INT: return var->var.i; 
      case VAR_INT_PTR: return *var->var.iptr; 
      case VAR_FLOAT: return var->var.f; 
      case VAR_FLOAT_PTR: return *var->var.fptr;
      case VAR_STRING:
      case VAR_CHPTR:     return atoi (var->var.chptr); 
      default: return 0; 
    }    
  }
  D_ExcRet ("VarPool: Error at geti",0);
}

const char*   VarPool::gets    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return NULL;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_STRING:
      case VAR_CHPTR:     return var->var.chptr;      
      default: return NULL; 
    }    
  }
  D_ExcRet ("VarPool: Error at gets",NULL);
}

float   VarPool::getf    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return 0;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_INT: return var->var.i; 
      case VAR_INT_PTR: return *var->var.iptr; 
      case VAR_FLOAT: return var->var.f; 
      case VAR_FLOAT_PTR: return *var->var.fptr;
      case VAR_STRING:
      case VAR_CHPTR:     return atof (var->var.chptr); 
      default: return 0;      
    }    
  }
  D_ExcRet ("VarPool: Error at getf",0);
}

int     VarPool::getdefi    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return 0;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_INT: return var->defvar.i; 
      case VAR_INT_PTR: return *var->defvar.iptr; 
      case VAR_FLOAT: return var->defvar.f; 
      case VAR_FLOAT_PTR: return *var->defvar.fptr;
      case VAR_STRING:
      case VAR_CHPTR:     return atoi (var->defvar.chptr); 
      default: return 0; 
    }    
  }
  D_ExcRet ("VarPool: Error at getdefi",0);
}

const char*   VarPool::getdefs    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return NULL;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_CHPTR:     return var->defvar.chptr;
      case VAR_STRING:    return var->var.chptr;
      default: return NULL; 
    }    
  }
  D_ExcRet ("VarPool: Error at getdefs",NULL);
}

float   VarPool::getdeff    (const char* name,int prior) const
{
  D_Try
  {
    comvar_t* var  = find (name);

    if (!var)
      return 0;

    if (!compatible (prior,var->attr))
      return FALSE;

    switch (var->type)
    {
      case VAR_INT: return var->defvar.i; 
      case VAR_INT_PTR: return *var->defvar.iptr; 
      case VAR_FLOAT: return var->defvar.f; 
      case VAR_FLOAT_PTR: return *var->defvar.fptr;
      case VAR_STRING:
      case VAR_CHPTR:     return atof (var->defvar.chptr); 
      default: return 0;      
    }    
  }
  D_ExcRet ("VarPool: Error at getdeff",0);
}

int     VarPool::attr  (const char* name) const 
{
  comvar_t* var = find (name);

  return var ? var->attr : -1;
}

void    VarPool::attr  (const char* name,int prior,int attr)
{  
  comvar_t* var = find (name);

  if (var && (prior==AC_FULL))
    var->attr = attr;
}

int     VarPool::type  (const char* name) const 
{
  comvar_t* var = find (name);

  return var ? var->type : -1;
}

void    VarPool::type  (const char* name,int type,int prior)
{  
  comvar_t* var = find (name);

  if (var && (prior==AC_FULL))
  {
    if (type == VAR_STRING || var->type == VAR_STRING)
      return;

    var->type = type;
  }  
}

int     VarPool::strncmp     (const char* s1,const char* s2,int n,BOOL& flag)
{
  for (int pos=0;pos < n && *s1==*s2;s1++,s2++,pos++);
  if  (!*s1 && !*s2 && pos == n) flag = TRUE;
  else                           flag = FALSE;  

  return pos;
}

int     VarPool::helpfind       (char* cmd,ostream& os)
{
  D_Try
  {
    int  len   = strlen (cmd);
    int  cnt   = 0;
    int  index = -1;
    BOOL flag  = FALSE;

    for (int i=0;i<mCount;i++)
      if (mVarPool.data (i)->use &&
          strncmp (cmd,mVarPool.data (i)->name,len,flag) == len)
      {
        if (flag)
        {
          strcpy (cmd,mVarPool.data (i)->name);
          strcat (cmd," ");
          return 1;
        }  

        cnt++;        
        index = i;
      }  

    switch (cnt)
    {
      case 0: return 0;
      case 1: 
        strcpy (cmd,mVarPool.data (index)->name); 
        strcat (cmd," ");
        return 1;
      default:
        os<<"\\"<<cmd<<endl;
        for (int i=0;i<mCount;i++)
          if (mVarPool.data (i)->use && (mVarPool.data (i)->type == VAR_FUNC) && strncmp (cmd,mVarPool.data (i)->name,len,flag) == len)
            os<<"\t"<<mVarPool.data (i)->name<<endl;

        return cnt;
    }
  }
  D_ExcRet ("VarPool: Error at helpfind",0);
}

VarPool::VarPool  (Pool* _Pool,uint _Num,size_t _Size)
        : MemObject (_Pool),
          mVarPool  (_Pool,_Num),
          mStrPool  (_Pool,_Size),
          mDict     (_Pool),
          mCount    (_Num), mPos (0)                  
{   
//  D_Try
//  {
    for (int i=0;i<mCount;i++)
      mVarPool.data (i)->use = FALSE;
//  }
//  D_Exc ("VarPool: Error at construct");
}

VarPool::~VarPool ()
{   }

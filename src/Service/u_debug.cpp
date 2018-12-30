#include <pch.h>
#include <debug.h>
#include <fstream.h>

accessptr_t<DebugService>    dbgService;

void        DebugService::BindMap   (const char* mf)
{
  if (!this)
    return;

  if (mf)  strcpy (mMapFile,mf);
  else     mMapFile [0] = 0;
}

uint        DebugService::Dump       (void* ptr,ostream& os)  const
{
  return Dump (ptr,mMapFile,os);
}

uint        DebugService::Dump       (void* ptr,const char* map,ostream& os) 
{
  LPEXCEPTION_POINTERS info = (LPEXCEPTION_POINTERS)ptr;

  if (!info || !&os)
    return EXCEPTION_EXECUTE_HANDLER;

  char  buf [128];
  ulong code = info->ExceptionRecord->ExceptionCode;

  os<<"Exception: "<<GetExceptionMessage (code,buf)<<endl;
  PrintCrash (info->ExceptionRecord->ExceptionAddress,map,os);

  return EXCEPTION_EXECUTE_HANDLER;
}

void        DebugService::PrintCrash (void* crash_addr,ostream& os) const
{
  PrintCrash (crash_addr,mMapFile,os);
}

void        DebugService::PrintCrash (void* crash_addr,const char* map,ostream& os) 
{
  os<<"Crash at addr: "<<crash_addr<<endl;
  os<<"Implementation not realesed"<<endl;
/*

  if (!map || !*map)
  {
    entry = false;
    return ;
  }  

  ifstream is   (map);
  char     file [1024], func [1024];         
  long     line  = -1;
  ulong    bytes = 0;
  void*    start = D_FindLoadAddr (is);
  
  if (!start)
  {
    entry = false;
    return;
  }  

  if (!D_FindFuncName  (is,crash_addr,func,file))
  {
    entry = false;
    return;
  }  

  os<<"\tFunc: '"<<func<<"' ("<<file<<")"<<endl;

  line = D_FindLine (is,crash_addr,start,func,file,bytes);

  if (line == -1)
  {
    entry = false;
    return;
  }  

  os<<"\tFile: '"<<file<<endl;
  os<<"\tLine:  "<<line<<" + "<<bytes<<" bytes"<<endl;   

  entry = false;
*/
}

static ulong  ReadLine (char*& str,char* dest,char from = ' ',char to =' ')
{
  ulong len = 0;

  while (*str && *str == from) str++;
  while (*str && *str != to)   { *dest++ = *str++; len++; }

  *dest = 0;  

  return len;
}

char*     DebugService::GetExceptionMessage (DWORD code,char* buffer)
{
  char* message;

  switch (code)
  {
    case  EXCEPTION_ACCESS_VIOLATION:         message = "Access violation";         break;
    case  EXCEPTION_DATATYPE_MISALIGNMENT:    message = "Datatype misalignment";    break; 
    case  EXCEPTION_BREAKPOINT:               message = "Breakpoint";               break;
    case  EXCEPTION_SINGLE_STEP:              message = "Single step";              break;
    case  EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    message = "Array bounds exceeded";    break;
    case  EXCEPTION_NONCONTINUABLE_EXCEPTION: message = "Noncontinuable exception"; break;
    case  EXCEPTION_FLT_DENORMAL_OPERAND:     message = "Float debormal operand";   break;
    case  EXCEPTION_FLT_DIVIDE_BY_ZERO:       message = "Float divide by zero";     break;
    case  EXCEPTION_FLT_INEXACT_RESULT:       message = "Float inexact result";     break;
    case  EXCEPTION_FLT_INVALID_OPERATION:    message = "Float invalid operation";  break;
    case  EXCEPTION_FLT_OVERFLOW:             message = "Float overflow";           break;        
    case  EXCEPTION_FLT_STACK_CHECK:          message = "Float stack check";        break;
    case  EXCEPTION_FLT_UNDERFLOW:            message = "Float underflow";          break;
    case  EXCEPTION_INT_DIVIDE_BY_ZERO:       message = "Integer devide by zero";   break;
    case  EXCEPTION_INT_OVERFLOW:             message = "Integer overflow";         break;
    case  EXCEPTION_PRIV_INSTRUCTION:         message = "Priv instruction";         break;
    case  EXCEPTION_IN_PAGE_ERROR:            message = "In page error";            break;
    case  EXCEPTION_ILLEGAL_INSTRUCTION:      message = "Illegal instruction";      break;
    case  EXCEPTION_STACK_OVERFLOW:           message = "Stack overflow";           break;
    case  EXCEPTION_INVALID_DISPOSITION:      message = "Invalid disposition";      break;
    case  EXCEPTION_GUARD_PAGE:               message = "Guard page";               break;
    case  EXCEPTION_INVALID_HANDLE:           message = "Invalid handle";           break;
    case  CONTROL_C_EXIT:                     message = "User exit (Ctrl+C)";       break;
    default:                                  message = "UNKNOWN";                  break;
  }    
  
  strcpy (buffer,message);

  return buffer;
}

LONG  __stdcall DebugService::Unhandled           (void* ptr)
{
  if (&dout)
  {
    LPEXCEPTION_POINTERS info = (LPEXCEPTION_POINTERS)ptr;

    char  buf [128];
    ulong code = info->ExceptionRecord->ExceptionCode;

    dout<<"Exception: "<<GetExceptionMessage (code,buf)<<endl;
    dbgService->PrintCrash (info->ExceptionRecord->ExceptionAddress,dout);
  }

  return EXCEPTION_EXECUTE_HANDLER;
}

void       DebugService::PrintVars  (ostream& os) const
{
  os<<"Search vars"<<endl;

  Ring<Var*>::iterator iter  = mVars;
  int                  count = 0;

  for (int i=0;i<count;i++,iter++)
    os<<"\t"<<*iter<<endl;
}

void       DebugService::PrintFlags  (ostream& os) const
{
  os<<"Search flags"<<endl;

  Ring<Flag*>::iterator iter  = mFlags;
  int                   count = 0;

  for (int i=0;i<count;i++,iter++)
    os<<"\t"<<*iter<<endl;
}

DebugService::DebugService  (const char* map_file,Pool* pool)
             : Service   (&dbgService),
               mTempPool (pool,DBG_POOL_SIZE),
               mVars     (&mTempPool),
               mFlags    (&mTempPool)
{
  if (map_file)   strcpy (mMapFile,map_file);
  else            mMapFile [0] = 0;

  SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER)Unhandled);
}

DebugService::~DebugService ()
{
  SetUnhandledExceptionFilter (NULL);
}

///////////////////////////////Var//////////////////////////////////////////////////////////

Var::Var(): owner (dbgService), ptr (NULL) { name [0] = 0; }

Var::~Var() 
{  
  if (owner)
    owner->mVars.erase (this);
}

Var::Var(const char* __name,void* __var)
    : owner (dbgService),
      ptr   (__var)
{
  strcpy (name,__name);

  if (owner)
    owner->mVars.insert (this);
}

Var::Var(const Var& var)
    : owner (dbgService),
      ptr   (var.ptr)
{
  strcpy (name,var.name);

  if (owner)
    owner->mVars.insert (this);    
}

Var::operator = (const Var& var)
{
  ptr = var.ptr;
  strcpy (name,var.name);
}

void* Var::GetPtr () const
{
  return ptr;
}

//////////////////////////////Flag///////////////////////////////////////////////////////////

Flag::Flag          ()
     : flag (false)
{
  name [0] = 0;
}

Flag::Flag (const char* flag_name,bool state)
     : flag  (state),
       owner (dbgService)
{
  strcpy (name,flag_name);

  if (owner)
    owner->mFlags.insert (this);
}

Flag::Flag  (const Flag& f)
     : flag (f.flag)
{
  strcpy (name,f.name);

  if (owner)
    owner->mFlags.insert (this);  
}

Flag::operator =    (const Flag& f)
{
  flag = f.flag;
  strcpy (name,f.name);

  if (owner)
    owner->mFlags.insert (this);
}

Flag::operator =    (const bool state)
{
  flag = state;
}

Flag::~Flag         ()
{
  if (owner)
    owner->mFlags.erase (this);
}

Flag::operator bool () const
{
  return flag;
}

/////////////////////////////////////////////////////////////////////////////////////////////

DebugService*   D_Load (const char* map,Pool* pool)
{
  return new (pool) DebugService (map,pool);
}  

void       D_Done (DebugService* srv)
{
  if (srv) delete srv;

}

void*      D_FindLoadAddr  (istream& map)
{
  static char startAddr [] = "Preferred load address is ";
  char   string [2048];

  while (map)
  {           
    map.getline (string,2048);

    char* str = strstr (string,startAddr);

    if (str)
    {
      ulong start = 0;

      str    += strlen (startAddr);
      start   = strtol (str,&str,16);
      
      return (void*)start;
    }
  }  

  return 0;
}

bool       D_FindFuncName 
 ( istream& map,
   void*    _addr,
   char*    funcName,
   char*    fileName
 )
{
  static char table [] = "Address         Publics by Value              Rva+Base     Lib:Object";
  struct lineinfo_t   {
    ulong addr;    
    char  func [1024];
    char  file [1024];
  } info [2];
  char   string [2048];

  ulong addr = (ulong)_addr;
  ulong sel  = 0;
  bool  find = false;

  while (map)
  {
    map.getline (string,2048);

    char* str = strstr (string,table);

    if (str)
    {
      find = true;
      break;
    }
  }

  if (!find)
    return false;

  find = false;

  map.getline (string,2048);

  while (map)
  {
    map.getline (string,2048);

    char* str = string;
   
    strtol   (str,&str,16);  str++; 
    strtol   (str,&str,16); 

    ReadLine (str,info [sel].func);

    info [sel].addr = strtol (str,&str,16);  

    while (ReadLine (str,info [sel].file) <= 1);

    if (addr < info [sel].addr)
    {
      sel = !sel;
      find = true;
      break;
    }  

    sel = !sel;  
  }

  if (!find)
    return false;

  strcpy (funcName,info [sel].func);
  strcpy (fileName,info [sel].file);

  return true;
}

static bool CheckNames (const char* line,char* libName,char* fileName)
{
 //Line numbers for d:\projects\library\wood\filed.lib(zbinfile.cpp) segment .text    
  static char linesHeader [] = "Line numbers for ";

  char* hdr = strstr (line,linesHeader);

  if (!hdr)
    return false;  

  char* lib = strstr (hdr + strlen (linesHeader),libName);

  if (!lib)
    return false;

  char* src = strstr (lib,"(");

  if (!src)   
    return false;

  char* s2 = strstr (s2,fileName);

  if (!s2)
    return false;

  memcpy (libName,lib,src-lib);
  libName [src-lib] = 0;

  char* endSrc = strstr (src,")");

  if (!endSrc)
    return false;

  memcpy (fileName,src+1,endSrc-src-2);
  fileName [endSrc-src-2] = 0;  

  return true;
}

/*void    ParseName (char* fileName,char* libName)
{
  char* s1 = strstr (fileName,":")

  if (s1)
  {  
    memcpy (libName,fileName,s1-fileName);
    libName [s1-fileName] = 0;
//    strcpy ();
  }
  else 
  {
  }
} 

int        D_FindLine     
 ( istream&     map,
   void*        _addr,
   void*        _start,
   char*        libName,
   char*        fileName,
   ulong&       bytes
 )
{
  char string    [2048];  
  ulong addr = (ulong)_addr;
  ulong start = (ulong)_start;

  strcat (lineInfo,fileName);  

  bool  find = false;

  while (map)
  {
    map.getline (string,2048);             

    if (CheckNames (string,libName,fileName))
    {
      find = true;
      break;
    }  
  }  

  if (!find)
    return -1;

  ulong  foffs = addr - start - 0x1000;
  ulong  moffs = 0;  
  int    line  = -1;

  map.getline (string,2048); 

  while (map)
  {
    map.getline (string,2048); 

    while (strlen (string) > 1)
    {
      char* str = string;

      while (*str && *str != '\n' && *str != '\r')
      {
        uint  l    = strtol (str,&str,10);
        ulong seg  = strtol (str,&str,16); str++;
        ulong offs = strtol (str,&str,16);

        if (foffs < offs && offs <= moffs)
        {
          line  = l;
          bytes = offs-foffs;
          moffs = offs;
        }
      }             
    }
  }  

  return line;
}


  */
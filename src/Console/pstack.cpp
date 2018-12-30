#include <pch.h>
#include <console\pstack.h>
#include <string.h>
#include <stdlib.h>

char*        ParamStack::gets        (int i) const
{
  return i < 0 || i >= mCount ? 0 : mPtr [i+1];
}

int          ParamStack::geti        (int i) const
{
  return i < 0 || i >= mCount ? 0 : atoi (mPtr [i+1]);
}

float        ParamStack::getf        (int i) const
{
  return i < 0 || i >= mCount ? 0 : atof (mPtr [i+1]);
}

char*        ParamStack::command     () const
{
  return mPtr [0];
}

char*        ParamStack::next        (char* str,char** dest) const
{
  char* fst = str;

  if (*str == 0)
    return str;

  BOOL open = FALSE;

  for (;*str && *str == ' ';str++);
  for (;*str && (*str != ' ' && !open);str++)
    if (*str == '"' || *str == 0x27) 
      if (!open) open = TRUE;
      else       break;

  if (*fst == '"' || *fst == 0x27) fst++;
  if (*str)                        *str++ = 0;

  *dest = str;
  
  return fst;
}                   

void     ParamStack::refresh     (const char* _str)
{
  D_Try
  {
    mCount = 0;
    strcpy (mStr,_str);
    char* str = mStr;

    int i      = 0;
    char* dest = str;

    for (str=next (dest,&dest);*str && str != dest;str=next (dest,&dest),i++,mCount++)
      mPtr [i] = str;
    mCount--;
  }     
  D_Exc ("ParmStack: Error at refresh");
}
 
ParamStack::ParamStack  (const char* _str)
           : mCount (0)
{
  if (_str)
    refresh (_str);
}

ParamStack::~ParamStack ()
{   }
